/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/fil_pending_tx_tracker.h"

#include <memory>
#include <utility>

#include "base/logging.h"
#include "base/synchronization/lock.h"
#include "brave/components/brave_wallet/browser/fil_nonce_tracker.h"
#include "brave/components/brave_wallet/browser/fil_tx_meta.h"
#include "brave/components/brave_wallet/browser/json_rpc_service.h"
#include "brave/components/brave_wallet/browser/tx_meta.h"
#include "brave/components/brave_wallet/common/brave_wallet.mojom.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace brave_wallet {

FilPendingTxTracker::FilPendingTxTracker(FilTxStateManager* tx_state_manager,
                                         JsonRpcService* json_rpc_service,
                                         FilNonceTracker* nonce_tracker)
    : tx_state_manager_(tx_state_manager),
      json_rpc_service_(json_rpc_service),
      nonce_tracker_(nonce_tracker),
      weak_factory_(this) {}
FilPendingTxTracker::~FilPendingTxTracker() = default;

bool FilPendingTxTracker::UpdatePendingTransactions(size_t* num_pending) {
  base::Lock* nonce_lock = nonce_tracker_->GetLock();
  if (!nonce_lock->Try())
    return false;

  auto pending_transactions = tx_state_manager_->GetTransactionsByStatus(
      mojom::TransactionStatus::Submitted, absl::nullopt);
  for (const auto& pending_transaction : pending_transactions) {
    if (IsNonceTaken(static_cast<const FilTxMeta&>(*pending_transaction))) {
      DropTransaction(pending_transaction.get());
      continue;
    }
    std::string id = pending_transaction->id();
    json_rpc_service_->GetTransactionReceipt(
        pending_transaction->tx_hash(),
        base::BindOnce(&FilPendingTxTracker::OnGetTxReceipt,
                       weak_factory_.GetWeakPtr(), std::move(id)));
  }

  nonce_lock->Release();
  *num_pending = pending_transactions.size();
  return true;
}

void FilPendingTxTracker::ResubmitPendingTransactions() {
  auto pending_transactions = tx_state_manager_->GetTransactionsByStatus(
      mojom::TransactionStatus::Submitted, absl::nullopt);
  for (const auto& pending_transaction : pending_transactions) {
    auto* pending_fil_transaction =
        static_cast<FilTxMeta*>(pending_transaction.get());
    if (!pending_fil_transaction->tx()->IsSigned()) {
      continue;
    }
    json_rpc_service_->SendRawTransaction(
        pending_fil_transaction->tx()->GetSignedTransaction(),
        base::BindOnce(&FilPendingTxTracker::OnSendRawTransaction,
                       weak_factory_.GetWeakPtr()));
  }
}

void FilPendingTxTracker::Reset() {
  network_nonce_map_.clear();
  dropped_blocks_counter_.clear();
}

void FilPendingTxTracker::OnGetTxReceipt(std::string id,
                                         TransactionReceipt receipt,
                                         mojom::ProviderError error,
                                         const std::string& error_message) {
  if (error != mojom::ProviderError::kSuccess)
    return;
  base::Lock* nonce_lock = nonce_tracker_->GetLock();
  if (!nonce_lock->Try())
    return;

  std::unique_ptr<FilTxMeta> meta = tx_state_manager_->GetFilTx(id);
  if (!meta) {
    nonce_lock->Release();
    return;
  }
  if (receipt.status) {
    meta->set_status(mojom::TransactionStatus::Confirmed);
    meta->set_confirmed_time(base::Time::Now());
    tx_state_manager_->AddOrUpdateTx(*meta);
  } else if (ShouldTxDropped(*meta)) {
    DropTransaction(meta.get());
  }

  nonce_lock->Release();
}

void FilPendingTxTracker::OnGetNetworkNonce(std::string address,
                                            uint256_t result,
                                            mojom::ProviderError error,
                                            const std::string& error_message) {
  if (error != mojom::ProviderError::kSuccess)
    return;

  network_nonce_map_[address] = result;
}

void FilPendingTxTracker::OnSendRawTransaction(
    const std::string& tx_hash,
    mojom::ProviderError error,
    const std::string& error_message) {}

bool FilPendingTxTracker::IsNonceTaken(const FilTxMeta& meta) {
  auto confirmed_transactions = tx_state_manager_->GetTransactionsByStatus(
      mojom::TransactionStatus::Confirmed, absl::nullopt);
  for (const auto& confirmed_transaction : confirmed_transactions) {
    auto* fil_confirmed_transaction =
        static_cast<FilTxMeta*>(confirmed_transaction.get());
    if (fil_confirmed_transaction->tx()->nonce() == meta.tx()->nonce() &&
        fil_confirmed_transaction->id() != meta.id())
      return true;
  }
  return false;
}

bool FilPendingTxTracker::ShouldTxDropped(const FilTxMeta& meta) {
  const std::string hex_address = meta.from();
  if (network_nonce_map_.find(hex_address) == network_nonce_map_.end()) {
    json_rpc_service_->GetTransactionCount(
        hex_address, mojom::CoinType::ETH,
        base::BindOnce(&FilPendingTxTracker::OnGetNetworkNonce,
                       weak_factory_.GetWeakPtr(), std::move(hex_address)));
  } else {
    uint256_t network_nonce = network_nonce_map_[hex_address];
    network_nonce_map_.erase(hex_address);
    if (meta.tx()->nonce() < network_nonce)
      return true;
  }

  const std::string tx_hash = meta.tx_hash();
  if (dropped_blocks_counter_.find(tx_hash) == dropped_blocks_counter_.end()) {
    dropped_blocks_counter_[tx_hash] = 0;
  }
  if (dropped_blocks_counter_[tx_hash] >= 3) {
    dropped_blocks_counter_.erase(tx_hash);
    return true;
  }

  dropped_blocks_counter_[tx_hash] = dropped_blocks_counter_[tx_hash] + 1;

  return false;
}

void FilPendingTxTracker::DropTransaction(TxMeta* meta) {
  if (!meta)
    return;
  tx_state_manager_->DeleteTx(meta->id());
}

}  // namespace brave_wallet
