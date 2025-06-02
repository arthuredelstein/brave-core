/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_EMAIL_ALIASES_EMAIL_ALIASES_SERVICE_H_
#define BRAVE_COMPONENTS_EMAIL_ALIASES_EMAIL_ALIASES_SERVICE_H_

#include <string>
#include <vector>

#include "brave/components/email_aliases/email_aliases.mojom.h"

class EmailAliasesService: public mojom::EmailAliasesService {
  public:
    EmailAliasesService();
    ~EmailAliasesService();

    void GetAliases(const std::string& auth_email, GetAliasesCallback callback);
    void CreateAlias(const std::string& auth_email,
                     const std::string& alias_email,
                     const std::string& note,
                     CreateAliasCallback callback);
    void UpdateAlias(const std::string& auth_email,
                     const std::string& alias_email,
                     const std::string& note,
                     UpdateAliasCallback callback);
    void DeleteAlias(const std::string& auth_email,
                     const std::string& alias_email,
                     DeleteAliasCallback callback);
    void RequestAuthentication(const std::string& auth_email,
                              RequestAuthenticationCallback callback);
    void OnAuthStateChanged(const std::string& auth_email,
                            const mojom::AuthenticationStatus& status);
    void OnAliasesUpdated(const std::string& auth_email,
                          const std::vector<Alias>& aliases);
};

#endif  // BRAVE_COMPONENTS_EMAIL_ALIASES_EMAIL_ALIASES_SERVICE_H_
