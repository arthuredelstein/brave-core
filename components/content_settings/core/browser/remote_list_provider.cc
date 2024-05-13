/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/content_settings/core/browser/remote_list_provider.h"

#include "components/content_settings/core/browser/content_settings_rule.h"

namespace content_settings {

namespace {

class TinyIterator : public RuleIterator {
 public:
  TinyIterator() {}
  ~TinyIterator() override {}
  bool HasNext() const override { return !sent_; }
  std::unique_ptr<Rule> Next() override {
    sent_ = true;
    return std::make_unique<Rule>(
        ContentSettingsPattern::FromString("https://example.net/*"),
        ContentSettingsPattern::Wildcard(), base::Value(CONTENT_SETTING_ALLOW),
        RuleMetaData());
  }

 private:
  bool sent_ = false;
};

}  // namespace

RemoteListProvider::RemoteListProvider() {}

std::unique_ptr<RuleIterator> RemoteListProvider::GetRuleIterator(
    ContentSettingsType content_type,
    bool off_the_record,
    const PartitionKey& partition_key) const {
  // TODO
  if (content_type == ContentSettingsType::BRAVE_FINGERPRINTING_V2) {
    DLOG(INFO)
        << "GetRuleIterator: ContentSettingsType::BRAVE_FINGERPRINTING_V2";
    return std::make_unique<TinyIterator>();
  }
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
