/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/dom/frame_request_callback_collection.h"

#include "third_party/blink/renderer/core/timing/time_clamper.h"

#define InvokeAndReportException(ARG0, ARG_HIGHRESTIME) \
  InvokeAndReportException(ARG0, TimeClamper::MaybeRoundMilliseconds(ARG_HIGHRESTIME))

#include "src/third_party/blink/renderer/core/dom/frame_request_callback_collection.cc"

#undef InvokeAndReportException
