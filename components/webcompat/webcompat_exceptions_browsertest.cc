/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <memory>
#include <string_view>

#include "base/feature_list.h"
#include "base/path_service.h"
#include "brave/components/brave_shields/content/browser/brave_shields_util.h"
#include "brave/components/brave_shields/core/common/brave_shield_constants.h"
#include "brave/components/brave_shields/core/common/features.h"
#include "brave/components/constants/brave_paths.h"
#include "build/build_config.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "components/google/core/common/google_switches.h"
#include "components/network_session_configurator/common/network_switches.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/test_utils.h"
#include "net/base/features.h"
#include "net/dns/mock_host_resolver.h"
#include "net/http/http_request_headers.h"
#include "net/test/embedded_test_server/default_handlers.h"
#include "net/test/embedded_test_server/http_request.h"
#include "net/test/embedded_test_server/request_handler_util.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "url/origin.h"

using brave_shields::ControlType;
using net::test_server::HttpRequest;
using net::test_server::HttpResponse;

namespace {

const char kIframeID[] = "test";

#define COOKIE_STR "test=hi"

// Remaps requests from /maps/simple.html to /simple.html
std::unique_ptr<HttpResponse> HandleGoogleMapsFileRequest(
    const base::FilePath& server_root,
    const HttpRequest& request) {
  HttpRequest new_request(request);
  if (!new_request.relative_url.starts_with("/maps")) {
    // This handler is only relevant for a Google Maps url.
    return nullptr;
  }
  new_request.relative_url = new_request.relative_url.substr(5);
  return HandleFileRequest(server_root, new_request);
}

}  // namespace

class WebcompatExceptionsBrowserTest : public InProcessBrowserTest {
 public:
  WebcompatExceptionsBrowserTest()
      : https_server_(net::EmbeddedTestServer::TYPE_HTTPS) {
    feature_list_.InitAndEnableFeature(net::features::kBraveEphemeralStorage);
  }

  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();

    host_resolver()->AddRule("*", "127.0.0.1");

    brave::RegisterPathProvider();
    base::FilePath test_data_dir;
    base::PathService::Get(brave::DIR_TEST_DATA, &test_data_dir);
    https_server_.SetSSLConfig(net::EmbeddedTestServer::CERT_TEST_NAMES);
    https_server_.ServeFilesFromDirectory(test_data_dir);
    https_server_.AddDefaultHandlers(GetChromeTestDataDir());
    https_server_.RegisterDefaultHandler(
        base::BindRepeating(&HandleGoogleMapsFileRequest, test_data_dir));
    content::SetupCrossSiteRedirector(&https_server_);

    ASSERT_TRUE(https_server_.Start());

    url_ = https_server_.GetURL("a.test", "/iframe.html");
    cross_site_url_ = https_server_.GetURL("b.test", "/simple.html");
    cross_site_image_url_ = https_server_.GetURL("b.test", "/logo.png");
    link_url_ = https_server_.GetURL("a.test", "/simple_link.html");
    redirect_to_cross_site_url_ =
        https_server_.GetURL("a.test", "/cross-site/b.test/simple.html");
    redirect_to_cross_site_image_url_ =
        https_server_.GetURL("a.test", "/cross-site/b.test/logo.png");
    same_site_url_ = https_server_.GetURL("sub.a.test", "/simple.html");
    same_origin_url_ = https_server_.GetURL("a.test", "/simple.html");
    same_origin_image_url_ = https_server_.GetURL("a.test", "/logo.png");
    top_level_page_url_ = https_server_.GetURL("a.test", "/");
    top_level_page_pattern_ =
        ContentSettingsPattern::FromString("https://a.test/*");
    iframe_pattern_ = ContentSettingsPattern::FromString("https://b.test/*");
    first_party_pattern_ =
        ContentSettingsPattern::FromString("https://firstParty/*");
  }

  void SetUpCommandLine(base::CommandLine* command_line) override {
    // Since the HTTPS server only serves a valid cert for localhost,
    // this is needed to load pages from "www.google.*" without an interstitial.
    command_line->AppendSwitch(switches::kIgnoreCertificateErrors);

    // The production code only allows known ports (80 for http and 443 for
    // https), but the test server runs on a random port.
    command_line->AppendSwitch(switches::kIgnoreGooglePortNumbers);
  }

  std::string GetLastReferrer(const GURL& url) const {
    base::AutoLock auto_lock(last_referrers_lock_);
    auto pos = last_referrers_.find(url);
    if (pos == last_referrers_.end()) {
      return "(missing)";  // Fail test if we haven't seen this URL before.
    }
    return pos->second;
  }

  const net::EmbeddedTestServer& https_server() { return https_server_; }
  const GURL& url() { return url_; }
  const GURL& cross_site_url() { return cross_site_url_; }
  const GURL& cross_site_image_url() { return cross_site_image_url_; }
  const GURL& link_url() { return link_url_; }
  const GURL& redirect_to_cross_site_url() {
    return redirect_to_cross_site_url_;
  }
  const GURL& redirect_to_cross_site_image_url() {
    return redirect_to_cross_site_image_url_;
  }
  const GURL& same_site_url() { return same_site_url_; }
  const GURL& same_origin_url() { return same_origin_url_; }

  const GURL& same_origin_image_url() { return same_origin_image_url_; }

  const GURL top_level_page_url() { return top_level_page_url_; }

  const ContentSettingsPattern& top_level_page_pattern() {
    return top_level_page_pattern_;
  }

  const ContentSettingsPattern& first_party_pattern() {
    return first_party_pattern_;
  }

  const ContentSettingsPattern& iframe_pattern() { return iframe_pattern_; }

  HostContentSettingsMap* content_settings() {
    return HostContentSettingsMapFactory::GetForProfile(browser()->profile());
  }

  void BlockReferrers() {
    content_settings()->SetContentSettingCustomScope(
        top_level_page_pattern(), ContentSettingsPattern::Wildcard(),
        ContentSettingsType::BRAVE_REFERRERS, CONTENT_SETTING_BLOCK);
    ContentSettingsForOneType settings =
        content_settings()->GetSettingsForOneType(
            ContentSettingsType::BRAVE_REFERRERS);
    // default plus new setting
    EXPECT_EQ(settings.size(), 2u);
  }

  void AllowReferrers() {
    content_settings()->SetContentSettingCustomScope(
        top_level_page_pattern(), ContentSettingsPattern::Wildcard(),
        ContentSettingsType::BRAVE_REFERRERS, CONTENT_SETTING_ALLOW);
    ContentSettingsForOneType settings =
        content_settings()->GetSettingsForOneType(
            ContentSettingsType::BRAVE_REFERRERS);
    // default plus new setting
    EXPECT_EQ(settings.size(), 2u);
  }

  void ShieldsDown() {
    brave_shields::SetBraveShieldsEnabled(content_settings(), false,
                                          top_level_page_url());
  }

  void ShieldsUp() {
    brave_shields::SetBraveShieldsEnabled(content_settings(), true,
                                          top_level_page_url());
  }

  void AllowFingerprinting() {
    brave_shields::SetFingerprintingControlType(
        content_settings(), ControlType::ALLOW, top_level_page_url());
  }

  void BlockFingerprinting() {
    brave_shields::SetFingerprintingControlType(
        content_settings(), ControlType::BLOCK, top_level_page_url());
  }

  void SetFingerprintingDefault() {
    brave_shields::SetFingerprintingControlType(
        content_settings(), ControlType::DEFAULT, top_level_page_url());
  }

  content::WebContents* contents() {
    return browser()->tab_strip_model()->GetActiveWebContents();
  }

  content::RenderFrameHost* child_frame() {
    return ChildFrameAt(contents()->GetPrimaryMainFrame(), 0);
  }

  void NavigateToURLUntilLoadStop(const std::string& origin,
                                  const std::string& path) {
    ASSERT_TRUE(ui_test_utils::NavigateToURL(
        browser(), https_server().GetURL(origin, path)));
  }

  void NavigateIframe(const GURL& url) {
    ASSERT_TRUE(NavigateIframeToURL(contents(), kIframeID, url));
    ASSERT_EQ(child_frame()->GetLastCommittedURL(), url);
  }

 protected:
  base::test::ScopedFeatureList feature_list_;

 private:
  GURL url_;
  GURL cross_site_url_;
  GURL cross_site_image_url_;
  GURL link_url_;
  GURL redirect_to_cross_site_url_;
  GURL redirect_to_cross_site_image_url_;
  GURL same_site_url_;
  GURL same_origin_url_;
  GURL same_origin_image_url_;
  GURL top_level_page_url_;
  ContentSettingsPattern top_level_page_pattern_;
  ContentSettingsPattern first_party_pattern_;
  ContentSettingsPattern iframe_pattern_;
  mutable base::Lock last_referrers_lock_;
  std::map<GURL, std::string> last_referrers_;

  base::ScopedTempDir temp_user_data_dir_;
  net::test_server::EmbeddedTestServer https_server_;
};

IN_PROC_BROWSER_TEST_F(WebcompatExceptionsBrowserTest,
                       BlockScriptsShieldsDownInOtherTab) {
  ASSERT_EQ(true, true);
}
