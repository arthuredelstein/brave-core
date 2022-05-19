/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_SCREEN_H_
#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_SCREEN_H_

#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "ui/display/screen_info.h"
#include "ui/display/screen_infos.h"

#define height            \
  height() const;         \
  int heightOriginal

#define width             \
  width() const;          \
  int widthOriginal

#define availHeight       \
  availHeight() const;    \
  int availHeightOriginal

#define availWidth        \
  availWidth() const;     \
  int availWidthOriginal

#define availLeft         \
  availLeft() const;      \
  int availLeftOriginal

#define availTop          \
  availTop() const;       \
  int availTopOriginal

#define isExtended        \
  isExtended() const;     \
  bool isExtendedOriginal

#include "src/third_party/blink/renderer/core/frame/screen.h"

#undef height
#undef width
#undef availHeight
#undef availWidth
#undef availLeft
#undef availTop
#undef isExtended

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_SCREEN_H_
