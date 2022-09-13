/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/timing/performance.h"

#include "brave/third_party/blink/renderer/core/farbling/brave_session_cache.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/timing/performance_observer.h"

#define cross_origin_isolated_capability_(...)    \
  cross_origin_isolated_capability_(__VA_ARGS__), \
      allow_fingerprinting_(brave::AllowTimeStampFingerprinting(context))

#define MonotonicTimeToDOMHighResTimeStamp \
  MonotonicTimeToDOMHighResTimeStamp_ChromiumImpl

#define GetNavigationId(...) GetNavigationId(__VA_ARGS__), allow_fingerprinting_

#define now now_ChromiumImpl

#define timeOrigin timeOrigin_ChromiumImpl

#define BuildJSONValue BuildJSONValue_ChromiumImpl

#include "src/third_party/blink/renderer/core/timing/performance.cc"

#undef cross_origin_isolated_capability_
#undef MonotonicTimeToDOMHighResTimeStamp
#undef GetNavigationId
#undef now
#undef timeOrigin
#undef BuildJSONValue

namespace blink {

namespace {

DOMHighResTimeStamp MaybeRoundDOMHighResTimeStamp(
    DOMHighResTimeStamp time_stamp,
    bool allow_fingerprinting) {
  return allow_fingerprinting ? time_stamp : brave::RoundTimeStamp(time_stamp);
}

}  // namespace

// static
DOMHighResTimeStamp Performance::MonotonicTimeToDOMHighResTimeStamp(
    base::TimeTicks time_origin,
    base::TimeTicks monotonic_time,
    bool allow_negative_value,
    bool cross_origin_isolated_capability) {
  return MonotonicTimeToDOMHighResTimeStamp_ChromiumImpl(
      time_origin, monotonic_time, allow_negative_value,
      cross_origin_isolated_capability);
}

// static
DOMHighResTimeStamp Performance::MonotonicTimeToDOMHighResTimeStamp(
    base::TimeTicks time_origin,
    base::TimeTicks monotonic_time,
    bool allow_negative_value,
    bool cross_origin_isolated_capability,
    bool allow_fingerprinting) {
  return MaybeRoundDOMHighResTimeStamp(
      MonotonicTimeToDOMHighResTimeStamp_ChromiumImpl(
          time_origin, monotonic_time, allow_negative_value,
          cross_origin_isolated_capability),
      allow_fingerprinting);
}

// static
DOMHighResTimeStamp Performance::MonotonicTimeToDOMHighResTimeStamp(
    base::TimeTicks time_origin,
    base::TimeTicks monotonic_time,
    bool allow_negative_value,
    bool cross_origin_isolated_capability,
    ExecutionContext* context) {
  return Performance::MonotonicTimeToDOMHighResTimeStamp(
      time_origin, monotonic_time, allow_negative_value,
      cross_origin_isolated_capability, brave::AllowFingerprinting(context));
}

DOMHighResTimeStamp Performance::MonotonicTimeToDOMHighResTimeStamp(
    base::TimeTicks monotonic_time) const {
  return MaybeRoundDOMHighResTimeStamp(
      MonotonicTimeToDOMHighResTimeStamp_ChromiumImpl(monotonic_time),
      allow_fingerprinting_);
}

DOMHighResTimeStamp Performance::now() const {
  return MaybeRoundDOMHighResTimeStamp(now_ChromiumImpl(),
                                       allow_fingerprinting_);
}

DOMHighResTimeStamp Performance::timeOrigin() const {
  return  MaybeRoundDOMHighResTimeStamp(timeOrigin_ChromiumImpl(), allow_fingerprinting_);
}

void Performance::BuildJSONValue(V8ObjectBuilder& builder) const {
  builder.AddNumber("timeOrigin", timeOrigin());
}

}  // namespace blink
