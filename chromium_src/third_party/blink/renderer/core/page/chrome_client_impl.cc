/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "src/third_party/blink/renderer/core/page/chrome_client_impl.cc"

namespace blink {

const display::ScreenInfos& ChromeClientImpl::BraveGetScreenInfos(
    LocalFrame& frame) const {
  LocalDOMWindow* dom_window = frame.DomWindow();
  gfx::Rect farbledScreenRect(dom_window->screenX(), dom_window->screenY(),
                              dom_window->outerWidth(),
                              dom_window->outerHeight());
  display::ScreenInfo screenInfo = GetScreenInfo(frame);
  screenInfo.rect = farbledScreenRect;
  screenInfo.available_rect = farbledScreenRect;
  screenInfo.is_extended = false;
  screenInfo.is_primary = false;
  screen_infos_ = display::ScreenInfos(screenInfo);
  return screen_infos_;
}

}  // namespace blink
