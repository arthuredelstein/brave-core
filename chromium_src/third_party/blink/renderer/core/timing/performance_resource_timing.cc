/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance_resource_timing.h"

#include "third_party/blink/renderer/core/timing/performance.h"
#include "third_party/blink/renderer/core/timing/performance_entry.h"

#define MonotonicTimeToDOMHighResTimeStamp(...)                                \
  BraveMonotonicTimeToDOMHighResTimeStamp(DynamicTo<LocalDOMWindow>(source()), \
                                          __VA_ARGS__)

// Put back the original MonotonicTimeToDOMHighResTimeStamp in the constructor.
#define PerformanceEntry(PARAM1, PARAM2, PARAM3, PARAM4)                \
  PerformanceEntry(                                                     \
      PARAM1,                                                           \
      Performance::MonotonicTimeToDOMHighResTimeStamp(                  \
          time_origin, info->start_time, info->allow_negative_values,   \
          cross_origin_isolated_capability),                            \
      Performance::MonotonicTimeToDOMHighResTimeStamp(                  \
          time_origin, info->response_end, info->allow_negative_values, \
          cross_origin_isolated_capability),                            \
      PARAM4)

#include "src/third_party/blink/renderer/core/timing/performance_resource_timing.cc"

#undef MonotonicTimeToDOMHighResTimeStamp
#undef PerformanceEntry
