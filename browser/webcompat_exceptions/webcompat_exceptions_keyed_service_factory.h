/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/keyed_service/content/browser_context_keyed_service_factory.h"

namespace base {
template <typename T>
class NoDestructor;
}  // namespace base

namespace webcompat_exceptions {

class WebcompatExceptionsKeyedService;

class WebcompatExceptionsKeyedServiceFactory
    : public BrowserContextKeyedServiceFactory {
 public:
  static WebcompatExceptionsKeyedServiceFactory* GetInstance();
  static KeyedService* GetServiceForContext(content::BrowserContext* context);

 private:
  friend base::NoDestructor<WebcompatExceptionsKeyedServiceFactory>;

  WebcompatExceptionsKeyedServiceFactory();
  ~WebcompatExceptionsKeyedServiceFactory() override;

  WebcompatExceptionsKeyedServiceFactory(
      const WebcompatExceptionsKeyedServiceFactory&) = delete;
  WebcompatExceptionsKeyedServiceFactory& operator=(
      const WebcompatExceptionsKeyedServiceFactory&) = delete;
};

}  // namespace webcompat_exceptions
