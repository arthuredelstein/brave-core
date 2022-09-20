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

#include "base/feature_list.h"
#include "third_party/blink/public/common/features.h"

namespace blink {

// static
bool TimeClamper::ShouldRound() {
  return base::FeatureList::IsEnabled(blink::features::kBraveRoundTimeStamps);
}

// static
double TimeClamper::MaybeRoundMilliseconds(double value) {
  return ShouldRound() ? round(value) : value;
}

// static
int TimeClamper::FineResolutionMicroseconds() {
  return TimeClamper::ShouldRound() ? 1000 : kFineResolutionMicroseconds_ChromiumImpl;
}

// static
int TimeClamper::CoarseResolutionMicroseconds() {
  return TimeClamper::ShouldRound() ? 1000 : kCoarseResolutionMicroseconds_ChromiumImpl;
}

}  // namespace blink
