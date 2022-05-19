/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/frame/screen.h"
#define availHeight availHeight_ChromiumImpl
#define availWidth availWidth_ChromiumImpl
#define availLeft availLeft_ChromiumImpl
#define availTop availTop_ChromiumImpl
#define isExtended isExtended_ChromiumImpl

#include "src/third_party/blink/renderer/core/frame/screen.cc"
#undef availHeight
#undef availWidth
#undef availLeft
#undef availTop
#undef isExtended

#include "src/third_party/blink/renderer/core/execution_context/execution_context.h"

namespace blink {

int Screen::height() const {
  if (!DomWindow())
    return 0;
  const int inner = DomWindow()->innerHeight();
  return brave::FarbledInteger(
    GetExecutionContext(), "window_innerHeight",
    inner, inner + 8, height_ChromiumImpl());
}

int Screen::width() const {
  if (!DomWindow())
    return 0;
  const int inner = DomWindow()->innerWidth();
  return brave::FarbledInteger(
    GetExecutionContext(), "window_innerWidth",
    inner, inner + 8, width_ChromiumImpl());
}

int Screen::availLeft() const {
  if (!DomWindow())
    return 0;
  return brave::FarbledInteger(
    GetExecutionContext(), "window_screenX",
    0, 8, availLeft_ChromiumImpl());
}

int Screen::availTop() const {
  if (!DomWindow())
    return 0;
  return brave::FarbledInteger(
    GetExecutionContext(), "window_screenY",
    0, 8, availTop_ChromiumImpl());
}

int Screen::availHeight() const {
  if (!DomWindow())
    return 0;
  const int inner = DomWindow()->innerHeight();
  return brave::FarbledInteger(
    GetExecutionContext(), "window_innerHeight",
    inner, inner + 8, availHeight_ChromiumImpl());
}

int Screen::availWidth() const {
  if (!DomWindow())
    return 0;
  const int inner = DomWindow()->innerWidth();
  return brave::FarbledInteger(
    GetExecutionContext(), "window_innerWidth",
    inner, inner + 8, availWidth_ChromiumImpl());
}

bool Screen::isExtended() const {
  const auto level = brave::GetBraveFarblingLevelFor(GetExecutionContext(), BraveFarblingLevel::OFF);
  return level == BraveFarblingLevel::OFF ? isExtended_ChromiumImpl() : false;
}

}  // namespace blink
