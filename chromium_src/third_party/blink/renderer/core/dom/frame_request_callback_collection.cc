/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/dom/frame_request_callback_collection.h"

#define ExecuteFrameCallbacks ExecuteFrameCallbacks_ChromiumImpl

#include "src/third_party/blink/renderer/core/dom/frame_request_callback_collection.cc"

#undef ExecuteFrameCallbacks

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"

namespace blink {

void FrameRequestCallbackCollection::ExecuteFrameCallbacks(
    double high_res_now_ms,
    double high_res_now_ms_legacy) {
  if (!brave::AllowFingerprinting(context_)) {
    high_res_now_ms = round(high_res_now_ms);
    high_res_now_ms_legacy = round(high_res_now_ms_legacy);
  }
  ExecuteFrameCallbacks_ChromiumImpl(high_res_now_ms, high_res_now_ms_legacy);
}

}  // namespace blink
