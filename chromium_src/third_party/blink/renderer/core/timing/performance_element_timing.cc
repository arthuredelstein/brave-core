/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance_element_timing.h"

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"

#define render_time_(...) \
  render_time_(brave::RoundPerformanceIfFarbling(source, __VA_ARGS__))

#define load_time_(...) \
  load_time_(           \
      brave::RoundPerformanceIfFarbling(source, __VA_ARGS__))

#include "src/third_party/blink/renderer/core/timing/performance_element_timing.cc"

#undef render_time_
#undef load_time_
