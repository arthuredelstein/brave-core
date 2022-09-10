/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/modules/mediarecorder/blob_event.h"

#include "src/third_party/blink/renderer/modules/mediarecorder/blob_event.cc"

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"

namespace blink {

class ExecutionContext;

BlobEvent::BlobEvent(const AtomicString& type,
                     ExecutionContext* context,
                     Blob* blob,
                     double timecode) {
  BlobEvent(type, blob,
            brave::AllowFingerprinting(context) ? timecode : round(timecode));
}

}  // namespace blink
