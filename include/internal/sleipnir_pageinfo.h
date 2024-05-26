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

#ifndef SLEIPNIR_INCLUDE_INTERNAL_SLEIPNIR_PAGEINFO_H
#define SLEIPNIR_INCLUDE_INTERNAL_SLEIPNIR_PAGEINFO_H

#include <stdint.h>
#include <stdbool.h>

struct SpPageInfo {
  size_t pageNo;
  int version;
  void* pageStart;
  void* origTwinPage;
  void* tempTwinPage;
  unsigned long* wordChanges;
  bool shared;
  bool alloced;
  bool hasTwinPage;
};

#endif  // SLEIPNIR_INCLUDE_INTERNAL_SLEIPNIR_PAGEINFO_H
