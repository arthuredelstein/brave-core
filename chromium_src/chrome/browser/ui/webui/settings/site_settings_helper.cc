/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/webui/settings/site_settings_helper.h"

#include <string_view>
#include <vector>

#include "base/containers/cxx20_erase_vector.h"
#include "brave/components/brave_shields/core/common/brave_shield_constants.h"
#include "third_party/blink/public/common/features.h"

#define HasRegisteredGroupName HasRegisteredGroupName_ChromiumImpl
#define ContentSettingsTypeToGroupName \
  ContentSettingsTypeToGroupName_ChromiumImpl
#define GetVisiblePermissionCategories \
  GetVisiblePermissionCategories_ChromiumImpl

// clang-format off
#define BRAVE_CONTENT_SETTINGS_TYPE_GROUP_NAMES_LIST                     \
  {ContentSettingsType::BRAVE_ADS, nullptr},                             \
  {ContentSettingsType::BRAVE_COSMETIC_FILTERING, nullptr},              \
  {ContentSettingsType::BRAVE_TRACKERS, nullptr},                        \
  {ContentSettingsType::BRAVE_HTTP_UPGRADABLE_RESOURCES, nullptr},       \
  {ContentSettingsType::BRAVE_FINGERPRINTING_V2, nullptr},               \
  {ContentSettingsType::BRAVE_SHIELDS, nullptr},                         \
  {ContentSettingsType::BRAVE_REFERRERS, nullptr},                       \
  {ContentSettingsType::BRAVE_COOKIES, nullptr},                         \
  {ContentSettingsType::BRAVE_SPEEDREADER, nullptr},                     \
  {ContentSettingsType::BRAVE_ETHEREUM, nullptr},                        \
  {ContentSettingsType::BRAVE_SOLANA, nullptr},                          \
  {ContentSettingsType::BRAVE_GOOGLE_SIGN_IN, nullptr},                  \
  {ContentSettingsType::BRAVE_HTTPS_UPGRADE, nullptr},                   \
  {ContentSettingsType::BRAVE_REMEMBER_1P_STORAGE, nullptr},             \
  {ContentSettingsType::BRAVE_LOCALHOST_ACCESS, nullptr},                \
                                                                         \
  {ContentSettingsType::BRAVE_WEBCOMPAT_AUDIO, nullptr},                 \
  {ContentSettingsType::BRAVE_WEBCOMPAT_CANVAS, nullptr},                \
  {ContentSettingsType::BRAVE_WEBCOMPAT_DEVICEMEMORY, nullptr},          \
  {ContentSettingsType::BRAVE_WEBCOMPAT_EVENTSOURCEPOOL, nullptr},       \
  {ContentSettingsType::BRAVE_WEBCOMPAT_FONT, nullptr},                  \
  {ContentSettingsType::BRAVE_WEBCOMPAT_HARDWARECONCURRENCY, nullptr},   \
  {ContentSettingsType::BRAVE_WEBCOMPAT_KEYBOARD, nullptr},              \
  {ContentSettingsType::BRAVE_WEBCOMPAT_LANGUAGE, nullptr},              \
  {ContentSettingsType::BRAVE_WEBCOMPAT_MEDIADEVICES, nullptr},          \
  {ContentSettingsType::BRAVE_WEBCOMPAT_PLUGINS, nullptr},               \
  {ContentSettingsType::BRAVE_WEBCOMPAT_SCREEN, nullptr},                \
  {ContentSettingsType::BRAVE_WEBCOMPAT_SPEECHSYNTHESIS, nullptr},       \
  {ContentSettingsType::BRAVE_WEBCOMPAT_USBDEVICESERIALNUMBER, nullptr}, \
  {ContentSettingsType::BRAVE_WEBCOMPAT_USERAGENT, nullptr},             \
  {ContentSettingsType::BRAVE_WEBCOMPAT_WEBGL, nullptr},                 \
  {ContentSettingsType::BRAVE_WEBCOMPAT_WEBGL2, nullptr},                \
  {ContentSettingsType::BRAVE_WEBCOMPAT_WEBSOCKETSPOOL, nullptr},

// clang-format on

#define BRAVE_SITE_SETTINGS_HELPER_CONTENT_SETTINGS_TYPE_FROM_GROUP_NAME \
  if (name == "autoplay")                                                \
    return ContentSettingsType::AUTOPLAY;                                \
  if (name == "googleSignIn")                                            \
    return ContentSettingsType::BRAVE_GOOGLE_SIGN_IN;                    \
  if (name == "localhostAccess")                                         \
    return ContentSettingsType::BRAVE_LOCALHOST_ACCESS;                  \
  if (name == "ethereum")                                                \
    return ContentSettingsType::BRAVE_ETHEREUM;                          \
  if (name == "solana")                                                  \
    return ContentSettingsType::BRAVE_SOLANA;                            \
  if (name == brave_shields::kBraveShields)                              \
    return ContentSettingsType::BRAVE_SHIELDS;

#include "src/chrome/browser/ui/webui/settings/site_settings_helper.cc"

#undef BRAVE_CONTENT_SETTINGS_TYPE_GROUP_NAMES_LIST
#undef BRAVE_SITE_SETTINGS_HELPER_CONTENT_SETTINGS_TYPE_FROM_GROUP_NAME
#undef GetVisiblePermissionCategories
#undef ContentSettingsTypeToGroupName
#undef HasRegisteredGroupName

namespace site_settings {

bool HasRegisteredGroupName(ContentSettingsType type) {
  if (type == ContentSettingsType::AUTOPLAY)
    return true;
  if (type == ContentSettingsType::BRAVE_GOOGLE_SIGN_IN)
    return true;
  if (type == ContentSettingsType::BRAVE_LOCALHOST_ACCESS) {
    return true;
  }
  if (type == ContentSettingsType::BRAVE_ETHEREUM)
    return true;
  if (type == ContentSettingsType::BRAVE_SOLANA)
    return true;
  if (type == ContentSettingsType::BRAVE_SHIELDS)
    return true;
  return HasRegisteredGroupName_ChromiumImpl(type);
}

std::string_view ContentSettingsTypeToGroupName(ContentSettingsType type) {
  if (type == ContentSettingsType::AUTOPLAY)
    return "autoplay";
  if (type == ContentSettingsType::BRAVE_GOOGLE_SIGN_IN)
    return "googleSignIn";
  if (type == ContentSettingsType::BRAVE_LOCALHOST_ACCESS) {
    return "localhostAccess";
  }
  if (type == ContentSettingsType::BRAVE_ETHEREUM)
    return "ethereum";
  if (type == ContentSettingsType::BRAVE_SOLANA)
    return "solana";
  if (type == ContentSettingsType::BRAVE_SHIELDS)
    return brave_shields::kBraveShields;
  return ContentSettingsTypeToGroupName_ChromiumImpl(type);
}

const std::vector<ContentSettingsType>& GetVisiblePermissionCategories() {
  static base::NoDestructor<std::vector<ContentSettingsType>> types{
      {ContentSettingsType::AUTOPLAY, ContentSettingsType::BRAVE_ETHEREUM,
       ContentSettingsType::BRAVE_SOLANA,
       ContentSettingsType::BRAVE_GOOGLE_SIGN_IN,
       ContentSettingsType::BRAVE_LOCALHOST_ACCESS}};
  static bool initialized = false;
  if (!initialized) {
    auto& base_types = GetVisiblePermissionCategories_ChromiumImpl();
    types->insert(types->end(), base_types.begin(), base_types.end());

    if (!base::FeatureList::IsEnabled(blink::features::kBraveWebSerialAPI)) {
      std::erase(*types, ContentSettingsType::SERIAL_GUARD);
    }

    initialized = true;
  }

  return *types;
}

}  // namespace site_settings
