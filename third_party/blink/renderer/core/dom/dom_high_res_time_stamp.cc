/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/dom/dom_high_res_time_stamp.h"

#include "base/feature_list.h"
#include "third_party/blink/public/common/features.h"

namespace blink {

double DOMHighResTimeStamp::GetValue() const {
  bool shouldRound = base::FeatureList::IsEnabled(blink::features::kBraveRoundTimeStamps);
  return shouldRound ? round(value_) : value_;
}

}  // namespace blink
