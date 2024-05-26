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

#ifndef SLEIPNIR_INCLUDE_INTERNAL_CONSTANTS_H
#define SLEIPNIR_INCLUDE_INTERNAL_CONSTANTS_H

#define SLEIPNIR_PAGE_SIZE (4096)
#define PAGE_SIZE_MASK (SLEIPNIR_PAGE_SIZE -1)
#define SLEIPNIR_CACHE_LINE_SIZE (64)
#define SLEIPNIR_CACHES_PER_PAGE (64)
#define SLEIPNIR_MIN_INVALIDATES_CARE (10)
#define SLEIPNIR_MAGIC (0xCAFEBABE)

#endif  // SLEIPNIR_INCLUDE_INTERNAL_CONSTANTS_H
