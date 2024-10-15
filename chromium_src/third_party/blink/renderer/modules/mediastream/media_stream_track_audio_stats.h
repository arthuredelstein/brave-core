/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASTREAM_MEDIA_STREAM_TRACK_AUDIO_STATS_H_
#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASTREAM_MEDIA_STREAM_TRACK_AUDIO_STATS_H_

#include "third_party/blink/renderer/platform/audio/audio_frame_stats_accumulator.h"

#define deliveredFramesDuration(...)    \
  deliveredFramesDuration(__VA_ARGS__); \
  DOMHighResTimeStamp deliveredFramesDuration_ChromiumImpl(__VA_ARGS__)

#define totalFramesDuration(...)    \
  totalFramesDuration(__VA_ARGS__); \
  DOMHighResTimeStamp totalFramesDuration_ChromiumImpl(__VA_ARGS__)

#define latency(...)    \
  latency(__VA_ARGS__); \
  DOMHighResTimeStamp latency_ChromiumImpl(__VA_ARGS__)

#define averageLatency(...)    \
  averageLatency(__VA_ARGS__); \
  DOMHighResTimeStamp averageLatency_ChromiumImpl(__VA_ARGS__)

#define minimumLatency(...)    \
  minimumLatency(__VA_ARGS__); \
  DOMHighResTimeStamp minimumLatency_ChromiumImpl(__VA_ARGS__)

#define maximumLatency(...)    \
  maximumLatency(__VA_ARGS__); \
  DOMHighResTimeStamp maximumLatency_ChromiumImpl(__VA_ARGS__)

#include "src/third_party/blink/renderer/modules/mediastream/media_stream_track_audio_stats.h"  // IWYU pragma: export

#undef deliveredFramesDuration
#undef totalFramesDuration
#undef latency
#undef averageLatency
#undef minimumLatency
#undef maximumLatency

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASTREAM_MEDIA_STREAM_TRACK_AUDIO_STATS_H_
