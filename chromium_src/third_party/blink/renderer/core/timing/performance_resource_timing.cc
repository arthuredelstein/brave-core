/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance_resource_timing.h"

#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/core/timing/performance.h"
#include "third_party/blink/renderer/core/timing/performance_entry.h"

#define is_secure_transport_(...)    \
  is_secure_transport_(__VA_ARGS__), \
      context(brave::AllowFingerprinting(context))

#define MonotonicTimeToDOMHighResTimeStamp(...) \
  MonotonicTimeToDOMHighResTimeStamp(__VA_ARGS__, context)

#define PerformanceEntry(...)                                           \
  PerformanceEntry(__VA_ARGS__, brave::AllowFingerprinting(context))

#include "src/third_party/blink/renderer/core/timing/performance_resource_timing.cc"

#undef is_secure_transport_
#undef MonotonicTimeToDOMHighResTimeStamp
#undef PerformanceEntry
