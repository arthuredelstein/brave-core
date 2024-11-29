// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include <iostream>

#include "brave/browser/ui/inbox_aliases/inbox_aliases_service.h"

#include "brave/app/command_utils.h"
#include "brave/components/inbox_aliases/common/inbox_aliases.mojom-forward.h"
#include "brave/components/inbox_aliases/common/inbox_aliases.mojom.h"
#include "components/prefs/pref_service.h"
#include "mojo/public/cpp/bindings/pending_remote.h"

namespace inbox_aliases {

InboxAliasesService::InboxAliasesService() {

  std::cout << "InboxAliasesService instantiated" << std::endl;

}

InboxAliasesService::~InboxAliasesService() = default;

void InboxAliasesService::BindInterface(
    mojo::PendingReceiver<inbox_aliases::mojom::InboxAliasesService> pending_receiver) {
  receivers_.Add(this, std::move(pending_receiver));
}

void InboxAliasesService::GenerateNewAlias(
  GenerateNewAliasCallback callback) {
  std::move(callback).Run("my-horrible-email-address@gmail.com");
}

void InboxAliasesService::Shutdown() {
  receivers_.Clear();
}

}  // namespace inbox_aliases
