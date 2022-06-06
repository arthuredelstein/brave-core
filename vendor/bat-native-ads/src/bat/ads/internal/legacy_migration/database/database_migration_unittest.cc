/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/strings/stringprintf.h"
#include "bat/ads/internal/ad_events/ad_event_info.h"
#include "bat/ads/internal/ad_events/ad_event_unittest_util.h"
#include "bat/ads/internal/ad_events/ad_events.h"
#include "bat/ads/internal/base/unittest_base.h"
#include "bat/ads/internal/base/unittest_time_util.h"
#include "bat/ads/internal/base/unittest_util.h"
#include "bat/ads/internal/creatives/creative_ad_info.h"
#include "bat/ads/internal/creatives/creative_ad_unittest_util.h"
#include "bat/ads/internal/legacy_migration/database/database_constants.h"

// npm run test -- brave_unit_tests --filter=BatAds*

namespace ads {

class BatAdsDatabaseMigrationTest : public UnitTestBase,
                                    public ::testing::WithParamInterface<int> {
 protected:
  BatAdsDatabaseMigrationTest() = default;

  ~BatAdsDatabaseMigrationTest() override = default;

  void SetUp() override {
    const std::string source_filename =
        base::StringPrintf("database_schema_%d.sqlite", GetSchemaVersion());

    ASSERT_TRUE(
        CopyFileFromTestPathToTempDir(source_filename, "database.sqlite"))
        << "Failed to copy " << source_filename;

    UnitTestBase::SetUpForTesting(/* is_integration_test */ false);
  }

  int GetSchemaVersion() { return GetParam() + 1; }
};

TEST_P(BatAdsDatabaseMigrationTest, MigrateFromSchema) {
  // Arrange
  const CreativeAdInfo& creative_ad = BuildCreativeAd();
  const AdEventInfo& ad_event = BuildAdEvent(
      creative_ad, AdType::kNotificationAd, ConfirmationType::kViewed, Now());

  // Act
  LogAdEvent(ad_event, [=](const bool success) {
    EXPECT_TRUE(success) << "Failed to migrate database from schema version "
                         << GetSchemaVersion() << " to schema version "
                         << database::kVersion;
  });

  // Assert
}

std::string TestParamToString(::testing::TestParamInfo<int> param_info) {
  return base::StringPrintf("%d_to_%d", param_info.param + 1,
                            database::kVersion);
}

INSTANTIATE_TEST_SUITE_P(BatAdsDatabaseMigration,
                         BatAdsDatabaseMigrationTest,
                         testing::Range(0, database::kVersion),
                         TestParamToString);

}  // namespace ads
