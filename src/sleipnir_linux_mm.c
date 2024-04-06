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
#include <sys/mman.h>

#include "internal/sleipnir_mm.h"

void *spAllocateShared(size_t size, int fd, void *addr) {
  int flags = 0;
  int prot = PROT_READ | PROT_WRITE;

  flags |= ((fd == -1) ? MAP_ANONYMOUS : 0);
  flags |= ((addr != NULL) ? MAP_FIXED : 0);
  flags |= MAP_PRIVATE;

  return mmap(addr, size, prot, flags, fd, 0);
}

int spDeallocateShared(void *ptr, size_t size) { return munmap(ptr, size); }

void *spAllocateCopyOnWrite(size_t size, int fd, void *addr) {
  int flags = 0;
  int prot = PROT_READ | PROT_WRITE;

  flags |= ((fd == -1) ? MAP_ANONYMOUS : 0);
  flags |= ((addr != NULL) ? MAP_FIXED : 0);
  flags |= MAP_SHARED;

  return mmap(addr, size, prot, flags, fd, 0);
}
#endif  // defined(SLEIPNIR_OS_LINUX)
