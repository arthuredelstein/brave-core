/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/settings/brave_inbox_aliases_handler.h"

#include <string>

#include "base/functional/bind.h"
#include "base/json/values_util.h"
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

const std::string kMappingServiceURL = "https://nbw40ecdu9.execute-api.us-west-2.amazonaws.com/Stage/manage";

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
      "inbox_aliases.generateNewAlias",
      base::BindRepeating(&BraveInboxAliasesHandler::GenerateNewAlias,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "inbox_aliases.getAliases",
      base::BindRepeating(&BraveInboxAliasesHandler::GetAliases,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "inbox_aliases.createAlias",
      base::BindRepeating(&BraveInboxAliasesHandler::CreateAlias,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "inbox_aliases.updateAlias",
      base::BindRepeating(&BraveInboxAliasesHandler::UpdateAlias,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "inbox_aliases.deleteAlias",
      base::BindRepeating(&BraveInboxAliasesHandler::DeleteAlias,
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
  simple_url_loader_ = network::SimpleURLLoader::Create(std::move(resource_request), traffic_annotation);
  simple_url_loader_->DownloadToString(
    profile_->GetURLLoaderFactory().get(),
    base::BindOnce(
        httpResponseCallbackPtr,
        weak_factory_.GetWeakPtr(), callback_id),
        MAX_RESPONSE_LENGTH);
}

void BraveInboxAliasesHandler::GenerateNewAlias(const base::Value::List& args) {
  AllowJavascript();
  CHECK_EQ(1U, args.size());
  const std::string& callback_id = args[0].GetString();
  auto resource_request = std::make_unique<network::ResourceRequest>();
  resource_request->url = GURL("http://localhost:3000/generate");
  resource_request->method = net::HttpRequestHeaders::kGetMethod;
  simple_url_loader_ = network::SimpleURLLoader::Create(std::move(resource_request), traffic_annotation);
  simple_url_loader_->DownloadToString(
    profile_->GetURLLoaderFactory().get(),
    base::BindOnce(
        &BraveInboxAliasesHandler::OnGenerateNewAliasResponse,
        weak_factory_.GetWeakPtr(), callback_id),
        MAX_RESPONSE_LENGTH);
}

void BraveInboxAliasesHandler::OnGenerateNewAliasResponse(
    const std::string callback_id, std::optional<std::string> response_body) {
  ResolveJavascriptCallback(base::Value(callback_id), response_body ? response_body.value() : std::string());
}

void BraveInboxAliasesHandler::GetAliases(const base::Value::List& args) {
  CHECK_EQ(1U, args.size());
  MakeMappingServiceRequest(
    args[0].GetString(),
    GURL(kMappingServiceURL),
    net::HttpRequestHeaders::kGetMethod,
    std::nullopt,
    &BraveInboxAliasesHandler::OnGetAliasesResponse
  );
}

void BraveInboxAliasesHandler::OnGetAliasesResponse(
    const std::string callback_id, std::optional<std::string> response_body) {
  ResolveJavascriptCallback(base::Value(callback_id), response_body ? response_body.value() : std::string());
}

void BraveInboxAliasesHandler::CreateAlias(const base::Value::List& args) {
  CHECK_EQ(2U, args.size());
  const std::string body = "{\"alias\":\"" + args[1].GetString() + "\"}";
  MakeMappingServiceRequest(
    args[0].GetString(),
    GURL(kMappingServiceURL),
    net::HttpRequestHeaders::kPostMethod,
    body,
    &BraveInboxAliasesHandler::OnCreateAliasResponse
  );
}

void BraveInboxAliasesHandler::OnCreateAliasResponse(
    const std::string callback_id, std::optional<std::string> response_body) {
  ResolveJavascriptCallback(base::Value(callback_id), base::Value());
}


void BraveInboxAliasesHandler::UpdateAlias(const base::Value::List& args) {
  CHECK_EQ(3U, args.size());
  const std::string body = "{\"alias\":\"" + args[1].GetString() + "\"}";
  MakeMappingServiceRequest(
    args[0].GetString(),
    GURL(kMappingServiceURL),
    net::HttpRequestHeaders::kPutMethod,
    body,
    &BraveInboxAliasesHandler::OnUpdateAliasResponse
  );
}

void BraveInboxAliasesHandler::OnUpdateAliasResponse(
    const std::string callback_id, std::optional<std::string> response_body) {
  ResolveJavascriptCallback(base::Value(callback_id), base::Value());
}

void BraveInboxAliasesHandler::DeleteAlias(const base::Value::List& args) {
  CHECK_EQ(2U, args.size());
  const std::string body = "{\"alias\":\"" + args[1].GetString() + "\"}";
  MakeMappingServiceRequest(
    args[0].GetString(),
    GURL(kMappingServiceURL),
    net::HttpRequestHeaders::kDeleteMethod,
    body,
    &BraveInboxAliasesHandler::OnDeleteAliasResponse
  );
}

void BraveInboxAliasesHandler::OnDeleteAliasResponse(
    const std::string callback_id, std::optional<std::string> response_body) {
  ResolveJavascriptCallback(base::Value(callback_id), base::Value());
}