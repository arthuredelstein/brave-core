/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/webcompat/webcompat_exceptions_service.h"

#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "base/containers/contains.h"
#include "base/containers/fixed_flat_map.h"
#include "base/files/file_path.h"
#include "base/json/json_reader.h"
#include "base/logging.h"
#include "base/strings/string_split.h"
#include "base/task/thread_pool.h"
#include "brave/components/brave_component_updater/browser/dat_file_util.h"
#include "brave/components/brave_component_updater/browser/local_data_files_observer.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_types.h"

#define WEBCOMPAT_EXCEPTIONS_JSON_FILE "webcompat-exceptions.json"
#define WEBCOMPAT_EXCEPTIONS_JSON_FILE_VERSION "1"

namespace webcompat {

using brave_component_updater::LocalDataFilesObserver;
using brave_component_updater::LocalDataFilesService;
using content_settings::RuleMetaData;
using content_settings::mojom::ContentSettingsType;

namespace {

// WebcompatExceptionService keys
const char kInclude[] = "include";
const char kExceptions[] = "exceptions";

using enum ContentSettingsType;

constexpr std::vector<ContentSettingsPattern> kEmptyPatternVector;

constexpr auto kWebcompatNamesToType =
    base::MakeFixedFlatMap<std::string_view, ContentSettingsType>({
        {"audio", BRAVE_WEBCOMPAT_AUDIO},
        {"canvas", BRAVE_WEBCOMPAT_CANVAS},
        {"device-memory", BRAVE_WEBCOMPAT_DEVICE_MEMORY},
        {"eventsource-pool", BRAVE_WEBCOMPAT_EVENT_SOURCE_POOL},
        {"font", BRAVE_WEBCOMPAT_FONT},
        {"hardware-concurrency", BRAVE_WEBCOMPAT_HARDWARE_CONCURRENCY},
        {"keyboard", BRAVE_WEBCOMPAT_KEYBOARD},
        {"language", BRAVE_WEBCOMPAT_LANGUAGE},
        {"media-devices", BRAVE_WEBCOMPAT_MEDIA_DEVICES},
        {"plugins", BRAVE_WEBCOMPAT_PLUGINS},
        {"screen", BRAVE_WEBCOMPAT_SCREEN},
        {"speech-synthesis", BRAVE_WEBCOMPAT_SPEECH_SYNTHESIS},
        {"usb-device-serial-number", BRAVE_WEBCOMPAT_USB_DEVICE_SERIAL_NUMBER},
        {"user-agent", BRAVE_WEBCOMPAT_USER_AGENT},
        {"webgl", BRAVE_WEBCOMPAT_WEBGL},
        {"webgl2", BRAVE_WEBCOMPAT_WEBGL2},
        {"websockets-pool", BRAVE_WEBCOMPAT_WEB_SOCKETS_POOL},
    });

WebcompatExceptionsService* singleton = nullptr;

}  // namespace

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

bool WebcompatExceptionsService::AddRule(const ContentSettingsPattern& pattern,
                                         const std::string& exception_string) {
  const auto it = kWebcompatNamesToType.find(exception_string);
  if (it != kWebcompatNamesToType.end()) {
    const auto webcompat_type = it->second;
    if (!patterns_by_webcompat_type_.contains(webcompat_type)) {
      patterns_by_webcompat_type_[webcompat_type] = kEmptyPatternVector;
    }
    patterns_by_webcompat_type_[webcompat_type].push_back(pattern);
    return true;
  } else {
    return false;
  }
}

void WebcompatExceptionsService::AddRules(
    const base::Value::List& include_strings,
    const base::Value::Dict& rule_dict) {
  const base::Value* exceptions = rule_dict.Find(kExceptions);
  if (exceptions->is_list()) {
    for (const base::Value& include_string : include_strings) {
      const auto pattern =
          ContentSettingsPattern::FromString(include_string.GetString());
      for (const base::Value& exception : exceptions->GetList()) {
        const bool success = AddRule(pattern, exception.GetString());
        if (!success) {
          DLOG(ERROR) << "Unrecognized webcompat exception "
                      << exception.GetString();
        }
      }
    }
  } else {
    DLOG(ERROR) << "Malformed exceptions list in "
                << WEBCOMPAT_EXCEPTIONS_JSON_FILE;
  }
}

const std::vector<ContentSettingsPattern>&
WebcompatExceptionsService::GetPatterns(ContentSettingsType webcompat_type) {
  if (patterns_by_webcompat_type_.contains(webcompat_type)) {
    return patterns_by_webcompat_type_.at(webcompat_type);
  } else {
    return kEmptyPatternVector;
  }
}

void WebcompatExceptionsService::OnJsonFileDataReady(
    const std::string& contents) {
  if (contents.empty()) {
    // We don't have the file yet.
    return;
  }
  const auto json_root = base::JSONReader::Read(contents);
  if (json_root == std::nullopt) {
    DLOG(ERROR) << "Failed to parse " << WEBCOMPAT_EXCEPTIONS_JSON_FILE;
    return;
  }
  if (!json_root->is_list()) {
    DLOG(ERROR) << "Didn't find expected list in "
                << WEBCOMPAT_EXCEPTIONS_JSON_FILE;
    return;
  }
  for (const auto& rule : json_root->GetList()) {
    if (!rule.is_dict()) {
      // Something is wrong with the rule definition; skip it.
      DLOG(ERROR) << "Found a malformed rule in "
                  << WEBCOMPAT_EXCEPTIONS_JSON_FILE;
      continue;
    }
    const auto& rule_dict = rule.GetDict();
    const auto* include = rule_dict.Find(kInclude);
    if (include->is_list()) {
      AddRules(include->GetList(), rule_dict);
    } else if (include->is_string()) {
      DLOG(ERROR) << "Not implemented yet";
    } else {
      DLOG(ERROR) << "Malformed include attribute in "
                  << WEBCOMPAT_EXCEPTIONS_JSON_FILE;
    }
  }
}

// implementation of LocalDataFilesObserver
void WebcompatExceptionsService::OnComponentReady(
    const std::string& component_id,
    const base::FilePath& install_dir,
    const std::string& manifest) {
  LoadWebcompatExceptions(install_dir);
}

WebcompatExceptionsService::~WebcompatExceptionsService() {}

// static
WebcompatExceptionsService* WebcompatExceptionsService::CreateInstance(
    LocalDataFilesService* local_data_files_service) {
  singleton = new WebcompatExceptionsService(local_data_files_service);
  return singleton;
}

// static
WebcompatExceptionsService* WebcompatExceptionsService::GetInstance() {
  return singleton;
}

}  // namespace webcompat
