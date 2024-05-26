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

#include "internal/sleipnir_obj_header.h"

#include "internal/sleipnir_constants.h"
#include "internal/sleipnir_sanity_check.h"

struct SpObjHeader {
  size_t magic;
  size_t size;
};

size_t spObjHeaderGetSize(struct SpObjHeader *pObjHeader) {
  spSanityCheck(&pObjHeader->magic);
  return pObjHeader->size;
}

bool spObjHeaderIsValidObject(struct SpObjHeader *pObjHeader) {
  spSanityCheck(&pObjHeader->magic);

  bool isValidMagic = spObjHeaderIsValidMagic(pObjHeader);

  if (isValidMagic) {
    isValidMagic = pObjHeader->size % 8 == 0;
  }

  return isValidMagic;
}

bool spObjHeaderIsValidMagic(struct SpObjHeader *pObjHeader) {
  bool isValidMagic = pObjHeader->magic == SLEIPNIR_MAGIC;
  spSanityCheck(&pObjHeader->magic);
  return isValidMagic;
}

size_t spObjHeaderGetSizeof() {
  return sizeof(struct SpObjHeader);
}
