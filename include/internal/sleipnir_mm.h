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

#ifndef SLEIPNIR_SLEIPNIR_MM_H
#define SLEIPNIR_SLEIPNIR_MM_H

#include <stdbool.h>
#include <stddef.h>

void *spAllocateShared(size_t size, int fd, void *addr);
int spDeallocateShared(void *ptr, size_t size);
void *spAllocateCopyOnWrite(size_t size, int fd, void *addr);
void *spChangeMapping(bool isShared, int prot, void *addr,
                      size_t len, int fd, int offset);

void *spChangeMappingToShared(int prot, void *addr, size_t len, int fd);

#endif  // SLEIPNIR_SLEIPNIR_MM_H
