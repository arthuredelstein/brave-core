/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_INBOX_ALIASES_BROWSER_INBOX_ALIASES_PREFS_H_
#define BRAVE_COMPONENTS_INBOX_ALIASES_BROWSER_INBOX_ALIASES_PREFS_H_

class PrefService;
class PrefRegistrySimple;

namespace inbox_aliases {

void RegisterProfilePrefs(PrefRegistrySimple* registry);

}  // namespace inbox_aliases

#endif  // BRAVE_COMPONENTS_INBOX_ALIASES_BROWSER_INBOX_ALIASES_PREFS_H_
