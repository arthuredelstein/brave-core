/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_H_

#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_H_

#include "third_party/blink/renderer/core/timing/largest_contentful_paint.h"
#include "third_party/blink/renderer/core/timing/layout_shift.h"
#include "third_party/blink/renderer/core/timing/performance_element_timing.h"
#include "third_party/blink/renderer/core/timing/performance_long_task_timing.h"
#include "third_party/blink/renderer/core/timing/performance_resource_timing.h"
#include "third_party/blink/renderer/core/timing/performance_event_timing.h"

namespace blink {
class ExecutionContext;
}  // namespace blink

#define now()                                                             \
  MonotonicTimeToDOMHighResTimeStamp_ChromiumImpl(base::TimeTicks) const; \
  DOMHighResTimeStamp now_ChromiumImpl() const;                           \
  DOMHighResTimeStamp now()

#define cross_origin_isolated_capability_ \
  cross_origin_isolated_capability_;      \
  bool allow_fingerprinting_

#define ClampTimeResolution                                              \
  MonotonicTimeToDOMHighResTimeStamp_ChromiumImpl(                       \
      base::TimeTicks time_origin, base::TimeTicks monotonic_time,       \
      bool allow_negative_value, bool cross_origin_isolated_capability); \
  static DOMHighResTimeStamp MonotonicTimeToDOMHighResTimeStamp(         \
      base::TimeTicks time_origin, base::TimeTicks monotonic_time,       \
      bool allow_negative_value, bool cross_origin_isolated_capability,  \
      bool allow_fingerprinting);                                        \
  static DOMHighResTimeStamp MonotonicTimeToDOMHighResTimeStamp(         \
      base::TimeTicks time_origin, base::TimeTicks monotonic_time,       \
      bool allow_negative_value, bool cross_origin_isolated_capability,  \
      ExecutionContext* context);                                        \
  static DOMHighResTimeStamp ClampTimeResolution

#define timeOrigin                 \
  timeOrigin_ChromiumImpl() const; \
  DOMHighResTimeStamp timeOrigin

#define BuildJSONValue                                 \
  BuildJSONValue_ChromiumImpl(V8ObjectBuilder&) const; \
  virtual void BuildJSONValue

#include "src/third_party/blink/renderer/core/timing/performance.h"

#undef now
#undef cross_origin_isolated_capability_
#undef ClampTimeResolution
#undef timeOrigin
#undef BuildJSONValue

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_H_
