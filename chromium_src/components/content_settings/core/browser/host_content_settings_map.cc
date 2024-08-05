/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/content_settings/core/browser/host_content_settings_map.h"

#include "base/containers/contains.h"
#include "brave/components/content_settings/core/browser/remote_list_provider.h"
#include "build/build_config.h"
#include "components/content_settings/core/browser/content_settings_utils.h"
#include "components/content_settings/core/common/content_settings_types.h"
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

}  // namespace
}  // namespace content_settings

#define IsMorePermissive(a, b) IsMorePermissive_BraveImpl(content_type, a, b)

#include "src/components/content_settings/core/browser/host_content_settings_map.cc"

#undef IsMorePermissive

void HostContentSettingsMap::RemoveRedundantWebcompatSettingsRules() {
  const auto& remote_list_provider =
      content_settings_providers_[ProviderType::kRemoteListProvider];
  for (auto settings_type = ContentSettingsType::BRAVE_WEBCOMPAT_NONE;
       settings_type != ContentSettingsType::BRAVE_WEBCOMPAT_ALL;
       settings_type = static_cast<ContentSettingsType>(
           static_cast<int32_t>(settings_type) + 1)) {
    std::unique_ptr<content_settings::RuleIterator> rule_iterator =
        pref_provider_->GetRuleIterator(
            settings_type, false,
            content_settings::PartitionKey::WipGetDefault());
    if (rule_iterator) {
      while (rule_iterator->HasNext()) {
        std::unique_ptr<content_settings::Rule> prefRule =
            rule_iterator->Next();
        std::unique_ptr<content_settings::Rule> remoteRule =
            remote_list_provider->GetRule(
                prefRule->primary_pattern.ToRepresentativeUrl(),
                prefRule->secondary_pattern.ToRepresentativeUrl(),
                settings_type, false,
                content_settings::PartitionKey::WipGetDefault());
        if (prefRule && remoteRule &&
            content_settings::ValueToContentSetting(prefRule->value) ==
                content_settings::ValueToContentSetting(remoteRule->value)) {
          pref_provider_->SetWebsiteSetting(
              prefRule->primary_pattern, prefRule->secondary_pattern,
              settings_type, base::Value(), {},
              content_settings::PartitionKey::WipGetDefault());
        }
      }
    }
  }
}

#if !BUILDFLAG(IS_IOS)
#undef PrefProvider
#undef PolicyProvider
#endif
