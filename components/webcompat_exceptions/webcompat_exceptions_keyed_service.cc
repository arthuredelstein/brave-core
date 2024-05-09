/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/webcompat_exceptions/webcompat_exceptions_keyed_service.h"

#include <memory>
#include <utility>
#include <vector>

#include "base/strings/strcat.h"
#include "base/task/sequenced_task_runner.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "brave/components/brave_shields/content/browser/brave_shields_util.h"
#include "brave/components/ephemeral_storage/ephemeral_storage_pref_names.h"
#include "brave/components/ephemeral_storage/url_storage_checker.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "components/content_settings/core/browser/content_settings_observable_provider.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "components/prefs/scoped_user_pref_update.h"
#include "components/user_prefs/user_prefs.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/site_instance.h"
#include "content/public/browser/storage_partition_config.h"
#include "net/base/features.h"
#include "net/base/schemeful_site.h"
#include "net/base/url_util.h"
#include "url/origin.h"
#include "url/url_constants.h"

namespace webcompat_exceptions {

WebcompatExceptionsKeyedService::WebcompatExceptionsKeyedService(
    content::BrowserContext* context)
    : context_(context) {
  DCHECK(context_);
  auto* map = HostContentSettingsMapFactory::GetForProfile(context);
  brave_shields::SetFingerprintingControlType(
      map, brave_shields::ControlType::ALLOW, GURL("https://brave.com"),
      g_browser_process->local_state(),
      Profile::FromBrowserContext(context)->GetPrefs());
}

WebcompatExceptionsKeyedService::~WebcompatExceptionsKeyedService() = default;

void WebcompatExceptionsKeyedService::Shutdown() {
  weak_ptr_factory_.InvalidateWeakPtrs();
}

base::WeakPtr<WebcompatExceptionsKeyedService>
WebcompatExceptionsKeyedService::GetWeakPtr() {
  return weak_ptr_factory_.GetWeakPtr();
}

}  // namespace webcompat_exceptions
