// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_BROWSER_UI_INBOX_ALIASES_INBOX_ALIASES_SERVICE_H_
#define BRAVE_BROWSER_UI_INBOX_ALIASES_INBOX_ALIASES_SERVICE_H_

#include "brave/components/inbox_aliases/common/inbox_aliases.mojom.h"
#include "components/keyed_service/core/keyed_service.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote_set.h"

namespace inbox_aliases {

class InboxAliasesService : public inbox_aliases::mojom::InboxAliasesService, public KeyedService {
 public:

  InboxAliasesService();
  InboxAliasesService(const InboxAliasesService&) = delete;
  InboxAliasesService& operator=(const InboxAliasesService&) = delete;
  ~InboxAliasesService() override;

  void BindInterface(
    mojo::PendingReceiver<inbox_aliases::mojom::InboxAliasesService> pending_receiver);

  void GenerateNewAlias(
    GenerateNewAliasCallback callback) override;

  // KeyedService:
  void Shutdown() override;


 private:
  mojo::ReceiverSet<inbox_aliases::mojom::InboxAliasesService> receivers_;
};

}  // namespace inbox_aliases

#endif  // BRAVE_BROWSER_UI_INBOX_ALIASES_INBOX_ALIASES_SERVICE_H_
