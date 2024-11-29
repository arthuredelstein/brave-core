// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_BROWSER_UI_INBOX_ALIASES_INBOX_ALIASES_SERVICE_FACTORY_H_
#define BRAVE_BROWSER_UI_INBOX_ALIASES_INBOX_ALIASES_SERVICE_FACTORY_H_

#include <memory>

#include "base/no_destructor.h"
#include "brave/browser/ui/inbox_aliases/inbox_aliases_service.h"
#include "chrome/browser/profiles/profile_keyed_service_factory.h"
#include "components/keyed_service/core/keyed_service.h"
#include "content/public/browser/browser_context.h"

namespace base {
template <typename T>
class NoDestructor;
}  // namespace base

namespace inbox_aliases {

class InboxAliasesServiceFactory : public ProfileKeyedServiceFactory {
 public:
  static InboxAliasesService* GetForContext(content::BrowserContext* context);
  static InboxAliasesServiceFactory* GetInstance();

  InboxAliasesServiceFactory(const InboxAliasesServiceFactory&) = delete;
  InboxAliasesServiceFactory& operator=(const InboxAliasesServiceFactory&) =
      delete;

 private:
  friend base::NoDestructor<InboxAliasesServiceFactory>;

  InboxAliasesServiceFactory();
  ~InboxAliasesServiceFactory() override;

  std::unique_ptr<KeyedService> BuildServiceInstanceForBrowserContext(
      content::BrowserContext* context) const override;
};

}  // namespace inbox_aliases

#endif  // BRAVE_BROWSER_UI_INBOX_ALIASES_INBOX_ALIASES_SERVICE_FACTORY_H_
