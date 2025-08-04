// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/email_aliases/email_aliases_service.h"

#include "base/functional/bind.h"
#include "base/test/bind.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/task_environment.h"
#include "brave/components/email_aliases/features.h"
#include "services/network/public/cpp/weak_wrapper_shared_url_loader_factory.h"
#include "services/network/test/test_url_loader_factory.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace email_aliases {

class EmailAliasesServiceTest : public ::testing::Test {
 protected:
  EmailAliasesServiceTest() {
    feature_list_.InitAndEnableFeature(email_aliases::kEmailAliases);
    url_loader_factory_ =
        base::MakeRefCounted<network::WeakWrapperSharedURLLoaderFactory>(
            &test_url_loader_factory_);
    service_ = std::make_unique<EmailAliasesService>(url_loader_factory_);
  }

  void CallRequestAuthenticationAndCheck(
      const std::string& email,
      const std::string& response_body,
      const std::optional<std::string>& expected_error = std::nullopt) {
    static constexpr char kVerifyInitUrl[] = "https://accounts.bsg.bravesoftware.com/v2/verify/init";
    bool called = false;
    std::optional<std::string> error;
    test_url_loader_factory_.AddResponse(kVerifyInitUrl, response_body);
    base::RunLoop run_loop;
    service_->RequestAuthentication(
        email,
        base::BindOnce(
            [](bool* called, std::optional<std::string>* error, base::RunLoop* run_loop, const std::optional<std::string>& result) {
                *called = true;
                *error = result;
                run_loop->Quit();
            },
            &called, &error, &run_loop));
    run_loop.Run();
    EXPECT_TRUE(called);
    if (expected_error) {
      ASSERT_TRUE(error.has_value());
      EXPECT_EQ(*error, *expected_error);
    } else {
      EXPECT_FALSE(error.has_value());
    }
  }

  base::test::ScopedFeatureList feature_list_;
  network::TestURLLoaderFactory test_url_loader_factory_;
  scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory_;
  std::unique_ptr<EmailAliasesService> service_;
  base::test::TaskEnvironment task_environment_;
};

TEST_F(EmailAliasesServiceTest, ConstructAndShutdown) {
  EXPECT_TRUE(service_);
  service_->Shutdown();
}

TEST_F(EmailAliasesServiceTest, RequestAuthentication_EmptyEmail) {
  CallRequestAuthenticationAndCheck("",  // empty email
                                    "dummy body", "No email provided");
}

TEST_F(EmailAliasesServiceTest, RequestAuthentication_InvalidJson) {
  CallRequestAuthenticationAndCheck("test@example.com", "not a json",
                                    "Invalid response body");
}

TEST_F(EmailAliasesServiceTest, RequestAuthentication_NoVerificationToken) {
  CallRequestAuthenticationAndCheck("test@example.com", "{\"foo\":\"bar\"}",
                                    "No verification token");
}

TEST_F(EmailAliasesServiceTest, RequestAuthentication_Success) {
  CallRequestAuthenticationAndCheck("test@example.com",
                                    "{\"verificationToken\":\"token123\"}");
}

}  // namespace email_aliases
