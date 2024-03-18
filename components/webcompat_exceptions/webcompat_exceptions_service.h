/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_WEBCOMPAT_EXCEPTIONS_BROWSER_WEBCOMPAT_EXCEPTIONS_SERVICE_H_
#define BRAVE_COMPONENTS_WEBCOMPAT_EXCEPTIONS_BROWSER_WEBCOMPAT_EXCEPTIONS_SERVICE_H_

#include <memory>
#include <set>
#include <string>

#include "base/files/file_path.h"
#include "brave/components/brave_component_updater/browser/local_data_files_observer.h"
#include "brave/components/brave_component_updater/browser/local_data_files_service.h"
#include "brave/components/webcompat_exceptions/common/webcompat_exceptions.mojom.h"
#include "brave/components/webcompat_exceptions/webcompat_constants.h"

namespace webcompat_exceptions {

class WebcompatExceptionsService
    : public brave_component_updater::LocalDataFilesObserver {
 public:
  explicit WebcompatExceptionsService(
      brave_component_updater::LocalDataFilesService* local_data_files_service);

  // implementation of brave_component_updater::LocalDataFilesObserver
  void OnComponentReady(const std::string& component_id,
                        const base::FilePath& install_dir,
                        const std::string& manifest) override;

  bool IsFeatureDisabled(const GURL& url, BraveFarblingType farbling_type);
  ~WebcompatExceptionsService() override;
  void SetIsReadyForTesting() { is_ready_ = true; }
  void OnJsonFileDataReady(const std::string& contents);

 private:
  void LoadWebcompatExceptions(const base::FilePath& install_dir);
  std::set<std::string> exceptional_domains_;
  bool is_ready_ = false;
  base::WeakPtrFactory<WebcompatExceptionsService> weak_factory_{this};
};

// Creates the WebcompatExceptionsService
std::unique_ptr<WebcompatExceptionsService> WebcompatExceptionsServiceFactory(
    brave_component_updater::LocalDataFilesService* local_data_files_service);

}  // namespace webcompat_exceptions

#endif  // BRAVE_COMPONENTS_WEBCOMPAT_EXCEPTIONS_BROWSER_WEBCOMPAT_EXCEPTIONS_SERVICE_H_
