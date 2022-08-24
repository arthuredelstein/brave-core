/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIARECORDER_BLOB_EVENT_H_

#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIARECORDER_BLOB_EVENT_H_

namespace blink {

class ExecutionContext;

}  // namespace blink

#define timecode()                                                           \
  dummy() const { return 0.0; }                                              \
  BlobEvent(const AtomicString& type, ExecutionContext* context, Blob* blob, \
            double timecode);                                                \
  DOMHighResTimeStamp timecode()

#include "src/third_party/blink/renderer/modules/mediarecorder/blob_event.h"

#undef timecode

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIARECORDER_BLOB_EVENT_H_
