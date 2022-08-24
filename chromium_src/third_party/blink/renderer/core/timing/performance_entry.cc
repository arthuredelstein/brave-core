/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance_entry.h"

#include "src/third_party/blink/renderer/core/timing/performance_entry.cc"

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"

namespace blink {

void PerformanceEntry::RoundOffTimes(bool allow_fingerprinting) {
  if (!allow_fingerprinting) {
    start_time_ = brave::RoundTimeStamp(start_time_);
    duration_ = brave::RoundTimeStamp(duration_);
  }
}

PerformanceEntry::PerformanceEntry(const AtomicString& name,
                                   double start_time,
                                   double finish_time,
                                   uint32_t navigation_id,
                                   bool allow_fingerprinting)
    : PerformanceEntry(name, start_time, finish_time, navigation_id) {
  RoundOffTimes(allow_fingerprinting);
}
PerformanceEntry::PerformanceEntry(double duration,
                                   const AtomicString& name,
                                   double start_time,
                                   uint32_t navigation_id,
                                   bool allow_fingerprinting)
    : PerformanceEntry(duration, name, start_time, navigation_id) {
  RoundOffTimes(allow_fingerprinting);
}

}  // namespace blink
