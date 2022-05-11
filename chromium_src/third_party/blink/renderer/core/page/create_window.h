#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CREATE_WINDOW_H_
#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CREATE_WINDOW_H_

#define GetWindowFeaturesFromString                                  \
GetWindowFeaturesFromString_ChromiumImpl(                            \
  const String& feature_string, LocalDOMWindow* dom_window);         \
CORE_EXPORT WebWindowFeatures GetWindowFeaturesFromString

#include "src/third_party/blink/renderer/core/page/create_window.h"

#undef GetWindowFeaturesFromString

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CREATE_WINDOW_H_