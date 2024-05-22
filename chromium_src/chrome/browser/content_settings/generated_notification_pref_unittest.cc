/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/content_settings/generated_notification_pref_unittest.h"

#define BRAVE_SETTING_SOURCE_REMOTE_LIST                      \
  case SETTING_SOURCE_TPCD_GRANT:                             \
    provider_type = HostContentSettingsMap::DEFAULT_PROVIDER; \
    break;

#include "src/chrome/browser/content_settings/generated_notification_pref_unittest.cc"

#undef SETTING_SOURCE_TPCD_GRANT
