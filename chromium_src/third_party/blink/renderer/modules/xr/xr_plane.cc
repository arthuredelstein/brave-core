/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"

#include "third_party/blink/renderer/modules/xr/xr_plane.h"

#define lastChangedTime lastChangedTime_ChromiumImpl

#include "src/third_party/blink/renderer/modules/xr/xr_plane.cc"

#undef lastChangedTime

namespace blink {

double XRPlane::lastChangedTime() const {
  const double rawLastChangedTime = lastChangedTime_ChromiumImpl();
  const bool allow_fingerprinting =
      brave::AllowFingerprinting(session_->GetExecutionContext());
  return allow_fingerprinting ? rawLastChangedTime : round(rawLastChangedTime);
}

}  // namespace blink
