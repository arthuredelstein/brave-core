/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance_long_animation_frame_timing.h"

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_object_builder.h"
#include "third_party/blink/renderer/core/dom/dom_high_res_time_stamp.h"

#define renderStart renderStart_ChromiumImpl
#define styleAndLayoutStart styleAndLayoutStart_ChromiumImpl
#define firstUIEventTimestamp firstUIEventTimestamp_ChromiumImpl
#define blockingDuration blockingDuration_ChromiumImpl

#define AddNumber(A, B) \
  AddNumber(A, brave::RoundPerformanceIfFarbling(source(), B))

#include "src/third_party/blink/renderer/core/timing/performance_long_animation_frame_timing.cc"

#undef renderStart
#undef styleAndLayoutStart
#undef firstUIEventTimestamp
#undef blockingDuration

#undef AddNumber

namespace blink {

DOMHighResTimeStamp PerformanceLongAnimationFrameTiming::renderStart() const {
  return brave::RoundPerformanceIfFarbling(source(), renderStart_ChromiumImpl());
}

DOMHighResTimeStamp PerformanceLongAnimationFrameTiming::styleAndLayoutStart()
    const {
  return brave::RoundPerformanceIfFarbling(source(), styleAndLayoutStart_ChromiumImpl());
}

DOMHighResTimeStamp PerformanceLongAnimationFrameTiming::firstUIEventTimestamp()
    const {
  return brave::RoundPerformanceIfFarbling(source(), firstUIEventTimestamp_ChromiumImpl());
}

DOMHighResTimeStamp PerformanceLongAnimationFrameTiming::blockingDuration()
    const {
  return brave::RoundPerformanceIfFarbling(source(), blockingDuration_ChromiumImpl());
}

}  // namespace blink