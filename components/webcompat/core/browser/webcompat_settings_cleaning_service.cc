// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/webcompat/core/browser/webcompat_settings_cleaner.h"

#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings_partition_key.h"
#include "brave/components/webcompat/content/browser/webcompat_exceptions_service.h"
#include "components/content_settings/core/common/content_settings_utils.h"

namespace webcompat {

namespace {

WebcompatExceptionsService* singleton = nullptr;
std::vector<WeakPtr<HostContentSettingsMap> > settings_maps_;

}  // namespace

WebcompatSettingsCleaningService::WebcompatSettingsCleaningService(
    HostContentSettingsMap* settings_map)
    : settings_map_(settings_map) {
    webcompat::WebcompatExceptionsService::AddObserver(this);
}

WebcompatSettingsCleaningService::~WebcompatSettingsCleaningService() {
  // NOOP
}

void WebcompatSettingsCleaningService::RemoveRedundantWebcompatSettingsByType(
    HostContentSettingsMap* settings_map,
    ContentSettingsType settings_type) {
  auto* svc = webcompat::WebcompatExceptionsService::GetInstance();
  if (!svc) {
    return;
  }
  const auto& patterns = svc->GetPatterns(settings_type);
  for (const ContentSettingPatternSource& setting :
       settings_map_->GetSettingsForOneType(settings_type)) {
    if (setting.source == content_settings::ProviderType::kPrefProvider) {
      const auto prefSettingValue =
          content_settings::ValueToContentSetting(setting.setting_value);
      bool patternExists = std::find(patterns.begin(), patterns.end(),
                                     setting.primary_pattern) != patterns.end();
      if ((prefSettingValue == CONTENT_SETTING_BLOCK ||
           prefSettingValue == CONTENT_SETTING_ASK) &&
          !patternExists) {
        settings_map_->SetContentSettingCustomScope(
            setting.primary_pattern, ContentSettingsPattern::Wildcard(),
            settings_type, CONTENT_SETTING_DEFAULT);
      }
    }
  }
}

void WebcompatSettingsCleaningService::OnWebcompatRulesUpdated() {
  if (content_type_set.ContainsAllTypes()) {
    return;
  }
  ContentSettingsType settings_type = content_type_set.GetType();
  if ((settings_type >= ContentSettingsType::BRAVE_WEBCOMPAT_NONE &&
       settings_type < ContentSettingsType::BRAVE_WEBCOMPAT_ALL) ||
      settings_type == ContentSettingsType::BRAVE_FINGERPRINTING_V2) {
    DLOG(INFO) << "OnContentSettingChanged(): " << primary_pattern.ToString()
               << ", " << secondary_pattern.ToString() << ", " << settings_type;
    RemoveRedundantWebcompatSettingsByType(settings_type);
  }
}

// static
WebcompatSettingsCleaningService* WebcompatSettingsCleaningService::GetInstance() {
  if (singleton == nullptr) {
    singleton = new WebcompatExceptionsService();
  }
  return singleton;
}

// static
WebcompatSettingsCleaningService* WebcompatSettingsCleaningService::AddSettingsMap(HostContentSettingsMap* settings_map) {
  settings_maps_.push_back(settings_map);
}

}  // namespace webcompat
