/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/settings/brave_inbox_aliases_handler.h"

#include <string>

#include "base/functional/bind.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/values.h"
#include "brave/browser/brave_browser_process.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_finder.h"
#include "content/public/browser/web_ui.h"
#include "net/http/http_request_headers.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "url/gurl.h"
#include "base/functional/callback.h"

#define MAX_RESPONSE_LENGTH 32768

const std::string kAccountsServiceRequestURL = "https://accounts.bsg.bravesoftware.com/v2/verify/init";
const std::string kAccountsServiceVerifyURL = "https://accounts.bsg.bravesoftware.com/v2/verify/result";
const std::string kMappingServiceManageURL = "https://aliases.bsg.bravesoftware.com/manage";
const std::string kMappingServiceGenerateURL = "https://aliases.bsg.bravesoftware.com/generate";
const std::string kBraveApiKey = "px6zQ7rIMGaS8FE6cmpUp45WQTFJYXgo7ZlBhrFK";
const std::string kTempSessionKey = "eyJhbGciOiJFUzI1NiIsImtpZCI6MSwidHlwIjoiSldUIn0.eyJpYXQiOjE3MzM1MTMyMzcsInNlc3Npb25faWQiOiIwMTkzOWQ3MC1kMmRhLTc1M2ItOTc5Mi05ZWY0ZWM5MDJmMzEifQ.PspbaYEATiOsvTJSgy8wotag4aQHmStzga8HxAIdcw6qgCbv4IdkkxpFBZgv55vsvEh1djQAJnsWzwzHkKb9Mg";

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



BraveInboxAliasesHandler::BraveInboxAliasesHandler() = default;

BraveInboxAliasesHandler::~BraveInboxAliasesHandler() = default;

void BraveInboxAliasesHandler::RegisterMessages() {
  profile_ = Profile::FromWebUI(web_ui());
  web_ui()->RegisterMessageCallback(
      "email_aliases.generateAlias",
      base::BindRepeating(&BraveInboxAliasesHandler::GenerateAlias,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "email_aliases.getAliases",
      base::BindRepeating(&BraveInboxAliasesHandler::GetAliases,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "email_aliases.createAlias",
      base::BindRepeating(&BraveInboxAliasesHandler::CreateAlias,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "email_aliases.updateAlias",
      base::BindRepeating(&BraveInboxAliasesHandler::UpdateAlias,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "email_aliases.deleteAlias",
      base::BindRepeating(&BraveInboxAliasesHandler::DeleteAlias,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "email_aliases.requestAccount",
      base::BindRepeating(&BraveInboxAliasesHandler::RequestAccount,
                          base::Unretained(this)));
}

void BraveInboxAliasesHandler::MakeMappingServiceRequest(
  const std::string& callback_id,
  const GURL& url,
  const char* method,
  const std::optional<std::string>& body,
  void (BraveInboxAliasesHandler::*httpResponseCallbackPtr)(const std::string, std::optional<std::string>)) {
  AllowJavascript();
  auto resource_request = std::make_unique<network::ResourceRequest>();
  resource_request->url = url;
  resource_request->method = method;
  resource_request->headers.SetHeader("Authorization", std::string("Bearer ") + kTempSessionKey);
  resource_request->headers.SetHeader("X-API-key", kBraveApiKey);
  simple_url_loader_ = network::SimpleURLLoader::Create(std::move(resource_request), traffic_annotation);
  if (body) {
    simple_url_loader_->AttachStringForUpload(body.value(), "application/json");
  }
  simple_url_loader_->DownloadToString(
    profile_->GetURLLoaderFactory().get(),
    base::BindOnce(
        httpResponseCallbackPtr,
        weak_factory_.GetWeakPtr(), callback_id),
        MAX_RESPONSE_LENGTH);
}

void BraveInboxAliasesHandler::SingleArgumentCommand(
  const std::string& callback_id,
  const std::string& alias_email,
  const char* method,
  void (BraveInboxAliasesHandler::*httpResponseCallbackPtr)(const std::string, std::optional<std::string>)) {
  auto bodyValue = base::Value::Dict().Set("alias", alias_email);
  auto body = base::WriteJson(bodyValue);
  if (!body) {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("json generation failed"));
    return;
  }
  MakeMappingServiceRequest(
    callback_id,
    GURL(kMappingServiceManageURL),
    method,
    body.value(),
    httpResponseCallbackPtr
  );
}

void BraveInboxAliasesHandler::GenerateAlias(const base::Value::List& args) {
  CHECK_EQ(1U, args.size());
  MakeMappingServiceRequest(
    args[0].GetString(),
    GURL(kMappingServiceGenerateURL),
    net::HttpRequestHeaders::kGetMethod,
    std::nullopt,
    &BraveInboxAliasesHandler::OnGenerateAliasResponse
  );
}

void BraveInboxAliasesHandler::OnGenerateAliasResponse(
    const std::string callback_id, std::optional<std::string> response_body) {
  if (response_body) {
    ResolveJavascriptCallback(base::Value(callback_id), response_body.value());
  } else {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("alias generation failed"));
  }
}

void BraveInboxAliasesHandler::GetAliases(const base::Value::List& args) {
  CHECK_EQ(1U, args.size());
  MakeMappingServiceRequest(
    args[0].GetString(),
    GURL(kMappingServiceManageURL + "?status=active"),
    net::HttpRequestHeaders::kGetMethod,
    std::nullopt,
    &BraveInboxAliasesHandler::OnGetAliasesResponse
  );
}

void BraveInboxAliasesHandler::OnGetAliasesResponse(
    const std::string callback_id, std::optional<std::string> response_body) {
  if (!response_body) {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("no response body"));
    return;
  }
  std::optional<base::Value> response_value = base::JSONReader::Read(response_body.value());
  if (response_value && response_value.value().is_list()) {
    auto alias_list = base::Value::List();
    for (auto& item : response_value.value().GetList()) {
      if (item.is_dict()) {
        auto& item_dict = item.GetDict();
        auto* email_value_ptr = item_dict.FindString("alias");
        auto* status_value_ptr = item_dict.FindString("status");
        alias_list.Append(
          base::Value::Dict()
            .Set("email", email_value_ptr ? *email_value_ptr : "")
            .Set("status", status_value_ptr ? *status_value_ptr : ""));
      }
    }
    ResolveJavascriptCallback(base::Value(callback_id), alias_list);
  } else {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("bad response"));
  }
}

void BraveInboxAliasesHandler::CreateAlias(const base::Value::List& args) {
  CHECK_EQ(3U, args.size());
  const auto callback_id = args[0].GetString();
  const auto alias_email = args[1].GetString();
  SingleArgumentCommand(callback_id, alias_email, net::HttpRequestHeaders::kPostMethod,
                        &BraveInboxAliasesHandler::OnCreateAliasResponse);
}

void BraveInboxAliasesHandler::OnCreateAliasResponse(
    const std::string callback_id, std::optional<std::string> response_body) {
  ResolveJavascriptCallback(base::Value(callback_id), base::Value());
}

void BraveInboxAliasesHandler::DeleteAlias(const base::Value::List& args) {
  CHECK_EQ(2U, args.size());
  const auto callback_id = args[0].GetString();
  const auto alias_email = args[1].GetString();
  SingleArgumentCommand(callback_id, alias_email, net::HttpRequestHeaders::kDeleteMethod,
                        &BraveInboxAliasesHandler::OnDeleteAliasResponse);
}

void BraveInboxAliasesHandler::OnDeleteAliasResponse(
  const std::string callback_id, std::optional<std::string> response_body) {
  ResolveJavascriptCallback(base::Value(callback_id), base::Value());
}

void BraveInboxAliasesHandler::UpdateAlias(const base::Value::List& args) {
  CHECK_EQ(4U, args.size());
    const auto callback_id = args[0].GetString();
  if (!args[1].is_string() || !args[2].is_string() || !args[3].is_bool()) {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("unexpected values"));
    return;
  }
  auto bodyValue = base::Value::Dict()
    .Set("alias", args[1].GetString())
    .Set("status", args[3].GetBool() ? "active" : "paused");
  auto body = base::WriteJson(bodyValue);
  if (!body) {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("data error"));
    return;
  }
  MakeMappingServiceRequest(
    callback_id,
    GURL(kMappingServiceManageURL),
    net::HttpRequestHeaders::kPutMethod,
    body,
    &BraveInboxAliasesHandler::OnUpdateAliasResponse
  );
}

void BraveInboxAliasesHandler::OnUpdateAliasResponse(
    const std::string callback_id, std::optional<std::string> response_body) {
  ResolveJavascriptCallback(base::Value(callback_id), base::Value());
}

void BraveInboxAliasesHandler::RequestAccount(const base::Value::List& args) {
  CHECK_EQ(2U, args.size());
  const auto callback_id = args[0].GetString();
  const auto account_email = args[1].GetString();
  AllowJavascript();
  auto resource_request = std::make_unique<network::ResourceRequest>();
  resource_request->url = GURL(kAccountsServiceRequestURL);
  resource_request->method = net::HttpRequestHeaders::kPostMethod;
  const auto bodyValue = base::Value::Dict()
    .Set("email", account_email)
    .Set("intent", "auth_token")
    .Set("service", "inbox-aliases");
  simple_url_loader_ = network::SimpleURLLoader::Create(std::move(resource_request), traffic_annotation);
  const auto body = base::WriteJson(bodyValue);
  if (body) {
    simple_url_loader_->AttachStringForUpload(body.value(), "application/json");
  }
  simple_url_loader_->DownloadToString(
    profile_->GetURLLoaderFactory().get(),
    base::BindOnce(
        &BraveInboxAliasesHandler::OnRequestAccountResponse,
        weak_factory_.GetWeakPtr(), callback_id),
        MAX_RESPONSE_LENGTH);
}

void BraveInboxAliasesHandler::OnRequestAccountResponse(
    const std::string callback_id, std::optional<std::string> response_body) {
  std::optional<base::Value> response_value = base::JSONReader::Read(response_body.value());
  if (response_value && response_value->is_dict()) {
    const auto* verification_token = response_value->GetDict().Find("verificationToken");
    if (verification_token && verification_token->is_string()) {
      verification_token_ = verification_token->GetString();
      ResolveJavascriptCallback(base::Value(callback_id), base::Value());
      return;
    }
  }
  RejectJavascriptCallback(base::Value(callback_id), base::Value("no verification token"));
}
