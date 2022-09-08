/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/modules/webaudio/audio_context.h"

#define getOutputTimestamp getOutputTimestamp_ChromiumImpl

#include "src/third_party/blink/renderer/modules/webaudio/audio_context.cc"

#undef getOutputTimestamp

namespace blink {

AudioTimestamp* AudioContext::getOutputTimestamp(
    ScriptState* script_state) const {
  AudioTimestamp* audioTimestamp = getOutputTimestamp_ChromiumImpl(script_state);
  ExecutionContext* context = GetExecutionContext();
  const bool allow_fingerprinting = brave::AllowFingerprinting(context);
  if (!allow_fingerprinting) {
    audioTimestamp->setContextTime(round(audioTimestamp->getContextTimeOr(0.0)));
  }
  return audioTimestamp;
}

}  // namespace blink
