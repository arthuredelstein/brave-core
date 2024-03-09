/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "components/content_settings/core/browser/content_settings_uma_util.h"

// Leave a gap between Chromium values and our values in the kHistogramValue
// array so that we don't have to renumber when new content settings types are
// added upstream.
namespace {

// Do not change the value arbitrarily. This is used to validate we have a gap
// between Chromium's and Brave's histograms. This value must be less than 1000
// as upstream performs a sanity check that the total number of buckets isn't
// unreasonably large.
constexpr int kBraveValuesStart = 900;

constexpr int brave_value(int incr) {
  return kBraveValuesStart + incr;
}

}  // namespace

static_assert(static_cast<int>(ContentSettingsType::NUM_TYPES) <
                  kBraveValuesStart,
              "There must a gap between the histograms used by Chromium, and "
              "the ones used by Brave.");

// clang-format off
#define BRAVE_HISTOGRAM_VALUE_LIST                                             \
  {ContentSettingsType::BRAVE_ADS, brave_value(0)},                            \
  {ContentSettingsType::BRAVE_COSMETIC_FILTERING, brave_value(1)},             \
  {ContentSettingsType::BRAVE_TRACKERS, brave_value(2)},                       \
  {ContentSettingsType::BRAVE_HTTP_UPGRADABLE_RESOURCES, brave_value(3)},      \
  {ContentSettingsType::BRAVE_FINGERPRINTING_V2, brave_value(4)},              \
  {ContentSettingsType::BRAVE_SHIELDS, brave_value(5)},                        \
  {ContentSettingsType::BRAVE_REFERRERS, brave_value(6)},                      \
  {ContentSettingsType::BRAVE_COOKIES, brave_value(7)},                        \
  {ContentSettingsType::BRAVE_SPEEDREADER, brave_value(8)},                    \
  {ContentSettingsType::BRAVE_ETHEREUM, brave_value(9)},                       \
  {ContentSettingsType::BRAVE_SOLANA, brave_value(10)},                        \
  {ContentSettingsType::BRAVE_GOOGLE_SIGN_IN, brave_value(11)},                \
  {ContentSettingsType::BRAVE_HTTPS_UPGRADE, brave_value(12)},                 \
  {ContentSettingsType::BRAVE_REMEMBER_1P_STORAGE, brave_value(13)},           \
  {ContentSettingsType::BRAVE_LOCALHOST_ACCESS, brave_value(14)},              \
  {ContentSettingsType::BRAVE_WEBCOMPAT_AUDIO, brave_value(30)},               \
  {ContentSettingsType::BRAVE_WEBCOMPAT_CANVAS, brave_value(31)},              \
  {ContentSettingsType::BRAVE_WEBCOMPAT_DEVICEMEMORY, brave_value(32)},        \
  {ContentSettingsType::BRAVE_WEBCOMPAT_EVENTSOURCEPOOL, brave_value(33)},     \
  {ContentSettingsType::BRAVE_WEBCOMPAT_FONT, brave_value(34)},                \
  {ContentSettingsType::BRAVE_WEBCOMPAT_HARDWARECONCURRENCY, brave_value(35)}, \
  {ContentSettingsType::BRAVE_WEBCOMPAT_KEYBOARD, brave_value(36)},            \
  {ContentSettingsType::BRAVE_WEBCOMPAT_LANGUAGE, brave_value(37)},            \
  {ContentSettingsType::BRAVE_WEBCOMPAT_MEDIADEVICES, brave_value(38)},        \
  {ContentSettingsType::BRAVE_WEBCOMPAT_PLUGINS, brave_value(39)},             \
  {ContentSettingsType::BRAVE_WEBCOMPAT_SCREEN, brave_value(40)},              \
  {ContentSettingsType::BRAVE_WEBCOMPAT_SPEECHSYNTHESIS, brave_value(41)},     \
  {ContentSettingsType::BRAVE_WEBCOMPAT_USBDEVICESERIALNUMBER,                 \
    brave_value(42)},                                                          \
  {ContentSettingsType::BRAVE_WEBCOMPAT_USERAGENT, brave_value(43)},           \
  {ContentSettingsType::BRAVE_WEBCOMPAT_WEBGL, brave_value(44)},               \
  {ContentSettingsType::BRAVE_WEBCOMPAT_WEBGL2, brave_value(45)},              \
  {ContentSettingsType::BRAVE_WEBCOMPAT_WEBSOCKETSPOOL, brave_value(46)},
// clang-format on

#include "src/components/content_settings/core/browser/content_settings_uma_util.cc"

#undef BRAVE_HISTOGRAM_VALUE_LIST
