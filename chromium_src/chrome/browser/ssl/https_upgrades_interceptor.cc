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
#define kHttpsUpgrades kBraveHttpsByDefault
#define is_navigation_fallback() will_fallback_to(resource_request.url)

#include "src/chrome/browser/ssl/https_upgrades_interceptor.cc"

#undef MaybeCreateLoader
#undef kHttpsUpgrades
#undef is_navigation_fallback

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
