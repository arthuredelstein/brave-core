/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_WEBCOMPAT_WEBCOMPAT_EXCEPTIONS_SERVICE_H_
#define BRAVE_COMPONENTS_WEBCOMPAT_WEBCOMPAT_EXCEPTIONS_SERVICE_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "base/containers/flat_set.h"
#include "base/files/file_path.h"
#include "base/values.h"
#include "brave/components/brave_component_updater/browser/local_data_files_observer.h"
#include "brave/components/brave_component_updater/browser/local_data_files_service.h"
#include "components/content_settings/core/common/content_settings_pattern.h"
#include "components/content_settings/core/common/content_settings_types.h"

namespace webcompat {

using content_settings::mojom::ContentSettingsType;

// The WebcompatExceptionsService loads a list of site-specific webcompat
// exceptions from the Brave Local Data component and provides these exceptions
// as needed.
class WebcompatExceptionsService
    : public brave_component_updater::LocalDataFilesObserver {
 public:
  explicit WebcompatExceptionsService(
      brave_component_updater::LocalDataFilesService* local_data_files_service);

  // implementation of brave_component_updater::LocalDataFilesObserver
  void OnComponentReady(const std::string& component_id,
                        const base::FilePath& install_dir,
                        const std::string& manifest) override;

  ~WebcompatExceptionsService() override;
  void OnJsonFileDataReady(const std::string& contents);
  static WebcompatExceptionsService* CreateInstance(
      brave_component_updater::LocalDataFilesService* local_data_files_service);
  static WebcompatExceptionsService* GetInstance();
  const std::vector<ContentSettingsPattern>& GetPatterns(
      ContentSettingsType webcompat_type);
  bool AddRule(const ContentSettingsPattern& pattern,
               const std::string& exception_string);

 private:
  void LoadWebcompatExceptions(const base::FilePath& install_dir);
  void AddRules(const base::Value::List& include_strings,
                const base::Value::Dict& rule_dict);
  std::map<ContentSettingsType, std::vector<ContentSettingsPattern>>
      patterns_by_webcompat_type_;
  base::WeakPtrFactory<WebcompatExceptionsService> weak_factory_{this};
};

}  // namespace webcompat

#endif  // BRAVE_COMPONENTS_WEBCOMPAT_WEBCOMPAT_EXCEPTIONS_SERVICE_H_
