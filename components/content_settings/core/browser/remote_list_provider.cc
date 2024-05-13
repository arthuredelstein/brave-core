/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/content_settings/core/browser/remote_list_provider.h"

namespace content_settings {

RemoteListProvider::RemoteListProvider() {}

std::unique_ptr<RuleIterator> RemoteListProvider::GetRuleIterator(
    ContentSettingsType content_type,
    bool off_the_record,
    const PartitionKey& partition_key) const {
  // TODO
  return nullptr;
}

std::unique_ptr<Rule> RemoteListProvider::GetRule(
    const GURL& primary_url,
    const GURL& secondary_url,
    ContentSettingsType content_type,
    bool off_the_record,
    const PartitionKey& partition_key) const {
  // TODO
  return nullptr;
}

bool RemoteListProvider::SetWebsiteSetting(
    const ContentSettingsPattern& primary_pattern,
    const ContentSettingsPattern& secondary_pattern,
    ContentSettingsType content_type,
    base::Value&& value,
    const ContentSettingConstraints& constraints,
    const PartitionKey& partition_key) {
  // RemoteListProvider is read-only.
  return false;
}

void RemoteListProvider::ClearAllContentSettingsRules(
    ContentSettingsType content_type,
    const PartitionKey& partition_key) {
  // RemoteListProvider is read-only.
}

void RemoteListProvider::ShutdownOnUIThread() {}

}  // namespace content_settings
