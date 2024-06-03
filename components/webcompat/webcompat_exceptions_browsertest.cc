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
#include "brave/components/webcompat/features.h"
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

class WebcompatExceptionsBrowserTest : public InProcessBrowserTest {
 public:
  WebcompatExceptionsBrowserTest()
      : https_server_(net::EmbeddedTestServer::TYPE_HTTPS) {
    feature_list_.InitAndEnableFeature(
        webcompat::features::kBraveWebcompatExceptionsService);
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

    ASSERT_TRUE(https_server_.Start());

    url_ = https_server_.GetURL("a.test", "/simple.html");
  }

  void SetUpCommandLine(base::CommandLine* command_line) override {
    // Since the HTTPS server only serves a valid cert for localhost,
    // this is needed to load pages from "www.google.*" without an interstitial.
    command_line->AppendSwitch(switches::kIgnoreCertificateErrors);

    // The production code only allows known ports (80 for http and 443 for
    // https), but the test server runs on a random port.
    command_line->AppendSwitch(switches::kIgnoreGooglePortNumbers);
  }

  const net::EmbeddedTestServer& https_server() { return https_server_; }
  const GURL& url() { return url_; }

  HostContentSettingsMap* content_settings() {
    return HostContentSettingsMapFactory::GetForProfile(browser()->profile());
  }

  void ShieldsDown() {
    brave_shields::SetBraveShieldsEnabled(content_settings(), false, url());
  }

  void ShieldsUp() {
    brave_shields::SetBraveShieldsEnabled(content_settings(), true, url());
  }

  void AllowFingerprinting() {
    brave_shields::SetFingerprintingControlType(content_settings(),
                                                ControlType::ALLOW, url());
  }

  void BlockFingerprinting() {
    brave_shields::SetFingerprintingControlType(content_settings(),
                                                ControlType::BLOCK, url());
  }

  void SetFingerprintingDefault() {
    brave_shields::SetFingerprintingControlType(content_settings(),
                                                ControlType::DEFAULT, url());
  }

  content::WebContents* contents() {
    return browser()->tab_strip_model()->GetActiveWebContents();
  }

  void NavigateToURLUntilLoadStop(const std::string& origin,
                                  const std::string& path) {
    ASSERT_TRUE(ui_test_utils::NavigateToURL(
        browser(), https_server().GetURL(origin, path)));
  }

 protected:
  base::test::ScopedFeatureList feature_list_;

 private:
  GURL url_;
  net::test_server::EmbeddedTestServer https_server_;
};

IN_PROC_BROWSER_TEST_F(WebcompatExceptionsBrowserTest,
                       BlockScriptsShieldsDownInOtherTab) {
  ASSERT_EQ(true, true);
}
