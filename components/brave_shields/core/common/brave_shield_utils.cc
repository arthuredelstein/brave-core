// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/brave_shields/core/common/brave_shield_utils.h"

#include <iostream>
#include <set>
#include <string>

#include "base/containers/fixed_flat_map.h"
#include "base/no_destructor.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_pattern.h"
#include "url/gurl.h"

namespace brave_shields {

namespace {
using webcompat_exceptions::BraveFarblingType;
using enum ContentSettingsType;

static constexpr auto kFarblingTypeToContentSettings =
    base::MakeFixedFlatMap<webcompat_exceptions::BraveFarblingType,
                           ContentSettingsType>({
        {BraveFarblingType::kAudio, BRAVE_WEBCOMPAT_AUDIO},
        {BraveFarblingType::kCanvas, BRAVE_WEBCOMPAT_CANVAS},
        {BraveFarblingType::kDeviceMemory, BRAVE_WEBCOMPAT_DEVICE_MEMORY},
        {BraveFarblingType::kEventSourcePool, BRAVE_WEBCOMPAT_EVENT_SOURCE_POOL},
        {BraveFarblingType::kFont, BRAVE_WEBCOMPAT_FONT},
        {BraveFarblingType::kHardwareConcurrency, BRAVE_WEBCOMPAT_HARDWARE_CONCURRENCY},
        {BraveFarblingType::kKeyboard, BRAVE_WEBCOMPAT_KEYBOARD},
        {BraveFarblingType::kLanguage, BRAVE_WEBCOMPAT_LANGUAGE},
        {BraveFarblingType::kMediaDevices, BRAVE_WEBCOMPAT_MEDIA_DEVICES},
        {BraveFarblingType::kPlugins, BRAVE_WEBCOMPAT_PLUGINS},
        {BraveFarblingType::kScreen, BRAVE_WEBCOMPAT_SCREEN},
        {BraveFarblingType::kSpeechSynthesis, BRAVE_WEBCOMPAT_SPEECH_SYNTHESIS},
        {BraveFarblingType::kUsbDeviceSerialNumber, BRAVE_WEBCOMPAT_USB_DEVICE_SERIALNUMBER},
        {BraveFarblingType::kUserAgent, BRAVE_WEBCOMPAT_USERAGENT},
        {BraveFarblingType::kWebGL, BRAVE_WEBCOMPAT_WEBGL},
        {BraveFarblingType::kWebGL2, BRAVE_WEBCOMPAT_WEBGL2},
        {BraveFarblingType::kWebSocketsPool, BRAVE_WEBCOMPAT_WEBSOCKETS_POOL},
    });
}  // namespace

ContentSettingsType GetContentSettingsTypeForBraveFarblingType(
    webcompat_exceptions::BraveFarblingType farbling_type) {
  return kFarblingTypeToContentSettings.at(farbling_type);
}

ContentSetting GetBraveFPContentSettingFromRules(
    const ContentSettingsForOneType& fp_rules,
    const GURL& primary_url) {
  static const base::NoDestructor<ContentSettingsPattern> kBalancedRule(
      ContentSettingsPattern::FromString("https://balanced"));
  ContentSettingPatternSource fp_rule;
  for (const auto& rule : fp_rules) {
    if (rule.secondary_pattern == *kBalancedRule) {
      continue;
    }
    if (rule.primary_pattern.Matches(primary_url)) {
      return rule.GetContentSetting();
    }
  }

  return CONTENT_SETTING_DEFAULT;
}

ShieldsSettingCounts GetFPSettingCountFromRules(
    const ContentSettingsForOneType& fp_rules) {
  ShieldsSettingCounts result = {};

  for (const auto& rule : fp_rules) {
    if (rule.primary_pattern.MatchesAllHosts()) {
      continue;
    }
    if (rule.GetContentSetting() == CONTENT_SETTING_ALLOW) {
      result.allow++;
    } else if (rule.GetContentSetting() == CONTENT_SETTING_BLOCK) {
      result.aggressive++;
    } else {
      result.standard++;
    }
  }

  return result;
}

ShieldsSettingCounts GetAdsSettingCountFromRules(
    const ContentSettingsForOneType& ads_rules) {
  ShieldsSettingCounts result = {};

  std::set<std::string> block_set;
  // Look at primary rules
  for (const auto& rule : ads_rules) {
    if (rule.primary_pattern.MatchesAllHosts() ||
        !rule.secondary_pattern.MatchesAllHosts()) {
      continue;
    }
    if (rule.GetContentSetting() == CONTENT_SETTING_ALLOW) {
      result.allow++;
    } else {
      block_set.insert(rule.primary_pattern.ToString());
    }
  }

  // And then look at "first party" rules
  for (const auto& rule : ads_rules) {
    if (rule.primary_pattern.MatchesAllHosts() ||
        rule.secondary_pattern.MatchesAllHosts() ||
        block_set.find(rule.primary_pattern.ToString()) == block_set.end()) {
      continue;
    }
    if (rule.GetContentSetting() == CONTENT_SETTING_BLOCK) {
      result.aggressive++;
    } else {
      result.standard++;
    }
  }

  return result;
}

ContentSetting GetWebcompatSettingFromRules(
    const std::map<ContentSettingsType, ContentSettingsForOneType>&
        webcompat_rules,
    webcompat_exceptions::BraveFarblingType farbling_type,
    const GURL& primary_url) {
  if (farbling_type == webcompat_exceptions::BraveFarblingType::kNone) {
    return CONTENT_SETTING_DEFAULT;
  }
  const auto settings =
      GetContentSettingsTypeForBraveFarblingType(farbling_type);
  const auto item = webcompat_rules.find(settings);
  if (item == webcompat_rules.end()) {
    return CONTENT_SETTING_DEFAULT;
  }
  for (const auto& rule : item->second) {
    if (rule.primary_pattern.Matches(primary_url)) {
      return rule.GetContentSetting();
    }
  }
  return CONTENT_SETTING_DEFAULT;
}

}  // namespace brave_shields
