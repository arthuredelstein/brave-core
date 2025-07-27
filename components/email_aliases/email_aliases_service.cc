/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/email_aliases/email_aliases_service.h"

#include <utility>

#include "base/check.h"
#include "base/feature_list.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "brave/components/email_aliases/email_aliases.mojom.h"
#include "brave/components/email_aliases/features.h"
#include "net/base/net_errors.h"
#include "net/base/url_util.h"
#include "net/http/http_status_code.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "services/network/public/cpp/simple_url_loader.h"

namespace email_aliases {

namespace {

const char kAccountsServiceRequestURL[] =
    "https://accounts.bsg.bravesoftware.com/v2/verify/init";
//const char kAccountsServiceVerifyURL[] =
//    "https://accounts.bsg.bravesoftware.com/v2/verify/result";
const char kBraveApiKey[] = "px6zQ7rIMGaS8FE6cmpUp45WQTFJYXgo7ZlBhrFK";

const net::NetworkTrafficAnnotationTag traffic_annotation =
    net::DefineNetworkTrafficAnnotation("email_aliases_mapping_service", R"(
    semantics {
      sender: "Email Aliases service"
      description:
        "Call Email Aliases Mapping Service API"
      trigger:
        "When the user connects to the Email Mapping Service, to "
        "Generate, Create, Read, Update, or Delete Email Aliases. "
      destination: BRAVE_OWNED_SERVICE
    }
    policy {
      cookies_allowed: YES
  })");

constexpr int kMaxResponseLength = 32768;

}  // namespace

EmailAliasesService::EmailAliasesService(
    scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory)
    : url_loader_factory_(url_loader_factory) {
  CHECK(base::FeatureList::IsEnabled(email_aliases::kEmailAliases));
}

EmailAliasesService::~EmailAliasesService() = default;

void EmailAliasesService::Shutdown() {
  receivers_.Clear();
  observers_.Clear();
}

void EmailAliasesService::BindInterface(
    mojo::PendingReceiver<mojom::EmailAliasesService> receiver) {
  receivers_.Add(this, std::move(receiver));
}

void EmailAliasesService::ApiFetch(
    const GURL& url,
    const char* method,
    const std::optional<std::string>& bearer_token,
    const base::Value::Dict& bodyValue,
    BodyAsStringCallback download_to_string_callback) {
  auto resource_request = std::make_unique<network::ResourceRequest>();
  resource_request->url = url;
  resource_request->method = method;
  if (bearer_token) {
    resource_request->headers.SetHeader(
        "Authorization", std::string("Bearer ") + bearer_token.value());
  }
  resource_request->headers.SetHeader("X-API-key", kBraveApiKey);
  std::unique_ptr<network::SimpleURLLoader> simple_url_loader = network::SimpleURLLoader::Create(
      std::move(resource_request), traffic_annotation);
  if (!bodyValue.empty() && method != net::HttpRequestHeaders::kGetMethod &&
      method != net::HttpRequestHeaders::kHeadMethod) {
    auto body = base::WriteJson(bodyValue);
    CHECK(body);
    simple_url_loader->AttachStringForUpload(body.value(), "application/json");
  }
  simple_url_loader->DownloadToString(
    url_loader_factory_.get(),
    std::move(download_to_string_callback), kMaxResponseLength);
}

void EmailAliasesService::RequestAuthentication(
    const std::string& auth_email,
    RequestAuthenticationCallback callback) {
    const auto bodyValue = base::Value::Dict()
                             .Set("email", auth_email)
                             .Set("intent", "auth_token")
                             .Set("service", "email-aliases");
  ApiFetch(
      GURL(kAccountsServiceRequestURL),
      net::HttpRequestHeaders::kPostMethod,
      std::nullopt, bodyValue,
      base::BindOnce(&EmailAliasesService::OnRequestAuthenticationResponse,
                     weak_factory_.GetWeakPtr(), std::move(callback)));
}

void EmailAliasesService::OnRequestAuthenticationResponse(
    RequestAuthenticationCallback callback,
    std::optional<std::string> response_body) {
  if (!response_body) {
    std::move(callback).Run("No response body");
    return;
  }
  const auto response_body_dict = base::JSONReader::Read(*response_body);
  if (!response_body_dict || !response_body_dict->is_dict()) {
    std::move(callback).Run("Invalid response body");
    return;
  }
  const auto* auth_token_ptr = response_body_dict->GetDict().FindString("auth_token");
  if (!auth_token_ptr) {
    std::move(callback).Run("No auth token");
    return;
  }
  // Success
  auth_token_ = *auth_token_ptr;
  std::move(callback).Run(std::nullopt);
}

void EmailAliasesService::CancelAuthenticationOrLogout(
    CancelAuthenticationOrLogoutCallback callback) {
  // TODO: Implement logout logic
  std::move(callback).Run();
}

void EmailAliasesService::GenerateAlias(GenerateAliasCallback callback) {
  // TODO: Implement alias generation logic
  mojom::GenerateAliasResultPtr result =
      mojom::GenerateAliasResult::NewErrorMessage("Not implemented");
  std::move(callback).Run(std::move(result));
}

void EmailAliasesService::UpdateAlias(const std::string& alias_email,
                                      const std::optional<std::string>& note,
                                      UpdateAliasCallback callback) {
  // TODO: Implement alias update logic
  std::move(callback).Run("Not implemented");
}

void EmailAliasesService::DeleteAlias(const std::string& alias_email,
                                      DeleteAliasCallback callback) {
  // TODO: Implement alias deletion logic
  std::move(callback).Run("Not implemented");
}

void EmailAliasesService::AddObserver(
    mojo::PendingRemote<mojom::EmailAliasesServiceObserver> observer) {
  observers_.Add(std::move(observer));
}

}  // namespace email_aliases
