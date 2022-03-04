/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>

#include "brave/components/brave_wallet/browser/eth_nonce_tracker.h"
#include "brave/components/brave_wallet/browser/eth_tx_meta.h"
#include "brave/components/brave_wallet/browser/json_rpc_service.h"
#include "brave/components/brave_wallet/browser/nonce_tracker.h"
#include "brave/components/brave_wallet/common/brave_wallet.mojom-shared.h"
#include "brave/components/brave_wallet/common/eth_address.h"

namespace brave_wallet {

EthNonceTracker::EthNonceTracker(TxStateManager* tx_state_manager,
                                 JsonRpcService* json_rpc_service)
    : NonceTracker(tx_state_manager, json_rpc_service), weak_factory_(this) {}
EthNonceTracker::~EthNonceTracker() = default;

void EthNonceTracker::GetNextNonce(const std::string& from,
                                   GetNextNonceCallback callback) {
  GetJsonRpcService()->GetEthTransactionCount(
      from,
      base::BindOnce(&NonceTracker::OnGetNetworkNonce,
                     weak_factory_.GetWeakPtr(), from, std::move(callback)));
}

std::string EthNonceTracker::GetChecksumAddress(const std::string& address) {
  return EthAddress::FromHex(address).ToChecksumAddress();
}

uint256_t EthNonceTracker::GetHighestLocallyConfirmed(
    const std::vector<std::unique_ptr<TxMeta>>& metas) {
  uint256_t highest = 0;
  for (auto& meta : metas) {
    auto* tx_meta = static_cast<EthTxMeta*>(meta.get());
    DCHECK(
        tx_meta->tx()->nonce());  // Not supposed to happen for a confirmed tx.
    highest = std::max(highest, tx_meta->tx()->nonce().value() + (uint256_t)1);
  }
  return highest;
}

uint256_t EthNonceTracker::GetHighestContinuousFrom(
    const std::vector<std::unique_ptr<TxMeta>>& metas,
    uint256_t start) {
  uint256_t highest = start;
  for (auto& meta : metas) {
    auto* eth_meta = static_cast<EthTxMeta*>(meta.get());
    DCHECK(
        eth_meta->tx()->nonce());  // Not supposed to happen for a submitted tx.
    if (eth_meta->tx()->nonce().value() == highest)
      highest++;
  }
  return highest;
}

}  // namespace brave_wallet
