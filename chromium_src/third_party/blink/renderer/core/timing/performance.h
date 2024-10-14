/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_H_
#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_H_

class DOMHighResTimeStamp;

#include "base/strings/to_string.h"
#include "third_party/libc++/src/include/__condition_variable/condition_variable.h"

#define now()               \
  now_ChromiumImpl() const; \
  DOMHighResTimeStamp now()

#define ClampTimeResolution(...)                                      \
  ClampTimeResolution(__VA_ARGS__);                                   \
  static DOMHighResTimeStamp BraveMonotonicTimeToDOMHighResTimeStamp( \
      ExecutionContext* context, base::TimeTicks time_origin,         \
      base::TimeTicks monotonic_time, bool allow_negative_value,      \
      bool cross_origin_isolated_capability);                         \
  DOMHighResTimeStamp BraveMonotonicTimeToDOMHighResTimeStamp(        \
      ExecutionContext* context, base::TimeTicks monotonic_time)

#include "src/third_party/blink/renderer/core/timing/performance.h"  // IWYU pragma: export

#undef now
#undef ClampTimeResolution

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_H_
