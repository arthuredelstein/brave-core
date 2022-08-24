/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "src/third_party/blink/renderer/core/timing/performance_long_task_timing.cc"

namespace blink {

PerformanceLongTaskTiming::PerformanceLongTaskTiming(
    double start_time,
    int duration,
    const AtomicString& name,
    const AtomicString& culprit_type,
    const AtomicString& culprit_src,
    const AtomicString& culprit_id,
    const AtomicString& culprit_name,
    const uint32_t navigation_id,
    const bool allow_fingerprinting)
    : PerformanceLongTaskTiming(start_time,
                                duration,
                                name,
                                culprit_type,
                                culprit_src,
                                culprit_id,
                                culprit_name,
                                navigation_id) {
  RoundOffTimes(allow_fingerprinting);
}

}  // namespace blink
