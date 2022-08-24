/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_LONG_TASK_TIMING_H_

#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_LONG_TASK_TIMING_H_

#include "third_party/blink/renderer/core/timing/performance_entry.h"

#define entryType                                                       \
  *dummy() { return nullptr; }                                          \
  PerformanceLongTaskTiming(double start_time,                          \
                            int duration,                               \
                            const AtomicString& name,                   \
                            const AtomicString& culprit_type,           \
                            const AtomicString& culprit_src,            \
                            const AtomicString& culprit_id,             \
                            const AtomicString& culprit_name,           \
                            const uint32_t navigation_id,               \
                            bool allow_fingerprinting);                 \
  AtomicString entryType

#include "src/third_party/blink/renderer/core/timing/performance_long_task_timing.h"

#undef entryType

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_LONG_TASK_TIMING_H_
