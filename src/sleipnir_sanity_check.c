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

#include "internal/sleipnir_sanity_check.h"

#include "internal/sleipnir_constants.h"
#include "sleipnir_macros.h"

#ifndef NDEBUG
#if defined(SLEIPNIR_OS_LINUX)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool spSanityCheck(size_t *magic) {
  if (*magic != SLEIPNIR_MAGIC) {
    fprintf(stderr,
            "Sanity check failed in process %d. Current _magic %zx at %p\n",
            getpid(), *magic, &magic);
    abort();
  }
  return true;
}
#else
bool spSanityCheck(size_t *magic) {
  (void)magic;
  return true;
}
#endif  // defined(SLEIPNIR_OS_LINUX)
#endif  // NDEBUG