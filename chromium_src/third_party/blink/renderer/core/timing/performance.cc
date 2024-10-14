/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance.h"

#define now() now_ChromiumImpl()

#include "src/third_party/blink/renderer/core/timing/performance.cc"

#undef now

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"

namespace blink {

DOMHighResTimeStamp Performance::now() const {
  return brave::RoundPerformanceIfFarbling(GetExecutionContext(),
                                           now_ChromiumImpl());
}

// static
DOMHighResTimeStamp Performance::BraveMonotonicTimeToDOMHighResTimeStamp(
    ExecutionContext* context,
    base::TimeTicks time_origin,
    base::TimeTicks monotonic_time,
    bool allow_negative_value,
    bool cross_origin_isolated_capability) {
  return brave::RoundPerformanceIfFarbling(context,
    MonotonicTimeToDOMHighResTimeStamp(time_origin, monotonic_time, allow_negative_value, cross_origin_isolated_capability));
}

}  // namespace blink
