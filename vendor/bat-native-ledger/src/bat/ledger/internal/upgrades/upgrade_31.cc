/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/upgrades/upgrade_31.h"

#include "bat/ledger/internal/upgrades/migration_job.h"

namespace ledger {

namespace {

const char kSQL[] = R"sql(
  ALTER TABLE pending_contribution ADD processor INTEGER DEFAULT 0 NOT NULL;
)sql";

}  // namespace

constexpr int Upgrade31::kVersion;

void Upgrade31::Start() {
  CompleteWithFuture(context().StartJob<MigrationJob>(kVersion, kSQL));
}

}  // namespace ledger
