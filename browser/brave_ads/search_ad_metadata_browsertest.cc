/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/path_service.h"
#include "base/strings/strcat.h"
#include "base/strings/string_number_conversions.h"
#include "brave/browser/brave_ads/ads_service_factory.h"
#include "brave/browser/brave_ads/search_ad_metadata_handler.h"
#include "brave/common/brave_paths.h"
#include "brave/components/brave_ads/browser/ads_service.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/content_mock_cert_verifier.h"
#include "net/dns/mock_host_resolver.h"

// npm run test -- brave_browser_tests --filter=SearchAdMetadataTest*

namespace {

constexpr char kAllowedDomain[] = "search.brave.com";

void CheckSampleSearchAdMetadataList(
    const brave_ads::SearchResultAdsList& ads_list) {
  EXPECT_EQ(ads_list.size(), 2u);

  for (size_t i = 0; i < ads_list.size(); ++i) {
    const auto& search_ad = ads_list[i];
    const std::string index = base::StrCat({"-", base::NumberToString(i + 1)});
    EXPECT_EQ(search_ad->placement_id,
              base::StrCat({"data-placement-id", index}));
    EXPECT_EQ(search_ad->creative_instance_id,
              base::StrCat({"data-creative-instance-id", index}));
    EXPECT_EQ(search_ad->creative_set_id,
              base::StrCat({"data-creative-set-id", index}));
    EXPECT_EQ(search_ad->campaign_id,
              base::StrCat({"data-campaign-id", index}));
    EXPECT_EQ(search_ad->advertiser_id,
              base::StrCat({"data-advertiser-id", index}));
    EXPECT_EQ(search_ad->target_url,
              base::StrCat({"data-landing-page", index}));
    EXPECT_EQ(search_ad->headline_text,
              base::StrCat({"data-headline-text", index}));
    EXPECT_EQ(search_ad->description,
              base::StrCat({"data-description", index}));
    EXPECT_EQ(search_ad->value, 0.5 + i + 1);

    EXPECT_EQ(search_ad->conversion->type,
              base::StrCat({"data-conversion-type-value", index}));
    EXPECT_EQ(search_ad->conversion->url_pattern,
              base::StrCat({"data-conversion-url-pattern-value", index}));
    EXPECT_EQ(
        search_ad->conversion->advertiser_public_key,
        base::StrCat({"data-conversion-advertiser-public-key-value", index}));
    EXPECT_EQ(static_cast<size_t>(search_ad->conversion->observation_window),
              i + 1);
  }
}

}  // namespace

class SearchAdMetadataTest : public InProcessBrowserTest {
 public:
  SearchAdMetadataTest() {}

  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();
    mock_cert_verifier_.mock_cert_verifier()->set_default_result(net::OK);
    host_resolver()->AddRule("*", "127.0.0.1");

    https_server_.reset(new net::EmbeddedTestServer(
        net::test_server::EmbeddedTestServer::TYPE_HTTPS));

    brave::RegisterPathProvider();
    base::FilePath test_data_dir;
    base::PathService::Get(brave::DIR_TEST_DATA, &test_data_dir);
    https_server_->ServeFilesFromDirectory(test_data_dir);
    ASSERT_TRUE(https_server_->Start());

    brave_ads::AdsService* ads_service =
        brave_ads::AdsServiceFactory::GetForProfile(browser()->profile());
    ads_service->SetEnabled(true);
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
    mock_cert_verifier_.TearDownInProcessBrowserTestFixture();
    InProcessBrowserTest::TearDownInProcessBrowserTestFixture();
  }

  net::EmbeddedTestServer* https_server() { return https_server_.get(); }

  content::WebContents* LoadTestDataUrl(const std::string& relative_path) {
    GURL url = https_server()->GetURL(kAllowedDomain, relative_path);

    EXPECT_TRUE(ui_test_utils::NavigateToURL(browser(), url));
    content::WebContents* web_contents =
        browser()->tab_strip_model()->GetActiveWebContents();
    EXPECT_EQ(url, web_contents->GetVisibleURL());

    return web_contents;
  }

 private:
  content::ContentMockCertVerifier mock_cert_verifier_;
  std::unique_ptr<net::EmbeddedTestServer> https_server_;
};

IN_PROC_BROWSER_TEST_F(SearchAdMetadataTest, SampleSearchAdMetadata) {
  content::WebContents* web_contents =
      LoadTestDataUrl("/brave_ads/search_ad_sample_metadata.html");

  brave_ads::SearchAdMetadataHandler search_ad_metadata_handler;
  EXPECT_TRUE(search_ad_metadata_handler.IsAllowedBraveSearchHost(
      web_contents->GetVisibleURL()));

  content::RenderFrameHost* render_frame_host = web_contents->GetMainFrame();
  base::RunLoop run_loop;
  search_ad_metadata_handler.RetrieveSearchAdMetadata(
      render_frame_host,
      base::BindOnce(
          [](base::RunLoop* run_loop, brave_ads::SearchResultAdsList ads_list) {
            CheckSampleSearchAdMetadataList(ads_list);
            run_loop->Quit();
          },
          base::Unretained(&run_loop)));
  run_loop.Run();
}

IN_PROC_BROWSER_TEST_F(SearchAdMetadataTest, NoSearchAdMetadata) {
  content::WebContents* web_contents = LoadTestDataUrl("/simple.html");

  brave_ads::SearchAdMetadataHandler search_ad_metadata_handler;
  EXPECT_TRUE(search_ad_metadata_handler.IsAllowedBraveSearchHost(
      web_contents->GetVisibleURL()));

  content::RenderFrameHost* render_frame_host = web_contents->GetMainFrame();
  base::RunLoop run_loop;
  search_ad_metadata_handler.RetrieveSearchAdMetadata(
      render_frame_host,
      base::BindOnce(
          [](base::RunLoop* run_loop, brave_ads::SearchResultAdsList ads_list) {
            EXPECT_TRUE(ads_list.empty());
            run_loop->Quit();
          },
          base::Unretained(&run_loop)));
  run_loop.Run();
}

IN_PROC_BROWSER_TEST_F(SearchAdMetadataTest, BrokenSearchAdMetadata) {
  content::WebContents* web_contents =
      LoadTestDataUrl("/brave_ads/search_ad_broken_metadata.html");

  brave_ads::SearchAdMetadataHandler search_ad_metadata_handler;
  EXPECT_TRUE(search_ad_metadata_handler.IsAllowedBraveSearchHost(
      web_contents->GetVisibleURL()));

  content::RenderFrameHost* render_frame_host = web_contents->GetMainFrame();
  base::RunLoop run_loop;
  search_ad_metadata_handler.RetrieveSearchAdMetadata(
      render_frame_host,
      base::BindOnce(
          [](base::RunLoop* run_loop, brave_ads::SearchResultAdsList ads_list) {
            EXPECT_TRUE(ads_list.empty());
            run_loop->Quit();
          },
          base::Unretained(&run_loop)));
  run_loop.Run();
}
