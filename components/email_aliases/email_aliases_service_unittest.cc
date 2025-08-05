// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/email_aliases/email_aliases_service.h"

#include <string>
#include <utility>
#include <vector>

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

  // Low-level helper for authentication requests
  // Returns the error string if any, or std::nullopt on success.
  std::optional<std::string> RequestAuthenticationWithResponse(
      const std::string& email,
      const std::string& response_body) {
    static constexpr char kVerifyInitUrl[] =
        "https://accounts.bsg.bravesoftware.com/v2/verify/init";
    test_url_loader_factory_.AddResponse(kVerifyInitUrl, response_body);
    bool called = false;
    std::optional<std::string> error;
    service_->RequestAuthentication(
        email, base::BindOnce(
                   [](bool* called, std::optional<std::string>* error,
                      const std::optional<std::string>& result) {
                     *called = true;
                     *error = result;
                   },
                   &called, &error));
    base::RunLoop().RunUntilIdle();
    EXPECT_TRUE(called);
    return error;
  }

  void CallRequestAuthenticationAndCheck(
      const std::string& email,
      const std::string& response_body,
      const std::optional<std::string>& expected_error = std::nullopt) {
    auto error = RequestAuthenticationWithResponse(email, response_body);
    if (expected_error) {
      EXPECT_TRUE(error.has_value());
      EXPECT_EQ(*error, *expected_error);
    } else {
      EXPECT_FALSE(error.has_value());
    }
  }

  // Test observer for authentication state changes
  class TestObserver
      : public email_aliases::mojom::EmailAliasesServiceObserver {
   public:
    void OnAuthStateChanged(email_aliases::mojom::AuthStatePtr state) override {
      last_state = state->status;
      call_count++;
    }
    void OnAliasesUpdated(
        std::vector<email_aliases::mojom::AliasPtr>) override {}
    email_aliases::mojom::AuthenticationStatus last_state =
        email_aliases::mojom::AuthenticationStatus::kUnauthenticated;
    int call_count = 0;
    mojo::Receiver<email_aliases::mojom::EmailAliasesServiceObserver> receiver_{
        this};
    void BindReceiver(
        mojo::PendingReceiver<email_aliases::mojom::EmailAliasesServiceObserver>
            pending) {
      receiver_.Bind(std::move(pending));
    }
  };

  // Helper for RequestSession tests
  struct RequestSessionTestResult {
    std::unique_ptr<TestObserver> observer;
  };

  RequestSessionTestResult RunRequestSessionTest(
      const std::vector<std::string>& responses) {
    auto observer = std::make_unique<TestObserver>();
    mojo::PendingRemote<email_aliases::mojom::EmailAliasesServiceObserver>
        remote;
    observer->BindReceiver(remote.InitWithNewPipeAndPassReceiver());
    service_->AddObserver(std::move(remote));

    // Authenticate and set the verification token via the public API
    auto error = RequestAuthenticationWithResponse(
        "test@example.com", "{\"verificationToken\":\"token123\"}");
    EXPECT_FALSE(error.has_value());

    for (const auto& body : responses) {
      test_url_loader_factory_.AddResponse(
          "https://accounts.bsg.bravesoftware.com/v2/verify/result", body);
    }

    base::RunLoop().RunUntilIdle();

    RequestSessionTestResult result;
    result.observer = std::move(observer);
    return result;
  }

  base::test::ScopedFeatureList feature_list_;
  network::TestURLLoaderFactory test_url_loader_factory_;
  scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory_;
  std::unique_ptr<EmailAliasesService> service_;
  base::test::TaskEnvironment task_environment_;
};

TEST_F(EmailAliasesServiceTest, RequestAuthentication_EmptyEmail) {
  CallRequestAuthenticationAndCheck("", "dummy body", "No email provided");
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

TEST_F(EmailAliasesServiceTest, RequestSession_Success) {
  auto result = RunRequestSessionTest({"{\"authToken\":\"auth456\"}"});
  EXPECT_EQ(service_->GetAuthTokenForTesting(), "auth456");
  // unauthenticated, authenticating, authenticated
  EXPECT_EQ(result.observer->call_count, 3);
  EXPECT_EQ(result.observer->last_state,
            email_aliases::mojom::AuthenticationStatus::kAuthenticated);
}

TEST_F(EmailAliasesServiceTest, RequestSession_InvalidJson) {
  auto result = RunRequestSessionTest({"not a json"});
  // unauthenticated, authenticating
  EXPECT_EQ(result.observer->call_count, 2);
  EXPECT_EQ(result.observer->last_state,
            email_aliases::mojom::AuthenticationStatus::kAuthenticating);
}

TEST_F(EmailAliasesServiceTest, RequestSession_RetryOnMissingAuthToken) {
  auto result = RunRequestSessionTest({
      "{\"foo\":\"bar\"}",           // triggers retry
      "{\"authToken\":\"auth456\"}"  // success
  });
  EXPECT_EQ(service_->GetAuthTokenForTesting(), "auth456");
  // unauthenticated, authenticating, authenticated
  EXPECT_EQ(result.observer->call_count, 3);
  EXPECT_EQ(result.observer->last_state,
            email_aliases::mojom::AuthenticationStatus::kAuthenticated);
}

}  // namespace email_aliases
