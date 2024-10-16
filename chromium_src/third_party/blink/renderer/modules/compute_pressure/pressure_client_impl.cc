/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/modules/compute_pressure/pressure_client_impl.h"

#include "third_party/blink/renderer/core/timing/performance.h"

#define MonotonicTimeToDOMHighResTimeStamp(...) \
  BraveMonotonicTimeToDOMHighResTimeStamp(context, __VA_ARGS__)

#include "src/third_party/blink/renderer/modules/compute_pressure/pressure_client_impl.cc"

#undef MonotonicTimeToDOMHighResTimeStamp

