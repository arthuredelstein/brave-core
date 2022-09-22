/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/test/scoped_feature_list.h"
#include "brave/components/brave_shields/common/features.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/test/render_view_test.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/public/web/web_local_frame.h"

using brave_shields::features::kBraveRoundTimeStamps;

class BraveTimeStampRoundingRenderViewTest : public content::RenderViewTest {
 public:
  BraveTimeStampRoundingRenderViewTest() {
    feature_list_.InitAndEnableFeature(kBraveReduceLanguage);
  }
  ~BraveTimeStampRoundingRenderViewTest() override = default;

 private:
  base::test::ScopedFeatureList feature_list_;
};

TEST_F(BraveTimeStampRoundingRenderViewTest, SynchronousApisRounded) {
  double result;
  EXPECT_TRUE(ExecuteJavaScriptAndReturnNumberValue("performance.now()", &result));
  EXPECT_EQ(result, round(result));
}

