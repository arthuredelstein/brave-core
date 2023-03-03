// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/components/https_upgrade_exceptions/common/features.h"

#include "base/feature_list.h"

namespace https_upgrade_exceptions {
namespace features {

// When enabled, Brave will attempt to upgrade all websites
// to HTTPS and fall back to HTTP when HTTPS is not available.
BASE_FEATURE(kBraveHttpsByDefault,
             "HttpsByDefault",
             base::FEATURE_DISABLED_BY_DEFAULT);

}  // namespace features
}  // namespace https_upgrade_exceptions
