/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_DOM_HIGH_RES_TIME_STAMP_FEATURE_H_
#define BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_DOM_HIGH_RES_TIME_STAMP_FEATURE_H_

namespace brave {

class DOMHighResTimeStampFeature {
public:
  static bool IsEnabled();
};

}  // namespace brave

#endif  // BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_DOM_HIGH_RES_TIME_STAMP_FEATURE_H_
