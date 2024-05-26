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

#include "internal/sleipnir_timeinfo.h"

#include <limits.h>
#include <time.h>

struct SpTimeInfo {
  unsigned long low;
  unsigned long high;
};

#define SLEIPNIR_CPU_FREQ (2327507.08008)

static void getTime(struct SpTimeInfo *ti) {
  unsigned int tlow, thigh;

  asm volatile("rdtsc" : "=a"(tlow), "=d"(thigh));

  ti->low = tlow;
  ti->high = thigh;
}

static double countElapse(struct SpTimeInfo *start, struct SpTimeInfo *stop) {
  double elapsed = (double)(stop->low - start->low) +
                   (double)(UINT_MAX) * (double)(stop->high - start->high);
  if (stop->low < start->low) elapsed -= (double)UINT_MAX;

  return elapsed;
}

static double getElapse(struct SpTimeInfo *start, struct SpTimeInfo *stop) {
  double elapse = countElapse(start, stop);

  return elapse;
}

void spTimeInfoStart(struct SpTimeInfo *ti) { getTime(ti); }

double spTimeInfoStop(struct SpTimeInfo *begin, struct SpTimeInfo *end) {
  double elapse;
  struct SpTimeInfo stop_ti;

  if (end == NULL) {
    getTime(&stop_ti);
    elapse = getElapse(begin, &stop_ti);
  } else {
    getTime(end);
    elapse = getElapse(begin, end);
  }

  return elapse;
}

unsigned long spTimeInfoElapsed2ms(double elapsed) {
  unsigned long ms = (unsigned long)(elapsed / SLEIPNIR_CPU_FREQ);
  return (ms);
}
