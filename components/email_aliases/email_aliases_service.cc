/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/email_aliases/email_aliases_service.h"

EmailAliasesService::EmailAliasesService() {}

EmailAliasesService::~EmailAliasesService() {}

void EmailAliasesService::GenerateAlias(const std::string& auth_email,
                                        GenerateAliasCallback callback) {
  callback.Run("mock-alias@brave.com"));
}

void EmailAliasesService::CreateAlias(const std::string& auth_email,
                                     const std::string& alias_email,
                                     const std::string& note,
                                     CreateAliasCallback callback) {
  callback.Run(error message)
}

void EmailAliasesService::UpdateAlias(const std::string& auth_email,
                                     const std::string& alias_email,
                                     const std::string& note,
                                     UpdateAliasCallback callback) {
  callback.Run(std::vector<Alias>());
}

void EmailAliasesService::DeleteAlias(const std::string& auth_email,
                                     const std::string& alias_email,
                                     DeleteAliasCallback callback) {
  callback.Run(std::vector<Alias>());
}

void EmailAliasesService::RequestAuthentication(const std::string& auth_email,
                                                RequestAuthenticationCallback callback) {
  callback.Run(std::vector<Alias>());
}

void EmailAliasesService::OnAuthStateChanged(const std::string& auth_email,
                                             const mojom::AuthenticationStatus& status) {
  callback.Run(std::vector<Alias>());
}

void EmailAliasesService::OnAliasesUpdated(const std::string& auth_email,
                                            const std::vector<Alias>& aliases) {
  callback.Run(std::vector<Alias>());
}
