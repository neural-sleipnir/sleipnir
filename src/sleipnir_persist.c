// Copyright 2024 Mykola Hohsadze
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "internal/sleipnir_persist.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "internal/sleipnir_mm.h"
#include "internal/sleipnir_pageinfo.h"
#include "internal/sleipnir_word_info.h"
#include "sleipnir_atomic.h"
#include "sleipnir_bumpalloc.h"
#include "sleipnir_cleanup_heap.h"
#include "sleipnir_common.h"
#include "sleipnir_constants.h"
#include "sleipnir_list.h"

struct SpPersist {
  int fd;
  void *pPersistentMemory;
  void *pTransientMemory;
  unsigned long typeSize;
  unsigned long numElements;
  void *address;
  size_t addressSize;
  bool isHeap;
  bool isProtected;
  unsigned long *pCacheInvalidates;
  unsigned long *pCacheLastThread;
  bool *pGlobalSharedInfo;
  bool *pLocalSharedInfo;
  unsigned long *pPageUsers;
  size_t totalPageNums;
  size_t totalCacheNums;
  struct SpWordInfo *pWordChanges;
  struct SpList *pPrivatePages;
  struct SpList *pSavedPages;
};

struct SpDirtyEntry {
  intptr_t pointer;
  void *object;
};

struct SpAddEntry {
  struct SpDirtyEntry dirtyEntry;
  bool isExist;
};

enum SpResult spPersistCreate(struct SpPersist *pPersist,
                              struct SpPersistCreateInfo *pInfo) {
  unsigned long elementBytes = pPersist->numElements * pPersist->typeSize;

  if (pInfo->address > 0 && pInfo->addressSize > elementBytes) {
    return SP_FAILURE;
  }

  pPersist->address = pInfo->address;
  pPersist->addressSize = pInfo->addressSize;
  pPersist->typeSize = pInfo->typeSize;
  pPersist->numElements = pInfo->numElements;
  pPersist->totalPageNums =
      pInfo->numElements * pInfo->typeSize / SLEIPNIR_PAGE_SIZE;
  pPersist->totalCacheNums =
      pInfo->numElements * pInfo->typeSize / SLEIPNIR_CACHE_LINE_SIZE;

  char backingFileName[L_tmpnam];
  snprintf(backingFileName, L_tmpnam, "/tmp/sp-XXXXXX");

  pPersist->fd = mkstemp(backingFileName);
  if (pPersist->fd == -1) {
    return SP_FAILURE;
  }

  if (ftruncate(pPersist->fd, (long)elementBytes) == -1) {
    return SP_FAILURE;
  }

  if (unlink(backingFileName) == -1) {
    return SP_FAILURE;
  }

  pPersist->pPersistentMemory =
      spSystemAllocateShared(elementBytes, pPersist->fd, NULL);

  if (pPersist->pPersistentMemory == MAP_FAILED) {
    return SP_FAILURE;
  }

  if (pPersist->address != NULL) {
    memcpy(pPersist->pPersistentMemory, pPersist->address,
           pPersist->addressSize);
    pPersist->addressSize =
        (pPersist->addressSize / SLEIPNIR_PAGE_SIZE + 1) * SLEIPNIR_PAGE_SIZE;
    pPersist->isHeap = false;
  } else {
    pPersist->isHeap = true;
  }

  pPersist->pTransientMemory =
      spSystemAllocateShared(elementBytes, pPersist->fd, pPersist->address);

  if (pPersist->pTransientMemory == MAP_FAILED) {
    return SP_FAILURE;
  }

  pPersist->pCacheLastThread = (unsigned long *)spSystemAllocateShared(
      pPersist->totalCacheNums * sizeof(unsigned long), -1, NULL);

  pPersist->pCacheInvalidates = (unsigned long *)spSystemAllocateShared(
      pPersist->totalCacheNums * sizeof(unsigned long), -1, NULL);

  pPersist->pPageUsers = (unsigned long *)spSystemAllocateShared(
      pPersist->totalPageNums * sizeof(unsigned long), -1, NULL);

  pPersist->pWordChanges =
      (struct SpWordInfo *)spSystemAllocateShared(elementBytes, -1, NULL);

  if (pPersist->isHeap) {
    const size_t persistSize = spPersistGetSize(pPersist);
    struct SpCleanupHeap *pCleanupHeap = spCleanupHeapGetInstance();
    struct SpCleanupHeapStoreInfo cleanupHeapStoreInfo = {
        .pHeapStart = pPersist->pTransientMemory,
        .heapSize = persistSize,
        .pCacheInvalidates = pPersist->pCacheInvalidates,
        .pCacheLastThread = pPersist->pCacheLastThread,
        .pWordChanges = pPersist->pWordChanges,
    };
    spCleanupHeapStore(pCleanupHeap, &cleanupHeapStoreInfo);
  }

  return SP_SUCCESS;
}

void spPersistDestroy(struct SpPersist *pPersist) {
  close(pPersist->fd);
  spSystemDeallocateShared(pPersist->pTransientMemory,
                           pPersist->numElements * pPersist->typeSize);
  spSystemDeallocateShared(pPersist->pPersistentMemory,
                           pPersist->numElements * pPersist->typeSize);
}

void *spPersistGetBase(struct SpPersist *pPersist) {
  return pPersist->pTransientMemory;
}

size_t spPersistGetSize(struct SpPersist *pPersist) {
  return pPersist->isHeap ? pPersist->numElements * pPersist->typeSize
                          : pPersist->addressSize;
}

size_t spPersistGetSizeof() { return sizeof(struct SpPersist); }

static void spCheckCommitWord(const char *local, const char *twin,
                              char *share) {
  int i = 0;
  while (i < sizeof(unsigned long)) {
    if (local[i] != twin[i]) {
      share[i] = local[i];
    }
    i++;
  }
}

static void spAddPageEntry(int pageNo, struct SpPageInfo *pCurrentPage,
                           struct SpList *pPageList) {
  struct SpAddEntry *pEntry =
      (struct SpAddEntry *)spBumpAlloc(sizeof(struct SpAddEntry));

  pEntry->dirtyEntry = (struct SpDirtyEntry){
      .pointer = (intptr_t)pageNo,
      .object = pCurrentPage,
  };
  pEntry->isExist = false;

  spListAdd(pPageList, pEntry);
  if (!pEntry->isExist) {
    // The element is existing in the list now.
    memcpy((void *)pEntry->dirtyEntry.object, pCurrentPage,
           sizeof(struct SpPageInfo));
  }
}

static unsigned int calcCacheNo(int words) {
  return (words * sizeof(unsigned int)) / SLEIPNIR_CACHE_LINE_SIZE;
}

static void spCommitPageDiffs(struct SpPersist *pPersist, const void *local,
                              const void *twin, int pageNo) {
  void *dest = (void *)((intptr_t)pPersist->pPersistentMemory +
                        SLEIPNIR_PAGE_SIZE * pageNo);

  unsigned long *mylocal = (unsigned long *)local;
  unsigned long *mytwin = (unsigned long *)twin;
  unsigned long *mydest = (unsigned long *)dest;

  for (int i = 0; i < SLEIPNIR_PAGE_SIZE / sizeof(unsigned long); i++) {
    if (mylocal[i] != mytwin[i]) {
      spCheckCommitWord(mylocal, mytwin, mydest);
    }
  }
}

static int spRecordCacheInvalidates(struct SpPersist *pPersist,
                                    unsigned long cacheNo) {
  int myTid = getpid();
  int lastTid;
  int interleaving = 0;

  // Try to check the global array about cache last thread id.
  lastTid =
      sp_atomic_exchange_relaxed(&pPersist->pCacheLastThread[cacheNo], myTid);

  if (lastTid != 0 && lastTid != myTid) {
    sp_atomic_inc_relaxed(&pPersist->pCacheInvalidates[cacheNo]);
    interleaving = 1;
  }
  return interleaving;
}

static void spRecordWordChanges(void *addr, int changes) {
  struct SpWordInfo *word = (struct SpWordInfo *)addr;
  unsigned short tid = word->tid;

  int mine = getpid();

  if (tid == 0) {
    word->tid = mine;
    word->version = 0;
  } else if (tid != 0 && tid != mine && tid != 0xFFFF) {
    word->tid = 0xFFFF;
  }

  word->version += changes;
}

static void spCheckCommitPage(struct SpPersist *pPersist,
                              struct SpPageInfo *pPageInfo) {
  int *twin = (int *)pPageInfo->origTwinPage;
  int *local = (int *)pPageInfo->pageStart;
  int *share = (int *)((intptr_t)pPersist->pPersistentMemory +
                       SLEIPNIR_PAGE_SIZE * pPageInfo->pageNo);
  int *tempTwin = (int *)pPageInfo->tempTwinPage;
  int *localChanges = (int *)pPageInfo->wordChanges;
  int *globalChanges = (int *)((intptr_t)pPersist->pWordChanges +
                               SLEIPNIR_PAGE_SIZE * pPageInfo->pageNo);
  unsigned long recordedCacheNo = 0xFFFFFF00;
  unsigned long cacheNo;
  unsigned long interWrites = 0;

  for (int i = 0; i < SLEIPNIR_PAGE_SIZE / sizeof(int); i++) {
    if (local[i] == twin[i]) {
      if (localChanges[i] != 0) {
        spRecordWordChanges((void *)&globalChanges[i], localChanges[i]);
      }
      continue;
    }

    if (local[i] != tempTwin[i]) {
      cacheNo = calcCacheNo(i);

      if (cacheNo != recordedCacheNo) {
        spRecordCacheInvalidates(
            pPersist, pPageInfo->pageNo * SLEIPNIR_CACHES_PER_PAGE + cacheNo);

        recordedCacheNo = cacheNo;
      }
      spRecordWordChanges((void *)&globalChanges[i], localChanges[i] + 1);
    } else {
      spRecordWordChanges((void *)&globalChanges[i], localChanges[i]);
    }

    spCheckCommitWord((char *)&local[i], (char *)&twin[i], (char *)&share[i]);
  }
}

void spPersistCommit(struct SpPersist *pPersist) {
  struct SpPageInfo *pPageInfo = NULL;
  int pageNumber;

  struct SpListNode *pos = NULL;
  SP_LIST_FOREACH(pos, pPersist->pPrivatePages) {
    struct SpDirtyEntry *pEntry = (struct SpDirtyEntry *)pos;
    pPageInfo = (struct SpPageInfo *)pEntry->pointer;
    pageNumber = pPageInfo->pageNo;

    if (pPageInfo->alloced) {
      spCheckCommitPage(pPersist, pPageInfo);
    } else {
      spCommitPageDiffs(pPersist, pPageInfo->pageStart, pPageInfo->origTwinPage,
                        pageNumber);
    }
  }
}

static size_t spComputePage(struct SpPersist *pPersist, size_t index) {
  return (index * pPersist->typeSize) / SLEIPNIR_PAGE_SIZE;
}

void spPersistHandleWriteAccess(struct SpPersist *pPersist, void *pAddr) {
  unsigned long *pageStart =
      (unsigned long *)(((intptr_t)pAddr) & ~(PAGE_SIZE_MASK));

  // Unprotect the page and record the write.
  mprotect((char *)pageStart, SLEIPNIR_PAGE_SIZE, PROT_READ | PROT_WRITE);

  // Compute the page number of this item
  size_t pageNo = spComputePage(
      pPersist, (size_t)pAddr - (size_t)spPersistGetBase(pPersist));

  // printf("handlePAGEWRITE: addr %p pageNO %d\n", addr, pageNo);

  // Get an entry from page store.
  struct SpPageInfo *currentPage = xpageentry::getInstance().alloc();
  currentPage->pageNo = pageNo;
  currentPage->pageStart = (void *)pageStart;
  currentPage->alloced = false;
}
