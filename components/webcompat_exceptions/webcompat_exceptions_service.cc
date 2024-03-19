/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/webcompat_exceptions/webcompat_exceptions_service.h"

#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "base/containers/contains.h"
#include "base/files/file_path.h"
#include "base/json/json_reader.h"
#include "base/strings/string_split.h"
#include "base/task/thread_pool.h"
#include "brave/components/brave_component_updater/browser/dat_file_util.h"
#include "brave/components/brave_component_updater/browser/local_data_files_observer.h"

#include "base/logging.h"

#define WEBCOMPAT_EXCEPTIONS_JSON_FILE "webcompat-exceptions.json"
#define WEBCOMPAT_EXCEPTIONS_JSON_FILE_VERSION "1"

namespace webcompat_exceptions {

using brave_component_updater::LocalDataFilesObserver;
using brave_component_updater::LocalDataFilesService;

WebcompatExceptionsService::WebcompatExceptionsService(
    LocalDataFilesService* local_data_files_service)
    : LocalDataFilesObserver(local_data_files_service) {}

void WebcompatExceptionsService::LoadWebcompatExceptions(
    const base::FilePath& install_dir) {
  base::FilePath txt_file_path =
      install_dir.AppendASCII(WEBCOMPAT_EXCEPTIONS_JSON_FILE_VERSION)
          .AppendASCII(WEBCOMPAT_EXCEPTIONS_JSON_FILE);
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock()},
      base::BindOnce(&brave_component_updater::GetDATFileAsString,
                     txt_file_path),
      base::BindOnce(&WebcompatExceptionsService::OnJsonFileDataReady,
                     weak_factory_.GetWeakPtr()));
}

void WebcompatExceptionsService::OnJsonFileDataReady(
    const std::string& contents) {
  if (contents.empty()) {
    // We don't have the file yet.
    return;
  }
  DLOG(ERROR) << WEBCOMPAT_EXCEPTIONS_JSON_FILE << ":\n" << contents;

  // brave_shield_utils::DisableFeatureForWebcompat(map,
  // BraveFarblingType::kHardwareConcurrency, true, "https://browserleaks.com");
  /*
  std::vector<std::string> lines = base::SplitString(
      contents, "\n", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
  for (const auto& line : lines) {
    exceptional_domains_.insert(std::move(line));
  }
  is_ready_ = true;
  return;
  */
}

std::vector<BraveFarblingType> WebcompatExceptionsService::GetFeatureExceptions(const GURL& url) {
  /*
  if (!is_ready_) {
    // We don't have the exceptions list loaded yet.
    return false;
  }
  */
  std::vector<BraveFarblingType> exceptions;
  exceptions.push_back(webcompat_exceptions::BraveFarblingType::kHardwareConcurrency);
  exceptions.push_back(webcompat_exceptions::BraveFarblingType::kUsbDeviceSerialNumber);
  // Allow upgrade only if the domain is not on the exceptions list.
  return exceptions;
}

// implementation of LocalDataFilesObserver
void WebcompatExceptionsService::OnComponentReady(
    const std::string& component_id,
    const base::FilePath& install_dir,
    const std::string& manifest) {
  LoadWebcompatExceptions(install_dir);
}

WebcompatExceptionsService::~WebcompatExceptionsService() {
  exceptional_domains_.clear();
}

std::unique_ptr<WebcompatExceptionsService> WebcompatExceptionsServiceFactory(
    LocalDataFilesService* local_data_files_service) {
  return std::make_unique<WebcompatExceptionsService>(local_data_files_service);
}

}  // namespace webcompat_exceptions
