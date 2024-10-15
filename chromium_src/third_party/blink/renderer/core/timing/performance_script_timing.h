/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_SCRIPT_TIMING_H_
#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_SCRIPT_TIMING_H_

#define executionStart()  \
  executionStart() const; \
  DOMHighResTimeStamp executionStart_ChromiumImpl()

#define forcedStyleAndLayoutDuration()  \
  forcedStyleAndLayoutDuration() const; \
  DOMHighResTimeStamp forcedStyleAndLayoutDuration_ChromiumImpl()

#define pauseDuration()  \
  pauseDuration() const; \
  DOMHighResTimeStamp pauseDuration_ChromiumImpl()

#include "src/third_party/blink/renderer/core/timing/performance_script_timing.h"  // IWYU pragma: export

#undef executionStart
#undef forcedStyleAndLayoutDuration
#undef pauseDuration

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_SCRIPT_TIMING_H_
