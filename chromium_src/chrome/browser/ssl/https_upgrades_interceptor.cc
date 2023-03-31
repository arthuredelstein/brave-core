/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ssl/https_upgrades_interceptor.h"

#include "brave/browser/brave_browser_process.h"
#include "brave/components/brave_shields/browser/brave_shields_util.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ssl/https_only_mode_tab_helper.h"
#include "components/prefs/pref_service.h"
#include "net/base/features.h"
#include "net/base/url_util.h"

class GURL;

namespace content {
class BrowserContext;
}  // namespace content

namespace {

bool ShouldUpgradeToHttps(content::BrowserContext* context, const GURL& url) {
  if (!brave_shields::IsHttpsByDefaultFeatureEnabled()) {
    return false;
  }
  HostContentSettingsMap* map =
      HostContentSettingsMapFactory::GetForProfile(context);
  return brave_shields::ShouldUpgradeToHttps(
      map, url, g_brave_browser_process->https_upgrade_exceptions_service());
}

}  // namespace

#define MaybeCreateLoader MaybeCreateLoader_ChromiumImpl
#define MaybeCreateLoaderForResponse MaybeCreateLoaderForResponse_ChromiumImpl
#define MaybeCreateLoaderOnHstsQueryCompleted MaybeCreateLoaderOnHstsQueryCompleted_ChromiumImpl
#define IsEnabled(FLAG)                                \
  IsEnabled(FLAG.name == features::kHttpsUpgrades.name \
                ? net::features::kBraveHttpsByDefault  \
                : FLAG)

#include "src/chrome/browser/ssl/https_upgrades_interceptor.cc"

#undef MaybeCreateLoader
#undef MaybeCreateLoaderForResponse
#undef MaybeCreateLoaderOnHstsQueryCompleted
#undef IsEnabled

void HttpsUpgradesInterceptor::MaybeCreateLoader(
    const network::ResourceRequest& tentative_resource_request,
    content::BrowserContext* browser_context,
    content::URLLoaderRequestInterceptor::LoaderCallback callback) {
  if (!ShouldUpgradeToHttps(browser_context, tentative_resource_request.url)) {
    std::move(callback).Run({});
    return;
  }
  return MaybeCreateLoader_ChromiumImpl(tentative_resource_request,
                                        browser_context, std::move(callback));
}

bool HttpsUpgradesInterceptor::MaybeCreateLoaderForResponse(
    const network::URLLoaderCompletionStatus& status,
    const network::ResourceRequest& request,
    network::mojom::URLResponseHeadPtr* response_head,
    mojo::ScopedDataPipeConsumerHandle* response_body,
    mojo::PendingRemote<network::mojom::URLLoader>* loader,
    mojo::PendingReceiver<network::mojom::URLLoaderClient>* client_receiver,
    blink::ThrottlingURLLoader* url_loader,
    bool* skip_other_interceptors,
    bool* will_return_unsafe_redirect) {
  auto* web_contents =
      content::WebContents::FromFrameTreeNodeId(frame_tree_node_id_);
  HostContentSettingsMap* map =
      HostContentSettingsMapFactory::GetForProfile(web_contents->GetBrowserContext());
  http_interstitial_enabled_ = brave_shields::ShouldForceHttps(map, request.url);
  return MaybeCreateLoaderForResponse_ChromiumImpl(status, request, response_head, response_body,
           loader, client_receiver, url_loader, skip_other_interceptors, will_return_unsafe_redirect);
}

void HttpsUpgradesInterceptor::MaybeCreateLoaderOnHstsQueryCompleted(
    const network::ResourceRequest& tentative_resource_request,
    content::URLLoaderRequestInterceptor::LoaderCallback callback,
    Profile* profile,
    content::WebContents* web_contents,
    HttpsOnlyModeTabHelper* tab_helper,
    bool is_hsts_active_for_host) {
  HostContentSettingsMap* map =
    HostContentSettingsMapFactory::GetForProfile(profile);
  http_interstitial_enabled_ = brave_shields::ShouldForceHttps(map, tentative_resource_request.url);
    MaybeCreateLoaderOnHstsQueryCompleted_ChromiumImpl(tentative_resource_request,
      std::move(callback), profile, web_contents, tab_helper, is_hsts_active_for_host);
}
