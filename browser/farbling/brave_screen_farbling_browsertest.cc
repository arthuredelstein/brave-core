/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/path_service.h"
#include "base/test/scoped_feature_list.h"
#include "brave/browser/brave_content_browser_client.h"
#include "brave/components/brave_shields/browser/brave_shields_util.h"
#include "brave/components/constants/brave_paths.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/extensions/extension_browsertest.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/common/chrome_content_client.h"
#include "chrome/test/base/ui_test_utils.h"
#include "content/public/test/browser_test.h"
#include "net/dns/mock_host_resolver.h"
#include "net/test/embedded_test_server/embedded_test_server.h"
#include "net/test/embedded_test_server/http_request.h"
#include "third_party/blink/public/common/features.h"

using brave_shields::ControlType;

const gfx::Rect testWindowBounds[] = {
    gfx::Rect(50, 50, 100, 100), gfx::Rect(50, 50, 100, 0),
    gfx::Rect(200, 100, 0, 100), gfx::Rect(0, 0, 0, 0)};

class BraveScreenFarblingBrowserTest : public InProcessBrowserTest {
 public:
  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();

    content_client_.reset(new ChromeContentClient);
    content::SetContentClient(content_client_.get());
    browser_content_client_.reset(new BraveContentBrowserClient());
    content::SetBrowserClientForTesting(browser_content_client_.get());

    host_resolver()->AddRule("*", "127.0.0.1");
    content::SetupCrossSiteRedirector(embedded_test_server());

    brave::RegisterPathProvider();
    base::FilePath test_data_dir;
    base::PathService::Get(brave::DIR_TEST_DATA, &test_data_dir);
    embedded_test_server()->ServeFilesFromDirectory(test_data_dir);

    ASSERT_TRUE(embedded_test_server()->Start());

    top_level_page_url_ = embedded_test_server()->GetURL("a.com", "/");
    farbling_url_ = embedded_test_server()->GetURL("a.com", "/simple.html");
  }

  void TearDown() override {
    browser_content_client_.reset();
    content_client_.reset();
  }

  HostContentSettingsMap* content_settings() {
    return HostContentSettingsMapFactory::GetForProfile(browser()->profile());
  }

  void SetFingerprintingSetting(bool allow) {
    brave_shields::SetFingerprintingControlType(
        content_settings(), allow ? ControlType::ALLOW : ControlType::DEFAULT,
        top_level_page_url_);
  }

  content::WebContents* contents() const {
    return browser()->tab_strip_model()->GetActiveWebContents();
  }

  bool NavigateToURLUntilLoadStop(const GURL& url) {
    EXPECT_TRUE(ui_test_utils::NavigateToURL(browser(), url));
    return WaitForLoadStop(contents());
  }

  Browser* OpenPopup(const std::string& script) const {
    content::ExecuteScriptAsync(contents(), script);
    Browser* popup = ui_test_utils::WaitForBrowserToOpen();
    EXPECT_NE(popup, browser());
    auto* popup_contents = popup->tab_strip_model()->GetActiveWebContents();
    EXPECT_TRUE(WaitForRenderFrameReady(popup_contents->GetMainFrame()));
    return popup;
  }

  virtual bool DisableFlag() const { return false; }

  const GURL& farbling_url() { return farbling_url_; }

  void FarbleScreenSize() {
    const char* testScreenSizeScripts[] = {
        "window.outerWidth - window.innerWidth",
        "window.outerHeight - window.innerHeight",
        "window.screen.availWidth - window.innerWidth",
        "window.screen.availHeight - window.innerHeight",
        "window.screen.width - window.innerWidth",
        "window.screen.height - window.innerHeight",
    };
    for (bool allow_fingerprinting : {false, true}) {
      SetFingerprintingSetting(allow_fingerprinting);
      for (int j = 0; j < static_cast<int>(std::size(testWindowBounds)); ++j) {
        browser()->window()->SetBounds(testWindowBounds[j]);
        NavigateToURLUntilLoadStop(farbling_url());
        for (int i = 0; i < static_cast<int>(std::size(testScreenSizeScripts));
             ++i) {
          std::string testScreenSizeScriptsAbs =
              std::string("Math.abs(") + testScreenSizeScripts[i] + ")";
          if (!allow_fingerprinting && !DisableFlag()) {
            EXPECT_GE(8, EvalJs(contents(), testScreenSizeScriptsAbs));
          } else {
            EXPECT_LT(8, EvalJs(contents(), testScreenSizeScriptsAbs));
          }
        }
      }
    }
  }

#define PREPARE_TEST_EVENT                                   \
  "let fakeScreenX = 100, fakeScreenY = 200; "               \
  "let fakeClientX = 300, fakeClientY = 400; "               \
  "let testEvent = document.createEvent('MouseEvent'); "     \
  "testEvent.initMouseEvent('click', true, true, window, 1," \
  "fakeScreenX + devicePixelRatio * fakeClientX,"            \
  "fakeScreenY + devicePixelRatio * fakeClientY,"            \
  "fakeClientX, fakeClientY, false, false, false, false, 0, null); "

  void FarbleWindowPosition() {
    for (bool allow_fingerprinting : {false, true}) {
      for (int j = 0; j < static_cast<int>(std::size(testWindowBounds)); ++j) {
        browser()->window()->SetBounds(testWindowBounds[j]);
        SetFingerprintingSetting(allow_fingerprinting);
        NavigateToURLUntilLoadStop(farbling_url());
        if (!allow_fingerprinting && !DisableFlag()) {
          EXPECT_GE(8, EvalJs(contents(), "window.screenX"));
          EXPECT_GE(8, EvalJs(contents(), "window.screenY"));
          EXPECT_GE(8, EvalJs(contents(), "window.screen.availLeft"));
          EXPECT_GE(8, EvalJs(contents(), "window.screen.availTop"));
          EXPECT_GE(
              8,
              EvalJs(
                  contents(), PREPARE_TEST_EVENT
                  "testEvent.screenX - devicePixelRatio * testEvent.clientX"));
          EXPECT_GE(
              8,
              EvalJs(
                  contents(), PREPARE_TEST_EVENT
                  "testEvent.screenY - devicePixelRatio * testEvent.clientY"));
        } else {
          EXPECT_LE(testWindowBounds[j].x(),
                    EvalJs(contents(), "window.screenX"));
          EXPECT_LE(testWindowBounds[j].y(),
                    EvalJs(contents(), "window.screenY"));
        }
      }
    }
  }

  void FarbleScreenMediaQuery() {
    for (bool allow_fingerprinting : {false, true}) {
      for (int j = 0; j < static_cast<int>(std::size(testWindowBounds)); ++j) {
        browser()->window()->SetBounds(testWindowBounds[j]);
        SetFingerprintingSetting(allow_fingerprinting);
        NavigateToURLUntilLoadStop(farbling_url());
        EXPECT_EQ(
            !allow_fingerprinting && !DisableFlag(),
            EvalJs(contents(),
                   "matchMedia(`(max-device-width: ${innerWidth + 8}px) and "
                   "(min-device-width: ${innerWidth}px)`).matches"));
        EXPECT_EQ(
            !allow_fingerprinting && !DisableFlag(),
            EvalJs(contents(),
                   "matchMedia(`(max-device-height: ${innerHeight + 8}px) and "
                   "(min-device-height: ${innerHeight}px)`).matches"));
      }
    }
  }

  void FarbleScreenPopupPosition() {
    for (bool allow_fingerprinting : {false, true}) {
      for (int j = 0; j < static_cast<int>(std::size(testWindowBounds)); ++j) {
        browser()->window()->SetBounds(testWindowBounds[j]);
        SetFingerprintingSetting(allow_fingerprinting);
        NavigateToURLUntilLoadStop(farbling_url());
        gfx::Rect parentBounds = browser()->window()->GetBounds();
        const char* script =
            "open('http://d.test/', '', `"
            "left=${screen.availLeft + 10},"
            "top=${screen.availTop + 10},"
            "width=${screen.availWidth - 10},"
            "height=${screen.availHeight - 10}"
            "`);";
        Browser* popup = OpenPopup(script);
        gfx::Rect childBounds = popup->window()->GetBounds();
        auto* parent_contents = contents();
        auto* popup_contents = popup->tab_strip_model()->GetActiveWebContents();
        const int popup_inner_width =
            EvalJs(popup_contents, "innerWidth").value.GetInt();
        const int popup_inner_height =
            EvalJs(popup_contents, "innerHeight").value.GetInt();
        if (!allow_fingerprinting && !DisableFlag()) {
          EXPECT_GE(childBounds.x(), 10 + parentBounds.x());
          EXPECT_GE(childBounds.y(), 10 + parentBounds.y());
          EXPECT_LE(childBounds.x(), 18 + parentBounds.x());
          EXPECT_LE(childBounds.y(), 18 + parentBounds.y());
          EXPECT_LE(popup_inner_width,
                    EvalJs(parent_contents, "innerWidth + 8"));
          EXPECT_LE(popup_inner_height,
                    EvalJs(parent_contents, "innerHeight + 8"));
        } else {
          EXPECT_LE(childBounds.x(), 10 + parentBounds.x());
          EXPECT_LE(childBounds.y(), 10 + parentBounds.y());
          EXPECT_GE(popup_inner_width, EvalJs(parent_contents, "innerWidth"));
          EXPECT_GE(popup_inner_height, EvalJs(parent_contents, "innerHeight"));
        }
      }
    }
  }

 protected:
  base::test::ScopedFeatureList feature_list_;

 private:
  GURL top_level_page_url_;
  GURL farbling_url_;
  std::unique_ptr<ChromeContentClient> content_client_;
  std::unique_ptr<BraveContentBrowserClient> browser_content_client_;
};

class BraveScreenFarblingBrowserTest_DisableFlag
    : public BraveScreenFarblingBrowserTest {
 public:
  BraveScreenFarblingBrowserTest_DisableFlag() {
    feature_list_.InitAndDisableFeature(
        blink::features::kBraveBlockScreenFingerprinting);
  }

  bool DisableFlag() const override { return true; }
};

IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest, FarbleScreenSize) {
  FarbleScreenSize();
}

IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest_DisableFlag,
                       FarbleScreenSize_DisableFlag) {
  FarbleScreenSize();
}

IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest, FarbleWindowPosition) {
  FarbleWindowPosition();
}

IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest_DisableFlag,
                       FarbleWindowPosition_DisableFlag) {
  FarbleWindowPosition();
}

IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest, FarbleScreenMediaQuery) {
  FarbleScreenMediaQuery();
}

IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest_DisableFlag,
                       FarbleScreenMediaQuery_DisableFlag) {
  FarbleScreenMediaQuery();
}

IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest,
                       FarbleScreenPopupPosition) {
  FarbleScreenPopupPosition();
}

IN_PROC_BROWSER_TEST_F(BraveScreenFarblingBrowserTest_DisableFlag,
                       FarbleScreenPopupPosition_DisableFlag) {
  FarbleScreenPopupPosition();
}
