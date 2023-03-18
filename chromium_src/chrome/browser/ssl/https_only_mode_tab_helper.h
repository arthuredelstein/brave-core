/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CHROME_BROWSER_SSL_HTTPS_ONLY_MODE_TAB_HELPER_H_
#define BRAVE_CHROMIUM_SRC_CHROME_BROWSER_SSL_HTTPS_ONLY_MODE_TAB_HELPER_H_

#define is_navigation_fallback                                     \
  will_fallback_to(const GURL& next_url) const {                   \
    return is_navigation_fallback() && next_url == fallback_url(); \
  }                                                                \
  bool is_navigation_fallback

#include "src/chrome/browser/ssl/https_only_mode_tab_helper.h"

#undef is_navigation_fallback

#endif  // BRAVE_CHROMIUM_SRC_CHROME_BROWSER_SSL_HTTPS_ONLY_MODE_TAB_HELPER_H_
