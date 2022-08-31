/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "src/third_party/blink/renderer/core/timing/performance_paint_timing.cc"

PerformancePaintTiming::PerformancePaintTiming(PaintType type,
                                               double start_time,
                                               uint32_t navigation_id)
    : PerformanceEntry(FromPaintTypeToString(type),
                       start_time,
                       start_time,
                       navigation_id,
                       allow_fingerprinting) {}
