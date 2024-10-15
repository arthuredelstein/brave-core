/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/layout_shift.h"

#define value_(...) \
  value(brave::RoundPerformanceIfFarbling(source, __VA_ARGS__))

#define most_recent_input_timestamp_(...) \
  most_recent_input_timestamp_(           \
      brave::RoundPerformanceIfFarbling(source, __VA_ARGS__))

#include "src/third_party/blink/renderer/core/timing/layout_shift.cc"

#undef value_
#undef most_recent_input_timestamp_
