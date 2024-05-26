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

#include "sleipnir_private_heap.h"

extern void *dlmalloc(size_t size);
extern void dlfree(void *ptr);
extern size_t dlmalloc_usable_size(void *ptr);
extern void *dlrealloc(void *ptr, size_t size);

void *spPrivateHeapAllocate(size_t size) { return dlmalloc(size); }

void spPrivateHeapFree(void *ptr) { dlfree(ptr); }

size_t spPrivateHeapGetSize(void *ptr) { return dlmalloc_usable_size(ptr); }

void *spPrivateHeapReallocate(void *ptr, size_t size) {
  return dlrealloc(ptr, size);
}
