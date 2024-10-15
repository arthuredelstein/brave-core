/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance_script_timing.h"

#include "third_party/blink/renderer/bindings/core/v8/v8_object_builder.h"

#define executionStart executionStart_ChromiumImpl
#define forcedStyleAndLayoutDuration forcedStyleAndLayoutDuration_ChromiumImpl
#define pauseDuration pauseDuration_ChromiumImpl

#define AddNumber(A, B) \
  AddNumber(A, brave::RoundPerformanceIfFarbling(source(), B))

#include "src/third_party/blink/renderer/core/timing/performance_script_timing.cc"

#undef executionStart
#undef forcedStyleAndLayoutDuration
#undef pauseDuration

#undef AddNumber

namespace blink {

DOMHighResTimeStamp PerformanceScriptTiming::executionStart() const {
  return brave::RoundPerformanceIfFarbling(source(),
                                           executionStart_ChromiumImpl());
}

DOMHighResTimeStamp PerformanceScriptTiming::forcedStyleAndLayoutDuration()
    const {
  return brave::RoundPerformanceIfFarbling(
      source(), forcedStyleAndLayoutDuration_ChromiumImpl());
}

DOMHighResTimeStamp PerformanceScriptTiming::pauseDuration() const {
  return brave::RoundPerformanceIfFarbling(source(),
                                           pauseDuration_ChromiumImpl());
}

}  // namespace blink
