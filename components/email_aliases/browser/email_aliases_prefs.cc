/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */


#include "brave/components/email_aliases/browser/pref_names.h"
#include "components/prefs/pref_registry_simple.h"

namespace email_aliases {

void RegisterLocalStatePrefs(PrefRegistrySimple* registry) {
  registry->RegisterStringPref(kEmailAliasesVerificationToken, std::string());
  registry->RegisterStringPref(kEmailAliasesAuthToken, std::string());
}

void RegisterProfilePrefs(PrefRegistrySimple* registry) {
  registry->RegisterDictionaryPref(kEmailAliasesNotes);
}

}  // namespace email_aliases
