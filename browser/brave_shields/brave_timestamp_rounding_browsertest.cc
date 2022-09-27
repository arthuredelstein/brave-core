/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>

#include "base/test/scoped_feature_list.h"
#include "base/test/scoped_mock_clock_override.h"
#include "brave/components/brave_shields/common/features.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/test/render_view_test.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/public/web/web_local_frame.h"
//#include "third_party/blink/renderer/core/timing/dom_window_performance.h"
#include "third_party/blink/public/web/web_performance.h"

using blink::features::kBraveRoundTimeStamps;

class BraveTimeStampRoundingRenderViewTest : public content::RenderViewTest {
 public:
  BraveTimeStampRoundingRenderViewTest() {
    feature_list_.InitAndDisableFeature(kBraveRoundTimeStamps);
  }
  ~BraveTimeStampRoundingRenderViewTest() override = default;

private:
  base::test::ScopedFeatureList feature_list_;
};

const std::u16string testScripts[] = {
  u"performance.now()",
  u"performance.timeOrigin",
  u"performance.getEntries().length + 0.5"
  //  u"performance.getEntries()[0].loadEventStart"
};


TEST_F(BraveTimeStampRoundingRenderViewTest, SynchronousApisRounded) {
  // Set time override.
  auto performance = GetMainFrame()->Performance();
  //base::TimeTicks before = base::TimeTicks::Now();
  //performance.ResetTimeOriginForTesting(before);
  
  //for (int j = 0; j < 10; ++j) {
    for (int i = 0; i < static_cast<int>(std::size(testScripts)); ++i) {
      double result;
      EXPECT_TRUE(ExecuteJavaScriptAndReturnNumberValue(testScripts[i], &result));
      std::cout << testScripts[i] << ": " << result << std::endl;
      EXPECT_EQ(round(result) - result, 0);
    }
    //}
  
}

