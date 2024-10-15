/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/back_forward_cache_restoration.h"

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"

#define pageshow_event_start_(...) \
  pageshow_event_start_(brave::RoundPerformanceIfFarbling(source, __VA_ARGS__))

#define pageshow_event_end_(...) \
  pageshow_event_end_(brave::RoundPerformanceIfFarbling(source, __VA_ARGS__))

#include "src/third_party/blink/renderer/core/timing/back_forward_cache_restoration.cc"

#undef pageshow_event_start_
#undef pageshow_event_end_
