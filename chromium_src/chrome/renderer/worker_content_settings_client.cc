/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <optional>

#include "brave/components/brave_shields/core/common/brave_shield_utils.h"
#include "chrome/renderer/worker_content_settings_client.h"
#include "components/content_settings/renderer/content_settings_agent_impl.h"
#include "content/public/renderer/render_frame.h"
#include "net/base/features.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"
#include "third_party/blink/public/web/web_local_frame.h"

BraveFarblingLevel WorkerContentSettingsClient::GetBraveFarblingLevel(
    webcompat_exceptions::WebcompatFeature farblingType) {
  ContentSetting setting = CONTENT_SETTING_DEFAULT;
  if (content_setting_rules_) {
    const GURL& primary_url = top_frame_origin_.GetURL();
    for (const auto& rule : content_setting_rules_->brave_shields_rules) {
      if (rule.primary_pattern.Matches(primary_url)) {
        setting = rule.GetContentSetting();
        break;
      }
    }
    if (setting == CONTENT_SETTING_BLOCK) {
      // Brave Shields is down
      setting = CONTENT_SETTING_ALLOW;
    } else {
      // Brave Shields is up, so check fingerprinting rules
      setting = brave_shields::GetBraveFPContentSettingFromRules(
          content_setting_rules_->fingerprinting_rules, primary_url);
    }
  }
  if (std::find(webcompat_features_.begin(), webcompat_features_.end(),
                farblingType) != webcompat_features_.end()) {
    return BraveFarblingLevel::OFF;
  }
  if (setting == CONTENT_SETTING_BLOCK) {
    return BraveFarblingLevel::MAXIMUM;
  } else if (setting == CONTENT_SETTING_ALLOW) {
    return BraveFarblingLevel::OFF;
  } else {
    return BraveFarblingLevel::BALANCED;
  }
}

blink::WebSecurityOrigin
WorkerContentSettingsClient::GetEphemeralStorageOriginSync() {
  if (!base::FeatureList::IsEnabled(net::features::kBraveEphemeralStorage))
    return {};

  if (is_unique_origin_)
    return {};

  // If first party ephemeral storage is enabled, we should always ask the
  // browser if a worker should use ephemeral storage or not.
  if (!base::FeatureList::IsEnabled(
          net::features::kBraveFirstPartyEphemeralStorage) &&
      net::registry_controlled_domains::SameDomainOrHost(
          top_frame_origin_, document_origin_,
          net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES)) {
    return {};
  }

  EnsureContentSettingsManager();

  std::optional<url::Origin> optional_ephemeral_storage_origin;
  content_settings_manager_->AllowEphemeralStorageAccess(
      frame_token_, document_origin_, site_for_cookies_, top_frame_origin_,
      &optional_ephemeral_storage_origin);
  // Don't cache the value intentionally as other WorkerContentSettingsClient
  // methods do.
  return blink::WebSecurityOrigin(
      optional_ephemeral_storage_origin
          ? blink::WebSecurityOrigin(*optional_ephemeral_storage_origin)
          : blink::WebSecurityOrigin());
}

bool WorkerContentSettingsClient::HasContentSettingsRules() const {
  return content_setting_rules_.get();
}

GURL GetOriginOrURL(const blink::WebFrame* frame) {
  url::Origin top_origin = url::Origin(frame->Top()->GetSecurityOrigin());
  // The |top_origin| is unique ("null") e.g., for file:// URLs. Use the
  // document URL as the primary URL in those cases.
  // TODO(alexmos): This is broken for --site-per-process, since top() can be a
  // WebRemoteFrame which does not have a document(), and the WebRemoteFrame's
  // URL is not replicated.  See https://crbug.com/628759.
  if (top_origin.opaque() && frame->Top()->IsWebLocalFrame()) {
    return frame->Top()->ToWebLocalFrame()->GetDocument().Url();
  }
  return top_origin.GetURL();
}

void GetWebcompatFeatures(content::RenderFrame* render_frame,
                          std::vector<brave_shields::mojom::WebcompatFeature>*
                              webcompat_features_out) {
  blink::WebLocalFrame* frame = render_frame->GetWebFrame();
  mojo::AssociatedRemote<brave_shields::mojom::BraveShieldsHost>
      brave_shields_remote;
  render_frame->GetRemoteAssociatedInterfaces()->GetInterface(
      &brave_shields_remote);
  brave_shields_remote->GetWebcompatExceptions(GetOriginOrURL(frame),
                                               webcompat_features_out);
}

#define TopFrameOrigin() \
  TopFrameOrigin();      \
  GetWebcompatFeatures(render_frame, &webcompat_features_)

#include "src/chrome/renderer/worker_content_settings_client.cc"

#undef TopFrameOrigin
