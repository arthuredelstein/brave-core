/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/fil_nonce_tracker.h"
#include <stdint.h>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "base/bind.h"
#include "brave/components/brave_wallet/browser/fil_transaction.h"
#include "brave/components/brave_wallet/browser/fil_tx_meta.h"
#include "brave/components/brave_wallet/browser/fil_tx_state_manager.h"
#include "brave/components/brave_wallet/browser/json_rpc_service.h"
#include "brave/components/brave_wallet/browser/tx_meta.h"
#include "brave/components/brave_wallet/common/brave_wallet.mojom.h"
#include "brave/components/brave_wallet/common/fil_address.h"

namespace brave_wallet {

namespace {

uint64_t GetHighestLocallyConfirmed(
    const std::vector<std::unique_ptr<TxMeta>>& metas) {
  uint64_t highest = 0;
  for (auto& meta : metas) {
    auto* fil_meta = static_cast<FilTxMeta*>(meta.get());
    DCHECK(
        fil_meta->tx()->nonce());  // Not supposed to happen for a confirmed tx.
    highest = std::max(highest, fil_meta->tx()->nonce().value() + (uint64_t)1);
  }
  return highest;
}

uint64_t GetHighestContinuousFrom(
    const std::vector<std::unique_ptr<TxMeta>>& metas,
    uint64_t start) {
  uint64_t highest = start;
  for (auto& meta : metas) {
    auto* fil_meta = static_cast<FilTxMeta*>(meta.get());
    DCHECK(
        fil_meta->tx()->nonce());  // Not supposed to happen for a submitted tx.
    if (fil_meta->tx()->nonce().value() == highest)
      highest++;
  }
  return highest;
}

}  // namespace

FilNonceTracker::FilNonceTracker(FilTxStateManager* tx_state_manager,
                                 JsonRpcService* json_rpc_service)
    : tx_state_manager_(tx_state_manager),
      json_rpc_service_(json_rpc_service),
      weak_factory_(this) {}
FilNonceTracker::~FilNonceTracker() = default;

void FilNonceTracker::GetNextNonce(const FilAddress& from,
                                   GetNextNonceCallback callback) {
  const std::string address = from.ToChecksumAddress();
  json_rpc_service_->GetTransactionCount(
      address, mojom::CoinType::FIL,
      base::BindOnce(&FilNonceTracker::OnGetNetworkNonce,
                     weak_factory_.GetWeakPtr(), FilAddress(from),
                     std::move(callback)));
}

void FilNonceTracker::OnGetNetworkNonce(FilAddress from,
                                        GetNextNonceCallback callback,
                                        uint256_t network_nonce,
                                        mojom::ProviderError error,
                                        const std::string& error_message) {
  // Filecoin nonce is expected to be uint64
  // https://github.com/filecoin-project/lotus/blob/master/chain/types/message.go#L36
  if (network_nonce > UINT64_MAX) {
    std::move(callback).Run(false, network_nonce);
    return;
  }

  if (!nonce_lock_.Try()) {
    std::move(callback).Run(false, network_nonce);
    return;
  }
  uint64_t network_fil_nonce = static_cast<uint64_t>(network_nonce);
  auto confirmed_transactions = tx_state_manager_->GetTransactionsByStatus(
      mojom::TransactionStatus::Confirmed, from.ToChecksumAddress());
  uint64_t local_highest = GetHighestLocallyConfirmed(confirmed_transactions);

  uint64_t highest_confirmed = std::max(network_fil_nonce, local_highest);

  auto pending_transactions = tx_state_manager_->GetTransactionsByStatus(
      mojom::TransactionStatus::Submitted, from.ToChecksumAddress());

  uint64_t highest_continuous_from =
      GetHighestContinuousFrom(pending_transactions, highest_confirmed);

  uint64_t nonce = std::max(network_fil_nonce, highest_continuous_from);

  nonce_lock_.Release();
  std::move(callback).Run(true, nonce);
}

}  // namespace brave_wallet
