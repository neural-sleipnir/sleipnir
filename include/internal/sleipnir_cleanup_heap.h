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

#ifndef SLEIPNIR_INCLUDE_INTERNAL_CLEANUP_HEAP_H
#define SLEIPNIR_INCLUDE_INTERNAL_CLEANUP_HEAP_H

#include <stdbool.h>
#include <stddef.h>

struct SpCleanupHeap;

struct SpCleanupHeapStoreInfo {
  void *pHeapStart;
  size_t heapSize;
  void *pCacheInvalidates;
  void *pCacheLastThread;
  void *pWordChanges;
};

struct SpCleanupHeapCleanObjectInfo {
  void *pAddr;
  size_t size;
  bool sameCallSite;
};

struct SpCleanupHeap *spCleanupHeapGetInstance();
void *spCleanupHeapGetHeap(struct SpCleanupHeap *pCleanupHeap);
size_t spCleanupHeapGetHeapSize(struct SpCleanupHeap *pCleanupHeap);
void spCleanupHeapStore(struct SpCleanupHeap *pCleanupHeap,
                        struct SpCleanupHeapStoreInfo *pInfo);
bool spCleanupHeapIsAddressInRange(struct SpCleanupHeap *pCleanupHeap,
                                   void *pAddr);

bool spCleanupHeapCleanObject(struct SpCleanupHeap *pCleanupHeap,
                              struct SpCleanupHeapCleanObjectInfo *pInfo);

#endif  // SLEIPNIR_INCLUDE_INTERNAL_CLEANUP_HEAP_H
