/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_ads/search_result_ad/search_result_ad_handler.h"

#include <string>
#include <utility>
#include <vector>

#include "base/containers/contains.h"
#include "base/containers/fixed_flat_set.h"
#include "base/containers/flat_set.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/render_frame_host.h"
#include "services/service_manager/public/cpp/interface_provider.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

using ads::mojom::Conversion;
using ads::mojom::ConversionPtr;
using ads::mojom::SearchResultAd;
using ads::mojom::SearchResultAdPtr;

namespace brave_ads {

namespace {

constexpr char kSearchAdLinkDataType[] = "Product";
constexpr char kContextPropertyName[] = "@context";
constexpr char kTypePropertyName[] = "@type";

constexpr char kDataPlacementId[] = "data-placement-id";
constexpr char kDataCreativeInstanceId[] = "data-creative-instance-id";
constexpr char kDataCreativeSetId[] = "data-creative-set-id";
constexpr char kDataCampaignId[] = "data-campaign-id";
constexpr char kDataAdvertiserId[] = "data-advertiser-id";
constexpr char kDataLandingPage[] = "data-landing-page";
constexpr char kDataHeadlineText[] = "data-headline-text";
constexpr char kDataDescription[] = "data-description";
constexpr char kDataRewardsValue[] = "data-rewards-value";
constexpr char kDataConversionTypeValue[] = "data-conversion-type-value";
constexpr char kDataConversionUrlPatternValue[] =
    "data-conversion-url-pattern-value";
constexpr char kDataConversionAdvertiserPublicKeyValue[] =
    "data-conversion-advertiser-public-key-value";
constexpr char kDataConversionObservationWindowValue[] =
    "data-conversion-observation-window-value";

constexpr auto kVettedBraveSearchHosts =
    base::MakeFixedFlatSet<base::StringPiece>(
        {"search.brave.com", "search-dev.brave.com",
         "search-dev-local.brave.com", "search.brave.software",
         "search.bravesoftware.com"});

constexpr auto kSearchResultAdAttributes =
    base::MakeFixedFlatSet<base::StringPiece>(
        {kDataPlacementId, kDataCreativeInstanceId, kDataCreativeSetId,
         kDataCampaignId, kDataAdvertiserId, kDataLandingPage,
         kDataHeadlineText, kDataDescription, kDataRewardsValue,
         kDataConversionTypeValue, kDataConversionUrlPatternValue,
         kDataConversionAdvertiserPublicKeyValue,
         kDataConversionObservationWindowValue});

bool GetStringValue(const schema_org::mojom::PropertyPtr& ad_property,
                    std::string* out_value) {
  DCHECK(ad_property);
  DCHECK(out_value);

  // Wrong attribute type
  if (!ad_property->values->is_string_values() ||
      ad_property->values->get_string_values().size() != 1) {
    return false;
  }

  *out_value = ad_property->values->get_string_values().front();

  return true;
}

bool GetInt64Value(const schema_org::mojom::PropertyPtr& ad_property,
                   int64_t* out_value) {
  DCHECK(ad_property);
  DCHECK(out_value);

  // Wrong attribute type
  if (!ad_property->values->is_long_values() ||
      ad_property->values->get_long_values().size() != 1) {
    return false;
  }

  *out_value = ad_property->values->get_long_values().front();

  return true;
}

bool GetDoubleValue(const schema_org::mojom::PropertyPtr& ad_property,
                    double* out_value) {
  DCHECK(ad_property);
  DCHECK(out_value);

  // Wrong attribute type
  if (!ad_property->values->is_string_values() ||
      ad_property->values->get_string_values().size() != 1) {
    return false;
  }

  std::string value = ad_property->values->get_string_values().front();
  return base::StringToDouble(value, out_value);
}

bool SetSearchAdProperty(const schema_org::mojom::PropertyPtr& ad_property,
                         SearchResultAd* search_result_ad) {
  DCHECK(ad_property);
  DCHECK(search_result_ad);
  DCHECK(search_result_ad->conversion);

  const std::string& name = ad_property->name;
  if (name == kDataPlacementId) {
    return GetStringValue(ad_property, &search_result_ad->placement_id);
  } else if (name == kDataCreativeInstanceId) {
    return GetStringValue(ad_property, &search_result_ad->creative_instance_id);
  } else if (name == kDataCreativeSetId) {
    return GetStringValue(ad_property, &search_result_ad->creative_set_id);
  } else if (name == kDataCampaignId) {
    return GetStringValue(ad_property, &search_result_ad->campaign_id);
  } else if (name == kDataAdvertiserId) {
    return GetStringValue(ad_property, &search_result_ad->advertiser_id);
  } else if (name == kDataLandingPage) {
    return GetStringValue(ad_property, &search_result_ad->target_url);
  } else if (name == kDataHeadlineText) {
    return GetStringValue(ad_property, &search_result_ad->headline_text);
  } else if (name == kDataDescription) {
    return GetStringValue(ad_property, &search_result_ad->description);
  } else if (name == kDataRewardsValue) {
    return GetDoubleValue(ad_property, &search_result_ad->value);
  } else if (name == kDataConversionTypeValue) {
    return GetStringValue(ad_property, &search_result_ad->conversion->type);
  } else if (name == kDataConversionUrlPatternValue) {
    return GetStringValue(ad_property,
                          &search_result_ad->conversion->url_pattern);
  } else if (name == kDataConversionAdvertiserPublicKeyValue) {
    return GetStringValue(ad_property,
                          &search_result_ad->conversion->advertiser_public_key);
  } else if (name == kDataConversionObservationWindowValue) {
    return GetInt64Value(ad_property,
                         &search_result_ad->conversion->observation_window);
  }

  NOTREACHED();

  return false;
}

absl::optional<SearchResultAdsList> ParseEntityProperties(
    const schema_org::mojom::EntityPtr& entity) {
  SearchResultAdsList search_result_ads_list;

  for (const auto& property : entity->properties) {
    if (property->name == kContextPropertyName ||
        property->name == kTypePropertyName) {
      continue;
    }

    // Search ad product could have only "@context" and "creatives" properties.
    if (property->name != "creatives") {
      return absl::nullopt;
    }

    if (!property->values->is_entity_values() ||
        property->values->get_entity_values().empty()) {
      LOG(ERROR) << "Search ad attributes list is empty";
      return SearchResultAdsList();
    }

    for (const auto& ad_entity : property->values->get_entity_values()) {
      if (property->name == kTypePropertyName) {
        continue;
      }

      SearchResultAdPtr search_result_ad = SearchResultAd::New();
      search_result_ad->conversion = Conversion::New();

      base::flat_set<base::StringPiece> found_attributes;
      for (const auto& ad_property : ad_entity->properties) {
        // Wrong attribute name
        const auto* it = std::find(kSearchResultAdAttributes.begin(),
                                   kSearchResultAdAttributes.end(),
                                   base::StringPiece(ad_property->name));

        if (it == kSearchResultAdAttributes.end()) {
          LOG(ERROR) << "Wrong search ad attribute specified: "
                     << ad_property->name;
          return SearchResultAdsList();
        }
        found_attributes.insert(*it);

        if (!SetSearchAdProperty(ad_property, search_result_ad.get())) {
          LOG(ERROR) << "Cannot read search ad attribute value: "
                     << ad_property->name;
          return SearchResultAdsList();
        }
      }

      // Not all of attributes were specified.
      if (found_attributes.size() != kSearchResultAdAttributes.size()) {
        std::vector<base::StringPiece> absent_attributes;
        std::set_difference(kSearchResultAdAttributes.begin(),
                            kSearchResultAdAttributes.end(),
                            found_attributes.begin(), found_attributes.end(),
                            std::back_inserter(absent_attributes));

        LOG(ERROR) << "Some of search ad attributes were not specified: "
                   << base::JoinString(absent_attributes, ", ");

        return SearchResultAdsList();
      }

      search_result_ads_list.push_back(std::move(search_result_ad));
    }

    // Creatives has been parsed.
    break;
  }

  return search_result_ads_list;
}

SearchResultAdsList ParseMetadataEntities(blink::mojom::WebPagePtr web_page) {
  for (const auto& entity : web_page->entities) {
    if (entity->type != kSearchAdLinkDataType) {
      continue;
    }

    absl::optional<SearchResultAdsList> search_result_ads_list =
        ParseEntityProperties(entity);

    if (search_result_ads_list) {
      return std::move(*search_result_ads_list);
    }
  }

  return SearchResultAdsList();
}

}  // namespace

SearchResultAdHandler::SearchResultAdHandler() = default;

SearchResultAdHandler::~SearchResultAdHandler() = default;

bool SearchResultAdHandler::IsAllowedBraveSearchHost(const GURL& url) const {
  if (!url.is_valid() || !url.SchemeIs(url::kHttpsScheme)) {
    return false;
  }
  const std::string host = url.host();
  return base::Contains(kVettedBraveSearchHosts, host);
}

void SearchResultAdHandler::RetrieveSearchResultAd(
    content::RenderFrameHost* render_frame_host,
    RetrieveSearchResultAdCallback callback) {
  DCHECK(render_frame_host);
  DCHECK(IsAllowedBraveSearchHost(render_frame_host->GetLastCommittedURL()));
  DCHECK(!document_metadata_.is_bound());

  render_frame_host->GetRemoteInterfaces()->GetInterface(
      document_metadata_.BindNewPipeAndPassReceiver());
  DCHECK(document_metadata_.is_bound());

  document_metadata_->GetEntities(
      base::BindOnce(&SearchResultAdHandler::OnRetrieveSearchResultAdEntities,
                     weak_factory_.GetWeakPtr(), std::move(callback)));
}

void SearchResultAdHandler::OnRetrieveSearchResultAdEntities(
    RetrieveSearchResultAdCallback callback,
    blink::mojom::WebPagePtr web_page) {
  document_metadata_.reset();

  if (!web_page) {
    std::move(callback).Run(SearchResultAdsList());
    return;
  }

  SearchResultAdsList search_result_ads =
      ParseMetadataEntities(std::move(web_page));
  std::move(callback).Run(std::move(search_result_ads));
}

}  // namespace brave_ads
