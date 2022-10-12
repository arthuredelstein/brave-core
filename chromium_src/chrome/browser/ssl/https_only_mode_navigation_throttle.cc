/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ssl/https_only_mode_navigation_throttle.h"

#define WillFailRequest WillFailRequest_ChromiumImpl

#include "src/chrome/browser/ssl/https_only_mode_navigation_throttle.cc"

#undef WillFailRequest

#include "base/threading/sequenced_task_runner_handle.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ssl/https_only_mode_controller_client.h"
#include "components/security_interstitials/content/stateful_ssl_host_state_delegate.h"
#include "content/public/browser/page_navigator.h"

// Called if there is a non-OK net::Error in the completion status.
content::NavigationThrottle::ThrottleCheckResult
HttpsOnlyModeNavigationThrottle::WillFailRequest() {
  auto* handle = navigation_handle();

  const net::SSLInfo info = handle->GetSSLInfo().value_or(net::SSLInfo());
  int cert_status = info.cert_status;
  if (!net::IsCertStatusError(cert_status) &&
      handle->GetNetErrorCode() == net::OK) {
    // Don't fall back.
    return content::NavigationThrottle::PROCEED;
  }

  // Don't fall back if the Interceptor didn't upgrade this navigation.
  auto* contents = handle->GetWebContents();
  auto* tab_helper = HttpsOnlyModeTabHelper::FromWebContents(contents);
  if (!tab_helper->is_navigation_upgraded()) {
    // Don't fall back.
    return content::NavigationThrottle::PROCEED;
  }

  // We are going to fall back.
  auto request_url = handle->GetURL();
  Profile* profile = Profile::FromBrowserContext(contents->GetBrowserContext());
  StatefulSSLHostStateDelegate* state =
      static_cast<StatefulSSLHostStateDelegate*>(
          profile->GetSSLHostStateDelegate());
  // StatefulSSLHostStateDelegate can be null during tests.
  if (state) {
    state->AllowHttpForHost(
        request_url.host(),
        contents->GetPrimaryMainFrame()->GetStoragePartition());
  }
  content::OpenURLParams url_params =
      content::OpenURLParams::FromNavigationHandle(handle);
  // Delete the redirect chain that tried to upgrade but hit a dead end.
  url_params.redirect_chain.clear();
  // Use the original fallback URL.
  url_params.url = tab_helper->fallback_url();
  tab_helper->set_is_navigation_upgraded(false);
  tab_helper->set_is_navigation_fallback(true);
  // Launch a new task to open the fallback URL.
  base::SequencedTaskRunnerHandle::Get()->PostTask(
      FROM_HERE, base::BindOnce(
                     [](base::WeakPtr<content::WebContents> contents,
                        const content::OpenURLParams& url_params) {
                       if (!contents) {
                         return;
                       }
                       // This deletes the NavigationThrottle and
                       // NavigationHandle.
                       contents->Stop();
                       // Navigate to the fallback URL.
                       contents->OpenURL(url_params);
                     },
                     contents->GetWeakPtr(), std::move(url_params)));
  // The NavigationThrottle object will be deleted; nothing more to do here.
  return content::NavigationThrottle::CANCEL_AND_IGNORE;
}
