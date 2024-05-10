/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_WEBCOMPAT_EXCEPTIONS_WEBCOMPAT_CONSTANTS_H_
#define BRAVE_COMPONENTS_WEBCOMPAT_EXCEPTIONS_WEBCOMPAT_CONSTANTS_H_

#include <vector>

#include "base/containers/flat_set.h"
#include "brave/components/brave_shields/core/common/brave_shields.mojom-shared.h"

namespace webcompat_exceptions {

using WebcompatFeature = brave_shields::mojom::WebcompatFeature;

using WebcompatFeatureSet = std::vector<WebcompatFeature>;

}  // namespace webcompat_exceptions

#endif  // BRAVE_COMPONENTS_WEBCOMPAT_EXCEPTIONS_WEBCOMPAT_CONSTANTS_H_
