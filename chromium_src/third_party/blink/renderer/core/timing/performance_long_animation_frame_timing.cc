/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance_long_animation_frame_timing.h"

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"

#define ToMonotonicTime(...) \
  brave::RoundPerformanceIfFarbling(DynamicTo<LocalDOMWindow>(source()), \
                                    ToMonotonicTime(__VA_ARGS__))

#include "src/third_party/blink/renderer/core/timing/performance_long_animation_frame_timing.cc"

#undef ToMonotonicTime