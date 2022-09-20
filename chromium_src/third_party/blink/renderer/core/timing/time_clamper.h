/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_TIME_CLAMPER_H_

#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_TIME_CLAMPER_H_

#define kCoarseResolutionMicroseconds kCoarseResolutionMicroseconds_ChromiumImpl

#define kFineResolutionMicroseconds \
  kFineResolutionMicroseconds_ChromiumImpl = 5; \
  static int CoarseResolutionMicroseconds(); \
  static int FineResolutionMicroseconds(); \
  int dummy

#include "src/third_party/blink/renderer/core/timing/time_clamper.h"

#undef kCoarseResolutionMicroseconds
#undef kFineResolutionMicroseconds

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_TIME_CLAMPER_H_
