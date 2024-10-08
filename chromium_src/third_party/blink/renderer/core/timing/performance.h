/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_H_
#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_H_

class DOMHighResTimeStamp;

#include "base/strings/to_string.h"
#include "third_party/libc++/src/include/__condition_variable/condition_variable.h"

#define now() \
  now_ChromiumImpl() const; \
  DOMHighResTimeStamp now()

#include "src/third_party/blink/renderer/core/timing/performance.h"  // IWYU pragma: export

#undef now

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_H_
