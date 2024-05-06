/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/webcompat_exceptions/webcompat_exceptions_keyed_service_factory.h"

#include <memory>
#include <string>

#include "base/feature_list.h"
#include "base/no_destructor.h"
#include "brave/browser/brave_browser_process.h"
#include "brave/components/webcompat_exceptions/webcompat_exceptions_keyed_service.h"
#include "chrome/browser/profiles/profile.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "components/keyed_service/core/keyed_service.h"

namespace webcompat_exceptions {

// static
WebcompatExceptionsKeyedServiceFactory*
WebcompatExceptionsKeyedServiceFactory::GetInstance() {
  static base::NoDestructor<WebcompatExceptionsKeyedServiceFactory> instance;
  return instance.get();
}

WebcompatExceptionsKeyedServiceFactory::WebcompatExceptionsKeyedServiceFactory()
    : BrowserContextKeyedServiceFactory(
          "WebcompatExceptionsKeyedService",
          BrowserContextDependencyManager::GetInstance()) {}

WebcompatExceptionsKeyedServiceFactory::
    ~WebcompatExceptionsKeyedServiceFactory() = default;

// static
KeyedService* WebcompatExceptionsKeyedServiceFactory::GetServiceForContext(
    content::BrowserContext* context) {
  return new WebcompatExceptionsKeyedService(context);
}

}  // namespace webcompat_exceptions
