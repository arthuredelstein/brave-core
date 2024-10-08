/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance.h"

#define now() now_ChromiumImpl()

#include "src/third_party/blink/renderer/core/timing/performance.cc"

#undef now

#include "brave/third_party/blink/renderer/brave_farbling_constants.h"
#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"
// #include "third_party/blink/renderer/core/timing/performance_user_timing.h"

namespace blink {

DOMHighResTimeStamp Performance::now() const {
  return brave::RoundPerformanceIfFarbling(GetExecutionContext(),
                                           now_ChromiumImpl());
}

}  // namespace blink
