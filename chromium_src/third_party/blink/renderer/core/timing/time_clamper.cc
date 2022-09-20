/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/time_clamper.h"

#define kFineResolutionMicroseconds FineResolutionMicroseconds()
#define kCoarseResolutionMicroseconds CoarseResolutionMicroseconds()

#include "src/third_party/blink/renderer/core/timing/time_clamper.cc"

#undef FineResolutionMicroseconds
#undef CoarseResolutionMicroseconds

namespace blink {

// static
int TimeClamper::FineResolutionMicroseconds() {
  return 1000;
}

// static
int TimeClamper::CoarseResolutionMicroseconds() {
  return 1000;
}

}  // namespace blink
