/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance_entry.h"

#include "src/third_party/blink/renderer/core/timing/performance_entry.cc"

namespace blink {

void PerformanceEntry::RoundOffTimes() {
  printf("RoundOffTimes: start_time_: %f\n", start_time_);
  start_time_ = round(start_time_);
  duration_ = round(duration_);
}

PerformanceEntry::PerformanceEntry(const AtomicString& name,
                                   double start_time,
                                   double finish_time,
                                   uint32_t navigation_id,
                                   bool allow_fingerprinting)
    : PerformanceEntry(name, start_time, finish_time, navigation_id) {
  allow_fingerprinting_ = allow_fingerprinting;
}
PerformanceEntry::PerformanceEntry(double duration,
                                   const AtomicString& name,
                                   double start_time,
                                   uint32_t navigation_id,
                                   bool allow_fingerprinting)
    : PerformanceEntry(duration, name, start_time, navigation_id) {
  allow_fingerprinting_ = allow_fingerprinting;
}

}  // namespace blink
