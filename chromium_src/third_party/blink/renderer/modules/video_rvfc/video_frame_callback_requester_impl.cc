/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/modules/video_rvfc/video_frame_callback_requester_impl.h"

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"

namespace {

blink::DOMHighResTimeStamp MaybeRound(blink::DOMHighResTimeStamp raw_value,
                                      bool allow_fingerprinting) {
  return allow_fingerprinting ? raw_value : brave::RoundTimeStamp(raw_value);
}

}  // namespace

#define setPresentationTime(X) \
  setPresentationTime(MaybeRound(X, allow_fingerprinting_))

#define setExpectedDisplayTime(X) \
  setExpectedDisplayTime(MaybeRound(X, allow_fingerprinting_))

#define callback_collection_(X)                                            \
  allow_fingerprinting_(                                                   \
      brave::AllowTimeStampFingerprinting(element.GetExecutionContext())), \
      callback_collection_(X)

#include "src/third_party/blink/renderer/modules/video_rvfc/video_frame_callback_requester_impl.cc"

#undef setPresentationTime
#undef setExpectedDisplayTime
#undef callback_collection_
