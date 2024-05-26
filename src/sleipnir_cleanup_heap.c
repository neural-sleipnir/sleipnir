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

#include "sleipnir_cleanup_heap.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "sleipnir_constants.h"
#include "sleipnir_obj_header.h"

struct SpCleanupHeap {
  void *pHeapStart;
  size_t heapSize;
  unsigned long *pCacheInvalidates;
  unsigned long *pCacheLastThread;
  unsigned long *pWordChanges;
};

static char buf[sizeof(struct SpCleanupHeap)];
static struct SpCleanupHeap *pCleanup = NULL;

struct SpCleanupHeap *spCleanupHeapGetInstance() {
  if (pCleanup == NULL) {
    pCleanup = (struct SpCleanupHeap *)buf;
    pCleanup->heapSize = 0;
    pCleanup->pHeapStart = NULL;
    pCleanup->pCacheInvalidates = NULL;
    pCleanup->pCacheLastThread = NULL;
    pCleanup->pWordChanges = NULL;
  }
  return pCleanup;
}

void *spCleanupHeapGetHeap(struct SpCleanupHeap *pCleanupHeap) {
  return pCleanupHeap->pHeapStart;
}

size_t spCleanupHeapGetHeapSize(struct SpCleanupHeap *pCleanupHeap) {
  return pCleanupHeap->heapSize;
}

void spCleanupHeapStore(struct SpCleanupHeap *pCleanupHeap,
                        struct SpCleanupHeapStoreInfo *pInfo) {
  pCleanupHeap->heapSize = pInfo->heapSize;
  pCleanupHeap->pHeapStart = pInfo->pHeapStart;
  pCleanupHeap->pCacheInvalidates = pInfo->pCacheInvalidates;
  pCleanupHeap->pCacheLastThread = pInfo->pCacheLastThread;
  pCleanupHeap->pWordChanges = pInfo->pWordChanges;
}

bool spCleanupHeapIsAddressInRange(struct SpCleanupHeap *pCleanupHeap,
                                   void *pAddr) {
  const size_t base = (size_t)spCleanupHeapGetHeap(pCleanupHeap);
  const size_t size = (size_t)spCleanupHeapGetHeapSize(pCleanupHeap);
  const size_t addr = (size_t)pAddr;

  return addr >= base && addr < base + size;
}

bool spCleanupHeapCleanObject(struct SpCleanupHeap *pCleanupHeap,
                              struct SpCleanupHeapCleanObjectInfo *pInfo) {
  intptr_t offset =
      (intptr_t)pInfo->pAddr - (intptr_t)spCleanupHeapGetHeap(pCleanupHeap);
  size_t cacheLines = pInfo->size / SLEIPNIR_CACHE_LINE_SIZE;
  size_t index = offset / SLEIPNIR_CACHE_LINE_SIZE;
  unsigned long *pCacheInvalidates = pCleanupHeap->pCacheInvalidates;

  if (cacheLines == 0) {
    cacheLines = 1;
  }

  if (!pInfo->sameCallSite) {
    for (size_t i = index; i < index + cacheLines; i++) {
      if (pCacheInvalidates[i] >= SLEIPNIR_MIN_INVALIDATES_CARE) return false;
      pCacheInvalidates[i] = 0;
    }

    unsigned idx = (offset - spObjHeaderGetSizeof()) / sizeof(unsigned long);
    void *wordptr = (void *)&pCleanupHeap->pWordChanges[idx];
    memset(wordptr, 0, pInfo->size);
    return true;
  }
  for (size_t i = index; i < index + cacheLines; i++) {
    if (pCacheInvalidates[i] >= SLEIPNIR_MIN_INVALIDATES_CARE)
      pCacheInvalidates[i] -= 1;
    pCacheInvalidates[i] = 0;
  }

  return true;
}
