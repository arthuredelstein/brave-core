/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/settings/brave_email_aliases_handler.h"

#include <iostream>
#include <string>

#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/values.h"
#include "brave/browser/brave_browser_process.h"
#include "brave/components/email_aliases/browser/pref_names.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_finder.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/web_ui.h"
#include "net/http/http_request_headers.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "url/gurl.h"
#include "components/prefs/scoped_user_pref_update.h"

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

BraveEmailAliasesHandler::BraveEmailAliasesHandler() = default;

BraveEmailAliasesHandler::~BraveEmailAliasesHandler() = default;

void BraveEmailAliasesHandler::RegisterMessages() {
  profile_ = Profile::FromWebUI(web_ui());
  verification_token_ = profile_->GetPrefs()->GetString(kEmailAliasesVerificationToken);

  web_ui()->RegisterMessageCallback(
      "email_aliases.generateAlias",
      base::BindRepeating(&BraveEmailAliasesHandler::GenerateAlias,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "email_aliases.getAliases",
      base::BindRepeating(&BraveEmailAliasesHandler::GetAliases,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "email_aliases.createAlias",
      base::BindRepeating(&BraveEmailAliasesHandler::CreateAlias,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "email_aliases.updateAlias",
      base::BindRepeating(&BraveEmailAliasesHandler::UpdateAlias,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "email_aliases.deleteAlias",
      base::BindRepeating(&BraveEmailAliasesHandler::DeleteAlias,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "email_aliases.requestAccount",
      base::BindRepeating(&BraveEmailAliasesHandler::RequestAccount,
                          base::Unretained(this)));
}

void BraveEmailAliasesHandler::SetNote(const std::string& alias_email, const std::string& note) {
  ScopedDictPrefUpdate update(profile_->GetPrefs(), kEmailAliasesNotes);
  update->Set(alias_email, note);
}

void BraveEmailAliasesHandler::DeleteNote(const std::string& alias_email) {
  ScopedDictPrefUpdate update(profile_->GetPrefs(), kEmailAliasesNotes);
  update->Remove(alias_email);
}

std::optional<std::string> BraveEmailAliasesHandler::GetNote(const std::string& alias_email) {
  const auto* note = profile_->GetPrefs()->GetDict(kEmailAliasesNotes).FindString(alias_email);
  return note ? std::optional<std::string>(*note) : std::nullopt;
}

void BraveEmailAliasesHandler::MakeMappingServiceURLLoader(
  const GURL& url,
  const char* method,
  const std::optional<std::string>& body,
  network::SimpleURLLoader::BodyAsStringCallback download_to_string_callback) {
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
    std::move(download_to_string_callback),
    MAX_RESPONSE_LENGTH);
}

void BraveEmailAliasesHandler::GenerateAlias(const base::Value::List& args) {
  AllowJavascript();
  CHECK_EQ(1U, args.size());
  const auto callback_id = args[0].GetString();
  MakeMappingServiceURLLoader(
    GURL(kMappingServiceGenerateURL),
    net::HttpRequestHeaders::kGetMethod,
    std::nullopt,
    base::BindOnce(
      &BraveEmailAliasesHandler::OnGenerateAliasResponse,
      weak_factory_.GetWeakPtr(), callback_id));
}

void BraveEmailAliasesHandler::OnGenerateAliasResponse(
  const std::string& callback_id, std::optional<std::string> response_body) {
  if (response_body) {
    ResolveJavascriptCallback(base::Value(callback_id), response_body.value());
  } else {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("alias generation failed"));
  }
}

void BraveEmailAliasesHandler::GetAliases(const base::Value::List& args) {
  AllowJavascript();
  CHECK_EQ(1U, args.size());
  const auto callback_id = args[0].GetString();
  MakeMappingServiceURLLoader(
    GURL(kMappingServiceManageURL + "?status=active"),
    net::HttpRequestHeaders::kGetMethod,
    std::nullopt,
    base::BindOnce(
      &BraveEmailAliasesHandler::OnGetAliasesResponse,
      weak_factory_.GetWeakPtr(), callback_id));
}

void BraveEmailAliasesHandler::OnGetAliasesResponse(
    const std::string& callback_id, std::optional<std::string> response_body) {
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
        auto note_value = email_value_ptr ? GetNote(*email_value_ptr) : std::nullopt;
        alias_list.Append(
          base::Value::Dict()
            .Set("email", email_value_ptr ? *email_value_ptr : "")
            .Set("note", note_value.value_or(""))
            .Set("status", status_value_ptr ? *status_value_ptr : ""));
      }
    }
    ResolveJavascriptCallback(base::Value(callback_id), alias_list);
  } else {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("bad response"));
  }
}

void BraveEmailAliasesHandler::CreateAlias(const base::Value::List& args) {
  AllowJavascript();
  CHECK_EQ(3U, args.size());
  const auto callback_id = args[0].GetString();
  const auto alias_email = args[1].GetString();
  const auto note = args[2].GetString();
  auto bodyValue = base::Value::Dict()
    .Set("alias", alias_email);
  auto body = base::WriteJson(bodyValue);
  if (!body) {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("json generation failed"));
    return;
  }
  std::cout << "body: " << body.value() << std::endl;
  MakeMappingServiceURLLoader(
    GURL(kMappingServiceManageURL),
    net::HttpRequestHeaders::kPostMethod,
    body.value(),
    base::BindOnce(
      &BraveEmailAliasesHandler::OnCreateAliasResponse,
      weak_factory_.GetWeakPtr(), callback_id, alias_email, note));
}

void BraveEmailAliasesHandler::OnCreateAliasResponse(
    const std::string& callback_id,
    const std::string& alias_email,
    const std::string& note,
    std::optional<std::string> response_body) {
  SetNote(alias_email, note);
  ResolveJavascriptCallback(base::Value(callback_id), base::Value());
}

void BraveEmailAliasesHandler::DeleteAlias(const base::Value::List& args) {
  AllowJavascript();
  CHECK_EQ(2U, args.size());
  const auto callback_id = args[0].GetString();
  const auto alias_email = args[1].GetString();
  auto bodyValue = base::Value::Dict()
    .Set("alias", alias_email);
  auto body = base::WriteJson(bodyValue);
  if (!body) {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("json generation failed"));
    return;
  }
  MakeMappingServiceURLLoader(
    GURL(kMappingServiceManageURL),
    net::HttpRequestHeaders::kDeleteMethod,
    body,
    base::BindOnce(
      &BraveEmailAliasesHandler::OnDeleteAliasResponse,
      weak_factory_.GetWeakPtr(), callback_id, alias_email));
}

void BraveEmailAliasesHandler::OnDeleteAliasResponse(
  const std::string& callback_id,
  const std::string& alias_email,
  std::optional<std::string> response_body) {
  DeleteNote(alias_email);
  ResolveJavascriptCallback(base::Value(callback_id), base::Value());
}

void BraveEmailAliasesHandler::UpdateAlias(const base::Value::List& args) {
  AllowJavascript();
  CHECK_EQ(4U, args.size());
  const auto callback_id = args[0].GetString();
  if (!args[1].is_string() || !args[2].is_string() || !args[3].is_bool()) {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("unexpected values"));
    return;
  }
  const auto alias_email = args[1].GetString();
  const auto note = args[2].GetString();
  const std::string status = args[3].GetBool() ? "active" : "paused";
  auto bodyValue = base::Value::Dict()
    .Set("alias", alias_email)
    .Set("status", status);
  auto body = base::WriteJson(bodyValue);
  if (!body) {
    RejectJavascriptCallback(base::Value(callback_id), base::Value("data error"));
    return;
  }
  MakeMappingServiceURLLoader(
    GURL(kMappingServiceManageURL),
    net::HttpRequestHeaders::kPutMethod,
    body,
    base::BindOnce(
      &BraveEmailAliasesHandler::OnUpdateAliasResponse,
      weak_factory_.GetWeakPtr(), callback_id, alias_email, note));
}

void BraveEmailAliasesHandler::OnUpdateAliasResponse(
    const std::string& callback_id,
    const std::string& alias_email,
    const std::string& note,
    std::optional<std::string> response_body) {
  SetNote(alias_email, note);
  ResolveJavascriptCallback(base::Value(callback_id), base::Value());
}

void BraveEmailAliasesHandler::RequestAccount(const base::Value::List& args) {
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
        &BraveEmailAliasesHandler::OnRequestAccountResponse,
        weak_factory_.GetWeakPtr(), callback_id),
        MAX_RESPONSE_LENGTH);
}

void BraveEmailAliasesHandler::OnRequestAccountResponse(
    const std::string& callback_id, std::optional<std::string> response_body) {
  std::optional<base::Value> response_value = base::JSONReader::Read(response_body.value());
  if (response_value && response_value->is_dict()) {
    const auto* verification_token = response_value->GetDict().Find("verificationToken");
    if (verification_token && verification_token->is_string()) {
      // Store the verification token while we wait for confirmation.
      verification_token_ = verification_token->GetString();
      profile_->GetPrefs()->SetString(kEmailAliasesVerificationToken, verification_token_);
      // Acknowledge success to the caller.
      ResolveJavascriptCallback(base::Value(callback_id), base::Value());
      return;
    }
  }
  RejectJavascriptCallback(base::Value(callback_id), base::Value("no verification token"));
}
