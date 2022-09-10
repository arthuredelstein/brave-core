/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/modules/xr/xr_frame_request_callback_collection.h"

#define ExecuteCallbacks ExecuteCallbacks_ChromiumImpl

#include "src/third_party/blink/renderer/modules/xr/xr_frame_request_callback_collection.cc"

#undef ExecuteCallbacks

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"

namespace blink {

void XRFrameRequestCallbackCollection::ExecuteCallbacks(XRSession* session,
                                                        double timestamp,
                                                        XRFrame* frame) {
  bool allow_fingerprinting = brave::AllowFingerprinting(context_);
  ExecuteCallbacks_ChromiumImpl(
      session, allow_fingerprinting ? timestamp : round(timestamp), frame);
}

}  // namespace blink
