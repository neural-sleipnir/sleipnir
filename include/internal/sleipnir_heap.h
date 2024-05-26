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

#ifndef SLEIPNIR_INCLUDE_INTERNAL_SLEIPNIR_HEAP_H
#define SLEIPNIR_INCLUDE_INTERNAL_SLEIPNIR_HEAP_H

#include <stddef.h>

#include "sleipnir_common.h"

struct SpHeap;

enum SpResult SpHeapCreate(struct SpHeap *pHeap);
void *SpHeapMalloc(struct SpHeap *pHeap, size_t size);
void SpHeapInit(struct SpHeap *pHeap);
void* spHeapGetEnd(struct SpHeap *pHeap);
void SpHeapDestroy(struct SpHeap *pHeap);

#endif  // SLEIPNIR_INCLUDE_INTERNAL_SLEIPNIR_HEAP_H
