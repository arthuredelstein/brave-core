/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/path_service.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/post_task.h"
#include "base/test/thread_test_helper.h"
#include "brave/browser/brave_content_browser_client.h"
#include "brave/browser/extensions/brave_base_local_data_files_browsertest.h"
#include "brave/common/brave_paths.h"
#include "brave/common/pref_names.h"
#include "brave/components/brave_component_updater/browser/local_data_files_service.h"
#include "brave/components/brave_shields/browser/brave_shields_util.h"
#include "chrome/browser/chrome_content_browser_client.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/extensions/extension_browsertest.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/common/chrome_content_client.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/embedder_support/user_agent_utils.h"
#include "components/permissions/permission_request.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/content_mock_cert_verifier.h"
#include "net/dns/mock_host_resolver.h"
#include "net/test/embedded_test_server/embedded_test_server.h"
#include "net/test/embedded_test_server/http_request.h"

using brave_shields::ControlType;

class BraveScreenFarblingBrowserTest : public InProcessBrowserTest {
 public:
  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();

    content_client_.reset(new ChromeContentClient);
    content::SetContentClient(content_client_.get());
    browser_content_client_.reset(new BraveContentBrowserClient());
    content::SetBrowserClientForTesting(browser_content_client_.get());

    mock_cert_verifier_.mock_cert_verifier()->set_default_result(net::OK);
    host_resolver()->AddRule("*", "127.0.0.1");
    https_server_.reset(new net::EmbeddedTestServer(
        net::test_server::EmbeddedTestServer::TYPE_HTTPS));
    content::SetupCrossSiteRedirector(https_server_.get());

    brave::RegisterPathProvider();
    base::FilePath test_data_dir;
    base::PathService::Get(brave::DIR_TEST_DATA, &test_data_dir);
    https_server_->ServeFilesFromDirectory(test_data_dir);
    // https_server_->RegisterRequestMonitor(base::BindRepeating(
    //    &BraveNavigatorUserAgentFarblingBrowserTest::MonitorHTTPRequest,
    //    base::Unretained(this)));

    ASSERT_TRUE(https_server_->Start());
  }

  void TearDown() override {
    browser_content_client_.reset();
    content_client_.reset();
  }

  void SetUpCommandLine(base::CommandLine* command_line) override {
    InProcessBrowserTest::SetUpCommandLine(command_line);
    mock_cert_verifier_.SetUpCommandLine(command_line);
  }

  void SetUpInProcessBrowserTestFixture() override {
    InProcessBrowserTest::SetUpInProcessBrowserTestFixture();
    mock_cert_verifier_.SetUpInProcessBrowserTestFixture();
  }

  void TearDownInProcessBrowserTestFixture() override {
    InProcessBrowserTest::TearDownInProcessBrowserTestFixture();
    mock_cert_verifier_.TearDownInProcessBrowserTestFixture();
  }

  HostContentSettingsMap* content_settings() {
    return HostContentSettingsMapFactory::GetForProfile(browser()->profile());
  }

  void AllowFingerprinting(std::string domain) {
    brave_shields::SetFingerprintingControlType(
        content_settings(), ControlType::ALLOW,
        https_server()->GetURL(domain, "/"));
  }

  void BlockFingerprinting(std::string domain) {
    brave_shields::SetFingerprintingControlType(
        content_settings(), ControlType::BLOCK,
        https_server()->GetURL(domain, "/"));
  }

  void SetFingerprintingDefault(std::string domain) {
    brave_shields::SetFingerprintingControlType(
        content_settings(), ControlType::DEFAULT,
        https_server()->GetURL(domain, "/"));
  }

  content::WebContents* contents() {
    return browser()->tab_strip_model()->GetActiveWebContents();
  }

  bool NavigateToURLUntilLoadStop(const GURL& url) {
    EXPECT_TRUE(ui_test_utils::NavigateToURL(browser(), url));
    return WaitForLoadStop(contents());
  }

  net::EmbeddedTestServer* https_server() { return https_server_.get(); }

 private:
  content::ContentMockCertVerifier mock_cert_verifier_;
  std::unique_ptr<net::EmbeddedTestServer> https_server_;
  std::unique_ptr<ChromeContentClient> content_client_;
  std::unique_ptr<BraveContentBrowserClient> browser_content_client_;
  std::vector<std::string> user_agents_;
};

const char* testScript[] = {"window.screenX",
                            "window.screenY",
                            "window.screen.availLeft",
                            "window.screen.availTop",
                            "window.outerWidth - window.innerWidth",
                            "window.outerHeight - window.innerHeight",
                            "window.screen.availWidth - window.innerWidth",
                            "window.screen.availHeight - window.innerHeight",
                            "window.screen.width - window.innerWidth",
                            "window.screen.height - window.innerHeight"};

const char* mediaQueryTestScripts[] = {
    "matchMedia(`(max-device-width: ${innerWidth + 8}px) and "
    "(min-device-width: ${innerWidth}px)`).matches",
    "matchMedia(`(max-device-height: ${innerHeight + 8}px) and "
    "(min-device-height: ${innerHeight}px)`).matches"};

// Tests results of farbling known values
IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest, FarbleScreenSize) {
  std::string domain1 = "b.test";
  std::string domain2 = "d.test";
  GURL url1 = https_server()->GetURL(domain1, "/simple.html");
  GURL url2 = https_server()->GetURL(domain2, "/simple.html");
  AllowFingerprinting(domain1);
  NavigateToURLUntilLoadStop(url1);
  for (int i = 0; i < (int)std::size(testScript); ++i) {
    printf("off testIndex: %d\n", i);
    EXPECT_LE(0, EvalJs(contents(), testScript[i]));
  }
  for (int i = 0; i < (int)std::size(mediaQueryTestScripts); ++i) {
    printf("off testIndex: %d\n", i);
    EXPECT_EQ(false, EvalJs(contents(), mediaQueryTestScripts[i]));
  }
  SetFingerprintingDefault(domain1);
  NavigateToURLUntilLoadStop(url1);
  for (int i = 0; i < (int)std::size(testScript); ++i) {
    printf("on testIndex: %d\n", i);
    EXPECT_GE(8, EvalJs(contents(), testScript[i]));
  }
  for (int i = 0; i < (int)std::size(mediaQueryTestScripts); ++i) {
    printf("off testIndex: %d\n", i);
    EXPECT_EQ(true, EvalJs(contents(), mediaQueryTestScripts[i]));
  }
  browser()->window()->
}