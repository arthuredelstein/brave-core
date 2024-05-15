/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_EPHEMERAL_STORAGE_EPHEMERAL_STORAGE_SERVICE_H_
#define BRAVE_COMPONENTS_EPHEMERAL_STORAGE_EPHEMERAL_STORAGE_SERVICE_H_

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "base/containers/flat_map.h"
#include "base/containers/flat_set.h"
#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "base/unguessable_token.h"
#include "base/values.h"
#include "brave/components/ephemeral_storage/ephemeral_storage_service_delegate.h"
#include "brave/components/ephemeral_storage/ephemeral_storage_service_observer.h"
#include "components/content_settings/core/common/content_settings_pattern.h"
#include "components/keyed_service/core/keyed_service.h"
#include "content/public/browser/storage_partition_config.h"
#include "url/gurl.h"
#include "url/origin.h"

class HostContentSettingsMap;
class PrefService;

namespace content {
class BrowserContext;
}  // namespace content

namespace webcompat_exceptions {

// Handles Ephemeral Storage cleanup/queuing and other events.
class WebcompatExceptionsKeyedService : public KeyedService {
 public:
  WebcompatExceptionsKeyedService(content::BrowserContext* context);
  ~WebcompatExceptionsKeyedService() override;

  void Shutdown() override;

  base::WeakPtr<WebcompatExceptionsKeyedService> GetWeakPtr();

  //  void AddObserver(WebcompatExceptionsKeyedServiceObserver* observer);
  //  void RemoveObserver(WebcompatExceptionsKeyedServiceObserver* observer);

 private:
  raw_ptr<content::BrowserContext> context_ = nullptr;
  raw_ptr<HostContentSettingsMap> host_content_settings_map_ = nullptr;
  raw_ptr<PrefService> prefs_ = nullptr;

  base::WeakPtrFactory<WebcompatExceptionsKeyedService> weak_ptr_factory_{this};
};

}  // namespace webcompat_exceptions

#endif  // BRAVE_COMPONENTS_EPHEMERAL_STORAGE_EPHEMERAL_STORAGE_SERVICE_H_
