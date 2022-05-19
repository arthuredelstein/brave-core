/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/frame/screen.h"

#include "brave/third_party/blink/renderer/core/brave_session_cache.h"

#define BRAVE_GET_SCREEN_INFO                                   \
  if (brave::IsFarblingEnabled(GetExecutionContext())) {        \
    gfx::Rect farbledScreenRect(                                \
        DomWindow()->screenX(), DomWindow()->screenY(),         \
        DomWindow()->outerWidth(), DomWindow()->outerHeight()); \
    brave_screen_info_ = screen;                                \
    brave_screen_info_.available_rect = farbledScreenRect;      \
    brave_screen_info_.rect = farbledScreenRect;                \
    brave_screen_info_.is_extended = false;                     \
    brave_screen_info_.is_primary = true;                       \
    return brave_screen_info_;                                  \
  }

#include "src/third_party/blink/renderer/core/frame/screen.cc"

#undef BRAVE_GET_SCREEN_INFO
