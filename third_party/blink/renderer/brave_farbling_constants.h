/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_THIRD_PARTY_BLINK_RENDERER_BRAVE_FARBLING_CONSTANTS_H_
#define BRAVE_THIRD_PARTY_BLINK_RENDERER_BRAVE_FARBLING_CONSTANTS_H_

#include "third_party/blink/public/platform/web_common.h"

enum BraveFarblingLevel { BALANCED = 0, OFF, MAXIMUM };

enum class BraveFarblingType {
  kNone,
  kCanvas,
  kDeviceMemory,
  kEventSourcePool,
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

#endif  // BRAVE_THIRD_PARTY_BLINK_RENDERER_BRAVE_FARBLING_CONSTANTS_H_
