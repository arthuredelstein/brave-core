// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/email_aliases/common/email_aliases.mojom.h"
#include "mojo/public/cpp/bindings/receiver.h"

namespace email_aliases {

class EmailAliasesService : email_aliases::mojom::EmailAliasesService {
public:
  explicit EmailAliasesService(
    mojo::PendingReceiver<email_aliases::mojom::EmailAliasesService> receiver);
  ~EmailAliasesService() override;
  EmailAliasesService(const EmailAliasesService&) = delete;
  EmailAliasesService& operator=(const EmailAliasesService&) = delete;

  void GenerateNewAlias(
    GenerateNewAliasCallback callback) override;

private:
  mojo::Receiver<email_aliases::mojom::EmailAliasesService> receiver_;

};



}  // namespace email_aliases