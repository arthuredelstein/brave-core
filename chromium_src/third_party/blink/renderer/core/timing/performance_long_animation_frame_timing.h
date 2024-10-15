/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_LONG_ANIMATION_FRAME_TIMING_H_
#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_LONG_ANIMATION_FRAME_TIMING_H_

#define renderStart() \
    renderStart() const; \
    DOMHighResTimeStamp renderStart_ChromiumImpl()

#define styleAndLayoutStart() \
    styleAndLayoutStart() const; \
    DOMHighResTimeStamp styleAndLayoutStart_ChromiumImpl()

#define firstUIEventTimestamp() \
    firstUIEventTimestamp() const; \
    DOMHighResTimeStamp firstUIEventTimestamp_ChromiumImpl()

#define blockingDuration() \
    blockingDuration() const; \
    DOMHighResTimeStamp blockingDuration_ChromiumImpl()

#include "src/third_party/blink/renderer/core/timing/performance_long_animation_frame_timing.h"  // IWYU pragma: export

#undef renderStart
#undef styleAndLayoutStart
#undef firstUIEventTimestamp
#undef blockingDuration

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_LONG_ANIMATION_FRAME_TIMING_H_
