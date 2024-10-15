/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/largest_contentful_paint.h"

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"

#define render_time_(...) \
  render_time_(brave::RoundPerformanceIfFarbling(source, __VA_ARGS__))

#define load_time_(...) \
  load_time_(brave::RoundPerformanceIfFarbling(source, __VA_ARGS__))

#define first_animated_frame_time_(...) \
  first_animated_frame_time_(brave::RoundPerformanceIfFarbling(source, __VA_ARGS__))

#include "src/third_party/blink/renderer/core/timing/largest_contentful_paint.cc"

#undef render_time_
#undef load_time_
#undef first_animated_frame_time_
