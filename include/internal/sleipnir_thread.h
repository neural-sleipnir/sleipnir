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

#ifndef SLEIPNIR_INCLUDE_INTERNAL_SLEIPNIR_THREAD_H
#define SLEIPNIR_INCLUDE_INTERNAL_SLEIPNIR_THREAD_H

#include <stdbool.h>
#include <stdint.h>

struct spThread {
  int id;
  int exitStatus;
  bool isForked;
  bool isProtected;
  uint32_t nestingLevel;
};

void spThreadSpawn();
void spThreadJoin();
void spThreadCancel();
void spThreadKill();

#endif  // SLEIPNIR_INCLUDE_INTERNAL_SLEIPNIR_THREAD_H
