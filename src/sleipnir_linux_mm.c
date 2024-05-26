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

#include "sleipnir_macros.h"

#if defined(SLEIPNIR_OS_LINUX)
#include <stdint.h>
#include <sys/mman.h>

#include "internal/sleipnir_mm.h"

void *spSystemAllocateShared(size_t size, int fd, void *addr) {
  int flags = 0;
  int prot = PROT_READ | PROT_WRITE;

  flags |= ((fd == -1) ? MAP_ANONYMOUS : 0);
  flags |= ((addr != NULL) ? MAP_FIXED : 0);
  flags |= MAP_SHARED;

  return mmap(addr, size, prot, flags, fd, 0);
}

int spSystemDeallocateShared(void *ptr, size_t size) { return munmap(ptr, size); }

void *spSystemAllocateCopyOnWrite(size_t size, int fd, void *addr) {
  int flags = 0;
  int prot = PROT_READ | PROT_WRITE;

  flags |= ((fd == -1) ? MAP_ANONYMOUS : 0);
  flags |= ((addr != NULL) ? MAP_FIXED : 0);
  flags |= MAP_PRIVATE;

  return mmap(addr, size, prot, flags, fd, 0);
}

void *spSystemChangeMapping(bool isShared, int prot, void *addr,
                      size_t len, int fd, int offset) {
  int flags = isShared ? MAP_SHARED : MAP_PRIVATE;
  flags |= MAP_FIXED;

  return mmap(addr, len, prot, flags, fd, offset);
}

void *spSystemChangeMappingToShared(int prot, void *addr, size_t len, int fd) {
  int  offset = (intptr_t)addr - (intptr_t)base();
  return spSystemChangeMapping(true, prot, addr, len, fd, offset);
}

#endif  // defined(SLEIPNIR_OS_LINUX)
