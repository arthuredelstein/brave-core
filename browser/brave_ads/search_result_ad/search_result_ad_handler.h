/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_BRAVE_ADS_SEARCH_RESULT_AD_SEARCH_RESULT_AD_HANDLER_H_
#define BRAVE_BROWSER_BRAVE_ADS_SEARCH_RESULT_AD_SEARCH_RESULT_AD_HANDLER_H_

#include "base/memory/weak_ptr.h"
#include "brave/browser/brave_ads/search_result_ad/search_result_ad_handler_aliases.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "third_party/blink/public/mojom/document_metadata/document_metadata.mojom.h"

namespace content {
class RenderFrameHost;
}

namespace brave_ads {

class SearchResultAdHandler final {
 public:
  SearchResultAdHandler();
  virtual ~SearchResultAdHandler();

  SearchResultAdHandler(const SearchResultAdHandler&) = delete;
  SearchResultAdHandler& operator=(const SearchResultAdHandler&) = delete;

  bool IsAllowedBraveSearchHost(const GURL& url) const;

  void RetrieveSearchResultAd(content::RenderFrameHost* render_frame_host,
                              RetrieveSearchResultAdCallback callback);

 private:
  void OnRetrieveSearchResultAdEntities(RetrieveSearchResultAdCallback callback,
                                        blink::mojom::WebPagePtr web_page);

  mojo::Remote<blink::mojom::DocumentMetadata> document_metadata_;

  base::WeakPtrFactory<SearchResultAdHandler> weak_factory_{this};
};

}  // namespace brave_ads

#endif  // BRAVE_BROWSER_BRAVE_ADS_SEARCH_RESULT_AD_SEARCH_RESULT_AD_HANDLER_H_
