/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_BRAVE_ADS_SEARCH_RESULT_AD_SEARCH_RESULT_AD_HANDLER_ALIASES_H_
#define BRAVE_BROWSER_BRAVE_ADS_SEARCH_RESULT_AD_SEARCH_RESULT_AD_HANDLER_ALIASES_H_

#include <vector>

#include "base/callback.h"
#include "brave/vendor/bat-native-ads/include/bat/ads/public/interfaces/ads.mojom.h"

namespace content {
class RenderFrameHost;
}

namespace brave_ads {

using SearchResultAdsList = std::vector<ads::mojom::SearchResultAdPtr>;

using RetrieveSearchResultAdCallback =
    base::OnceCallback<void(SearchResultAdsList)>;

}  // namespace brave_ads

#endif  // BRAVE_BROWSER_BRAVE_ADS_SEARCH_RESULT_AD_SEARCH_RESULT_AD_HANDLER_ALIASES_H_
