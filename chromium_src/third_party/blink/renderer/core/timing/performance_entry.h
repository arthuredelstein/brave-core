/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_ENTRY_H_

#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_ENTRY_H_

// Prevent #define collisions
#include "third_party/blink/public/platform/scheduler/web_scoped_virtual_time_pauser.h"
#include "third_party/blink/renderer/core/timing/performance_server_timing.h"
#include "third_party/blink/renderer/platform/network/server_timing_header.h"

#define IsResource                              \
  dummy_ [[maybe_unused]] = false;              \
  void RoundOffTimes(bool allowFingerprinting); \
  bool isResource

// Remove constness of start_time_.
#define start_time_                 \
  dummy2_ [[maybe_unused]] = false; \
  double start_time_

#define duration_                                               \
  duration_;                                                    \
  PerformanceEntry(const AtomicString& name, double start_time, \
                   double finish_time, uint32_t navigation_id,  \
                   bool allow_fingerprinting);                  \
  PerformanceEntry(double duration, const AtomicString& name,   \
                   double start_time, uint32_t navigation_id,   \
                   bool allow_fingerprinting)

#include "src/third_party/blink/renderer/core/timing/performance_entry.h"

#undef IsResource
#undef start_time_
#undef duration_

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_ENTRY_H_
