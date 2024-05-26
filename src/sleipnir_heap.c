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

#include "internal/sleipnir_heap.h"

#include "internal/sleipnir_constants.h"
#include "internal/sleipnir_mm.h"
#include "internal/sleipnir_persist.h"
#include "internal/sleipnir_sanity_check.h"
#include "sleipnir_bumpalloc.h"

struct SpHeap {
  struct SpPersist *persist;
  char **position;
  size_t *remaining;
  size_t *magic;
  volatile void *start;
  volatile void *end;
};

enum SpResult SpHeapCreate(struct SpHeap *pHeap) {
  enum SpResult result;
  struct SpPersistCreateInfo persistCreateInfo = {
      .typeSize = sizeof(char),
      .numElements = SLEIPNIR_PAGE_SIZE,
      .addressSize = 0,
      .address = NULL,
  };

  struct SpPersist *pPersist = spBumpAlloc(spPersistGetSizeof());
  if (pPersist == NULL) {
    return SP_FAILURE;
  }
  pHeap->persist = pPersist;

  result = spPersistCreate(pHeap->persist, &persistCreateInfo);
  if (result != SP_SUCCESS) {
    return result;
  }

  char *base = (char *)spSystemAllocateShared(SLEIPNIR_PAGE_SIZE, -1, NULL);

  pHeap->position = (char **)base;
  pHeap->remaining = (size_t *)(base + 1 * sizeof(void *));
  pHeap->magic = (size_t *)(base + 2 * sizeof(void *));
  // TODO ADD: _lock       = new (base + 3*sizeof(void *)) xplock;

  pHeap->start = spPersistGetBase(pPersist);
  pHeap->end = pHeap->start + spPersistGetSize(pPersist);
  *pHeap->position = (char *)pHeap->start;
  *pHeap->remaining = spPersistGetSize(pPersist);
  *pHeap->magic = SLEIPNIR_MAGIC;

  return SP_SUCCESS;
}

void SpHeapDestroy(struct SpHeap *pHeap) {}

void *SpHeapMalloc(struct SpHeap *pHeap, size_t size) {
  spSanityCheck(pHeap->magic);

  // Round up the size to be page-aligned.
  size = SLEIPNIR_PAGE_SIZE *
         ((size + SLEIPNIR_PAGE_SIZE - 1) / SLEIPNIR_PAGE_SIZE);

  // _lock->lock();

  if (*pHeap->remaining < size) {
    return NULL;
  }

  void *p = *pHeap->position;
  *pHeap->remaining -= size;
  *pHeap->position += size;

  // _lock->unlock();

  return p;
}

void SpHeapInit(struct SpHeap *pHeap) {}

void *spHeapGetEnd(struct SpHeap *pHeap) { return *pHeap->position; }
