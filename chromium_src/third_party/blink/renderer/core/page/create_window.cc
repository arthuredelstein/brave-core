#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CREATE_WINDOW_CC_
#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CREATE_WINDOW_CC_

#include "third_party/blink/renderer/core/frame/screen.h"
#include "third_party/blink/renderer/core/page/create_window.h"

// Because we are spoofing screenX and screenY, we need to offset the position
// when a page script opens a new window in screen coordinates. And because
// we are spoofing screen width and height, we should artificially limit the
// window size to that width and height as well, so that window.open can't
// be used to probe the screen size.
#define GetWindowFeaturesFromString                                           \
GetWindowFeaturesFromString(const String& feature_string,                     \
                            LocalDOMWindow* dom_window) {                     \
  WebWindowFeatures window_features = GetWindowFeaturesFromStringOriginal(    \
    feature_string, dom_window);                                              \
  if (window_features.x_set) {                                                \
    window_features.x += dom_window->screenXOriginal();                       \
  }                                                                           \
  if (window_features.y_set) {                                                \
    window_features.y += dom_window->screenYOriginal();                       \
  }                                                                           \
  if (window_features.width_set) {                                            \
    int maxWidth = dom_window->screen()->width();                             \
    if (window_features.width > maxWidth) {                                   \
      window_features.width = maxWidth;                                       \
    }                                                                         \
  }                                                                           \
  if (window_features.height_set) {                                           \
    int maxHeight = dom_window->screen()->height();                           \
    if (window_features.height > maxHeight) {                                 \
      window_features.height = maxHeight;                                     \
    }                                                                         \
  }                                                                           \
  return window_features;                                                     \
}                                                                             \
WebWindowFeatures GetWindowFeaturesFromStringOriginal

#include "src/third_party/blink/renderer/core/page/create_window.cc"

#undef GetWindowFeaturesFromString

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CREATE_WINDOW_CC_