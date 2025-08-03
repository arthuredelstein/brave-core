// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/email_aliases/email_aliases_service.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "base/test/scoped_feature_list.h"
#include "brave/components/email_aliases/features.h"
#include "services/network/test/test_shared_url_loader_factory.h"
#include "base/test/task_environment.h"

namespace email_aliases {

class EmailAliasesServiceTest : public ::testing::Test {
 protected:
  EmailAliasesServiceTest() {
    feature_list_.InitAndEnableFeature(email_aliases::kEmailAliases);
    url_loader_factory_ = base::MakeRefCounted<network::TestSharedURLLoaderFactory>();
    service_ = std::make_unique<EmailAliasesService>(url_loader_factory_);
  }

  void RunLoop() { task_environment_.RunUntilIdle(); }

  base::test::ScopedFeatureList feature_list_;
  scoped_refptr<network::TestSharedURLLoaderFactory> url_loader_factory_;
  std::unique_ptr<EmailAliasesService> service_;
  base::test::TaskEnvironment task_environment_;
};



TEST_F(EmailAliasesServiceTest, ConstructAndShutdown) {
  EXPECT_TRUE(service_);
  service_->Shutdown();
}

TEST_F(EmailAliasesServiceTest, RequestAuthenticationPlaceholder) {
  // TODO: Implement a real test with a mock/fake network response.
  // This is a placeholder to show test structure.
  EXPECT_TRUE(service_);
}

}  // namespace email_aliases
