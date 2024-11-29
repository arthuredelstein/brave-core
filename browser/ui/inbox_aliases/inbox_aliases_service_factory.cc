// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/inbox_aliases/inbox_aliases_service_factory.h"

#include "base/logging.h"

#include <memory>
#include <utility>

#include "base/no_destructor.h"
#include "brave/browser/ui/brave_browser_window.h"
#include "brave/browser/ui/inbox_aliases/inbox_aliases_service.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_keyed_service_factory.h"
#include "chrome/browser/profiles/profile_selections.h"
#include "components/keyed_service/core/keyed_service.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "content/public/browser/browser_context.h"

namespace inbox_aliases {

// static
InboxAliasesServiceFactory* InboxAliasesServiceFactory::GetInstance() {
  static base::NoDestructor<InboxAliasesServiceFactory> instance;
  return instance.get();
}

// static
InboxAliasesService* InboxAliasesServiceFactory::GetForContext(
    content::BrowserContext* context) {
  base::debug::StackTrace().Print();
  return static_cast<InboxAliasesService*>(
      GetInstance()->GetServiceForBrowserContext(context, true));
}

InboxAliasesServiceFactory::InboxAliasesServiceFactory()
    : ProfileKeyedServiceFactory(
          "InboxAliasesServiceFactory",
          ProfileSelections::Builder()
              .WithRegular(ProfileSelection::kRedirectedToOriginal)
              .Build()) {}

InboxAliasesServiceFactory::~InboxAliasesServiceFactory() = default;

std::unique_ptr<KeyedService>
InboxAliasesServiceFactory::BuildServiceInstanceForBrowserContext(
    content::BrowserContext* context) const {
  auto* profile = Profile::FromBrowserContext(context);
  DCHECK(profile);
  return std::make_unique<InboxAliasesService>();
}

}  // namespace inbox_aliases
