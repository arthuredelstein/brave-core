/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance_entry.h"

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"

#define duration_(...)                         \
  duration_(brave::RoundPerformanceIfFarbling( \
      DynamicTo<LocalDOMWindow>(source), __VA_ARGS__))

#define start_time_(...)                         \
  start_time_(brave::RoundPerformanceIfFarbling( \
      DynamicTo<LocalDOMWindow>(source), __VA_ARGS__))

#include "src/third_party/blink/renderer/core/timing/performance_entry.cc"

#undef duration_
#undef start_time_

namespace blink {}  // namespace blink
