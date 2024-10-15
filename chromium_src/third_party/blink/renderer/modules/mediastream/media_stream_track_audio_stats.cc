/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/modules/mediastream/media_stream_track_audio_stats.h"

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_object_builder.h"
#include "third_party/blink/renderer/platform/audio/audio_frame_stats_accumulator.h"

#define deliveredFramesDuration deliveredFramesDuration_ChromiumImpl
#define totalFramesDuration totalFramesDuration_ChromiumImpl
#define latency latency_ChromiumImpl
#define averageLatency averageLatency_ChromiumImpl
#define minimumLatency minimumLatency_ChromiumImpl
#define maximumLatency maximumLatency_ChromiumImpl

#define AddNumber(A, B)                           \
  AddNumber(A, brave::RoundPerformanceIfFarbling( \
                   ExecutionContext::From(script_state), B))

#include "src/third_party/blink/renderer/modules/mediastream/media_stream_track_audio_stats.cc"

#undef deliveredFramesDuration
#undef totalFramesDuration
#undef latency
#undef averageLatency
#undef minimumLatency
#undef maximumLatency

#undef AddNumber

namespace blink {

#define DEFINE_ROUNDED_FUNC(FUNC_NAME)                       \
  DOMHighResTimeStamp MediaStreamTrackAudioStats::FUNC_NAME( \
      ScriptState* script_state) {                           \
    return brave::RoundPerformanceIfFarbling(                \
        ExecutionContext::From(script_state),                \
        FUNC_NAME##_ChromiumImpl(script_state));             \
  }

DEFINE_ROUNDED_FUNC(deliveredFramesDuration)
DEFINE_ROUNDED_FUNC(totalFramesDuration)
DEFINE_ROUNDED_FUNC(latency)
DEFINE_ROUNDED_FUNC(averageLatency)
DEFINE_ROUNDED_FUNC(minimumLatency)
DEFINE_ROUNDED_FUNC(maximumLatency)

#undef DEFINE_ROUNDED_FUNC

}  // namespace blink
