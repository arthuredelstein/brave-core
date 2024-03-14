/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_WEBCOMPAT_EXCEPTIONS_BROWSER_WEBCOMPAT_CONSTANTS_H_
#define BRAVE_COMPONENTS_WEBCOMPAT_EXCEPTIONS_BROWSER_WEBCOMPAT_CONSTANTS_H_

namespace webcompat_exceptions {

enum class BraveFarblingType {
  kNone,
  kAudio,
  kCanvas,
  kDeviceMemory,
  kEventSourcePool,
  kFont,
  kHardwareConcurrency,
  kKeyboard,
  kLanguage,
  kMediaDevices,
  kPlugins,
  kScreen,
  kSpeechSynthesis,
  kUsbDeviceSerialNumber,
  kUserAgent,
  kWebGL,
  kWebGL2,
  kWebSocketsPool,
};

}  // namespace webcompat_exceptions

#endif  // BRAVE_COMPONENTS_WEBCOMPAT_EXCEPTIONS_BROWSER_WEBCOMPAT_CONSTANTS_H_
