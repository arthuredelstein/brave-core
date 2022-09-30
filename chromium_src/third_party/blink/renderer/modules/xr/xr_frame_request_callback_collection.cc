/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/modules/xr/xr_frame_request_callback_collection.h"
#include "third_party/blink/renderer/bindings/modules/v8/v8_xr_frame_request_callback.h"

#include "third_party/blink/renderer/core/timing/time_clamper.h"

#define InvokeAndReportException(ARG0, ARG_HIGHRESTIME, ARG1) \
  InvokeAndReportException(                                   \
      ARG0, TimeClamper::MaybeRoundMilliseconds(ARG_HIGHRESTIME), ARG1)

#include "src/third_party/blink/renderer/modules/xr/xr_frame_request_callback_collection.cc"

#undef InvokeAndReportException
