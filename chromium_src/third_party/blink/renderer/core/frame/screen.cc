/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/third_party/blink/renderer/core/brave_session_cache.h"
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

namespace blink {

int Screen::height() const {
  if (!DomWindow())
    return 0;
  return brave::IsFarbling(GetExecutionContext()) ?
    DomWindow()->outerHeight() : height_ChromiumImpl();
}

int Screen::width() const {
  if (!DomWindow())
    return 0;
  return brave::IsFarbling(GetExecutionContext()) ?
    DomWindow()->outerWidth() : width_ChromiumImpl();
}

int Screen::availLeft() const {
  if (!DomWindow())
    return 0;
  return brave::IsFarbling(GetExecutionContext()) ?
    DomWindow()->screenX() : availLeft_ChromiumImpl();
}

int Screen::availTop() const {
  if (!DomWindow())
    return 0;
  return brave::IsFarbling(GetExecutionContext()) ?
    DomWindow()->screenY() : availTop_ChromiumImpl();
}

int Screen::availHeight() const {
  if (!DomWindow())
    return 0;
  return brave::IsFarbling(GetExecutionContext()) ?
    DomWindow()->outerHeight() : availHeight_ChromiumImpl();
}

int Screen::availWidth() const {
  if (!DomWindow())
    return 0;
  return brave::IsFarbling(GetExecutionContext()) ?
    DomWindow()->outerWidth() : availWidth_ChromiumImpl();
}

bool Screen::isExtended() const {
  return brave::IsFarbling(GetExecutionContext()) ? false : isExtended_ChromiumImpl();
}

}  // namespace blink
