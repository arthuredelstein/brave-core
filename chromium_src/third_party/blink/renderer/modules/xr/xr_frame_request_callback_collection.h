/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_FRAME_REQUEST_CALLBACK_COLLECTION_H_

#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_FRAME_REQUEST_CALLBACK_COLLECTION_H_

#define ExecuteCallbacks                                    \
  ExecuteCallbacks(XRSession*, double timestamp, XRFrame*); \
  void ExecuteCallbacks_ChromiumImpl

#include "src/third_party/blink/renderer/modules/xr/xr_frame_request_callback_collection.h"

#undef ExecuteCallbacks

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_FRAME_REQUEST_CALLBACK_COLLECTION_H_
