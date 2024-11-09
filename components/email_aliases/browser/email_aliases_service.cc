// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include <iostream>

#include "brave/components/email_aliases/browser/email_aliases_service.h"

#include "mojo/public/cpp/bindings/pending_receiver.h"

#include "mojo/public/cpp/bindings/remote.h"

namespace email_aliases {

EmailAliasesService::EmailAliasesService(
    mojo::PendingReceiver<email_aliases::mojom::EmailAliasesService> receiver):
    receiver_(this, std::move(receiver)) {}

EmailAliasesService::~EmailAliasesService() = default;

void EmailAliasesService::GenerateNewAlias(
  GenerateNewAliasCallback callback) {
  std::move(callback).Run("my-stupid-email@gmail.com");
}

}  // namespace email_aliases


