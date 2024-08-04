/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/content_settings/core/browser/host_content_settings_map.h"

#include "base/containers/contains.h"
#include "brave/components/content_settings/core/browser/remote_list_provider.h"
#include "build/build_config.h"
#include "components/content_settings/core/browser/content_settings_provider.h"
#include "components/content_settings/core/browser/content_settings_utils.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "components/content_settings/core/common/content_settings_utils.h"
#include "components/content_settings/core/common/features.h"

#if !BUILDFLAG(IS_IOS)
#include "brave/components/content_settings/core/browser/brave_content_settings_pref_provider.h"
#define PrefProvider BravePrefProvider
#endif

namespace content_settings {
namespace {

bool IsMorePermissive_BraveImpl(ContentSettingsType content_type,
                                ContentSetting setting,
                                ContentSetting initial_setting) {
  // These types have additional logic for OffTheRecord profiles to always
  // return BLOCK (with a random timeout) instead of inheriting the setting.
  //
  // We must be careful to not break this, otherwise
  // ProcessIncognitoInheritanceBehavior() will return `initial_setting` which
  // is usually ASK and incorrect for OffTheRecord profiles.
  const ContentSettingsType kOffTheRecordAwareTypes[] = {
      ContentSettingsType::NOTIFICATIONS,
      ContentSettingsType::PROTECTED_MEDIA_IDENTIFIER,
      ContentSettingsType::IDLE_DETECTION,
      ContentSettingsType::BRAVE_HTTPS_UPGRADE,
  };

  const bool is_more_permissive = IsMorePermissive(setting, initial_setting);
  if (is_more_permissive ||
      base::Contains(kOffTheRecordAwareTypes, content_type) ||
      base::FeatureList::IsEnabled(kAllowIncognitoPermissionInheritance)) {
    return is_more_permissive;
  }

  // If the type doesn't have special OffTheRecord handling, force
  // ProcessIncognitoInheritanceBehavior() to always return `initial_setting`.
  return true;
}

void RemoveRedundantWebcompatSettingsRulesForSetting(
    ProviderInterface* remote_list_provider,
    ProviderInterface* pref_provider,
    ContentSettingsType settings_type) {
  std::unique_ptr<RuleIterator> rule_iterator =
      remote_list_provider->GetRuleIterator(settings_type, false,
                                            PartitionKey::WipGetDefault());
  if (rule_iterator) {
    while (rule_iterator->HasNext()) {
      std::unique_ptr<Rule> remoteRule = rule_iterator->Next();
      if (remoteRule) {
        std::unique_ptr<Rule> prefRule = pref_provider->GetRule(
            remoteRule->primary_pattern.ToRepresentativeUrl(),
            remoteRule->secondary_pattern.ToRepresentativeUrl(), settings_type,
            false, PartitionKey::WipGetDefault());
        if (prefRule &&
            content_settings::ValueToContentSetting(prefRule->value) ==
                content_settings::ValueToContentSetting(remoteRule->value)) {
          // Pref rule matches the remote rule. Delete the redundant pref rule.
          pref_provider->SetWebsiteSetting(
              prefRule->primary_pattern, prefRule->secondary_pattern,
              settings_type, base::Value(), {}, PartitionKey::WipGetDefault());
        }
      }
    }
  }
}

}  // namespace
}  // namespace content_settings

#define IsMorePermissive(a, b) IsMorePermissive_BraveImpl(content_type, a, b)

#include "src/components/content_settings/core/browser/host_content_settings_map.cc"

#undef IsMorePermissive

// Removes all webcompat settings set by user in Prefs that are the same as
// those provided by the remote webcompat exceptions list.
void HostContentSettingsMap::RemoveRedundantWebcompatSettingsRules() {
  content_settings::ProviderInterface* remote_list_provider =
      content_settings_providers_[ProviderType::kRemoteListProvider].get();
  for (auto settings_type = ContentSettingsType::BRAVE_WEBCOMPAT_NONE;
       settings_type != ContentSettingsType::BRAVE_WEBCOMPAT_ALL;
       settings_type = static_cast<ContentSettingsType>(
           static_cast<int32_t>(settings_type) + 1)) {
    content_settings::RemoveRedundantWebcompatSettingsRulesForSetting(
        remote_list_provider, pref_provider_, settings_type);
  }
  content_settings::RemoveRedundantWebcompatSettingsRulesForSetting(
      remote_list_provider, pref_provider_,
      ContentSettingsType::BRAVE_FINGERPRINTING_V2);
}

void HostContentSettingsMap::OnWebcompatRulesUpdated() {
  RemoveRedundantWebcompatSettingsRules();
}

#if !BUILDFLAG(IS_IOS)
#undef PrefProvider
#undef PolicyProvider
#endif
