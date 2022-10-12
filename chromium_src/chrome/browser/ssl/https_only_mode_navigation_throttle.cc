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

  // If there was no certificate error, SSLInfo will be empty.
  const net::SSLInfo info = handle->GetSSLInfo().value_or(net::SSLInfo());
  int cert_status = info.cert_status;
  if (!net::IsCertStatusError(cert_status) &&
      handle->GetNetErrorCode() == net::OK) {
    // Don't fallback.
    return content::NavigationThrottle::PROCEED;
  }

  // Only fall back if the Interceptor attempted to upgrade the navigation.
  auto* contents = handle->GetWebContents();
  auto* tab_helper = HttpsOnlyModeTabHelper::FromWebContents(contents);
  if (!tab_helper->is_navigation_upgraded()) {
    return content::NavigationThrottle::PROCEED;
  }

  auto request_url = handle->GetURL();
  std::unique_ptr<HttpsOnlyModeControllerClient> client =
      std::make_unique<HttpsOnlyModeControllerClient>(contents, request_url);
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
  tab_helper->set_is_navigation_upgraded(false);
  tab_helper->set_is_navigation_fallback(true);
  content::OpenURLParams params =
      content::OpenURLParams::FromNavigationHandle(navigation_handle());
  bool stop_navigation = true;
  base::SequencedTaskRunnerHandle::Get()->PostTask(
      FROM_HERE,
      base::BindOnce(
          [](base::WeakPtr<content::WebContents> web_contents,
             const content::OpenURLParams& url_params, bool stop_navigation) {
            if (!web_contents)
              return;
            if (stop_navigation) {
              // This deletes the NavigationThrottle and NavigationHandle.
              web_contents->Stop();
            }
            web_contents->OpenURL(url_params);
          },
          contents->GetWeakPtr(), std::move(params), stop_navigation));
  // Shouldn't end up here.
  return content::NavigationThrottle::PROCEED;
}
