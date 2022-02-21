/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_FIL_TX_MANAGER_H_
#define BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_FIL_TX_MANAGER_H_

#include <string>

#include "brave/components/brave_wallet/browser/fil_tx_state_manager.h"
#include "brave/components/brave_wallet/browser/tx_manager.h"
#include "brave/components/brave_wallet/common/brave_wallet.mojom.h"
#include "mojo/public/cpp/bindings/receiver_set.h"

class PrefService;

namespace brave_wallet {

class TxService;
class JsonRpcService;
class KeyringService;
class FilTransaction;
class FilNonceTracker;
class FilPendingTxTracker;

class FilTxManager : public TxManager {
 public:
  FilTxManager(TxService* tx_service,
               JsonRpcService* json_rpc_service,
               KeyringService* keyring_service,
               PrefService* prefs);
  ~FilTxManager() override;
  void AddUnapprovedTransaction(mojom::TxDataUnionPtr tx_data_union,
                                const std::string& from,
                                AddUnapprovedTransactionCallback) override;
  void ApproveTransaction(const std::string& tx_meta_id,
                          ApproveTransactionCallback) override;
  void RejectTransaction(const std::string& tx_meta_id,
                         RejectTransactionCallback) override;
  void GetAllTransactionInfo(const std::string& from,
                             GetAllTransactionInfoCallback) override;

  void SpeedupOrCancelTransaction(
      const std::string& tx_meta_id,
      bool cancel,
      SpeedupOrCancelTransactionCallback callback) override;
  void RetryTransaction(const std::string& tx_meta_id,
                        RetryTransactionCallback callback) override;

  void GetTransactionMessageToSign(
      const std::string& tx_meta_id,
      GetTransactionMessageToSignCallback callback) override;

  void Reset() override;

  void GetEstimatedGas(const std::string& from,
                       std::unique_ptr<FilTransaction> tx,
                       AddUnapprovedTransactionCallback callback);
  std::unique_ptr<FilTxMeta> GetTxForTesting(const std::string& tx_meta_id);

  // FilTxStateManager::Observer
  void OnNewUnapprovedTx(mojom::TransactionInfoPtr tx_info) override;
  void OnTransactionStatusChanged(mojom::TransactionInfoPtr tx_info) override;

 private:
  // TxManager
  void UpdatePendingTransactions() override;
  friend class FilTxManagerUnitTest;

  void PublishTransaction(const std::string& tx_meta_id,
                          const std::string& signed_transaction,
                          ApproveTransactionCallback callback);
  void OnPublishTransaction(std::string tx_meta_id,
                            ApproveTransactionCallback callback,
                            const std::string& tx_hash,
                            mojom::ProviderError error,
                            const std::string& error_message);
  static bool ValidateTxData(const mojom::FilTxDataPtr& tx_data,
                             std::string* error);
  void AddUnapprovedTransaction(mojom::FilTxDataPtr tx_data,
                                const std::string& from,
                                AddUnapprovedTransactionCallback callback);
  void OnGetNextNonce(std::unique_ptr<FilTxMeta> meta,
                      ApproveTransactionCallback callback,
                      bool success,
                      uint64_t nonce);
  void ContinueAddUnapprovedTransaction(
      const std::string& from,
      std::unique_ptr<FilTransaction> tx,
      AddUnapprovedTransactionCallback callback,
      const std::string& gas_premium,
      const std::string& gas_fee_cap,
      uint64_t gas_limit,
      const std::string& cid,
      mojom::ProviderError error,
      const std::string& error_message);

  std::unique_ptr<FilTxStateManager> tx_state_manager_;
  std::unique_ptr<FilNonceTracker> nonce_tracker_;
  std::unique_ptr<FilPendingTxTracker> pending_tx_tracker_;
  bool known_no_pending_tx_ = false;
  base::WeakPtrFactory<FilTxManager> weak_factory_{this};
};

}  // namespace brave_wallet

#endif  // BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_FIL_TX_MANAGER_H_
