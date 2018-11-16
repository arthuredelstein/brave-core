/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/search_engines/search_engine_provider_service_factory.h"

#include "brave/browser/search_engines/guest_window_search_engine_provider_service.h"
#include "brave/browser/search_engines/private_window_search_engine_provider_service.h"
#include "brave/browser/search_engines/search_engine_provider_util.h"
#include "brave/browser/search_engines/tor_window_search_engine_provider_service.h"
#include "chrome/browser/profiles/incognito_helpers.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/search_engines/template_url_service_factory.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"

namespace {
// Factory owns service object.
KeyedService* InitializeSearchEngineProviderServiceIfNeeded(Profile* profile) {
  // In non qwant region, controller is also needed for private profile.
  // We uses separate TemplateURLService for normal and off the recored profile.
  // That means changing normal profile's provider doesn't affect otr profile's.
  // This controller monitor's normal profile's service and apply it's change to
  // otr profile to use same provider.
  // Private profile's setting is shared with normal profile's setting.
  if (profile->GetProfileType() == Profile::INCOGNITO_PROFILE) {
    return new PrivateWindowSearchEngineProviderService(profile);
  }

  // Regardless of qwant region, tor profile needs controller to store
  // previously set search engine provider.
  if (profile->IsTorProfile() &&
      profile->GetProfileType() == Profile::GUEST_PROFILE) {
    return new TorWindowSearchEngineProviderService(profile);
  }

  // Guest profile in qwant region doesn't need special handling of search
  // engine provider because its newtab doesn't have ddg toggle button.
  if (brave::IsRegionForQwant(profile))
    return nullptr;

  if (profile->GetProfileType() == Profile::GUEST_PROFILE) {
    return new GuestWindowSearchEngineProviderService(profile);
  }

  return nullptr;
}

}  // namespace

// static
SearchEngineProviderServiceFactory*
SearchEngineProviderServiceFactory::GetInstance() {
  return base::Singleton<SearchEngineProviderServiceFactory>::get();
}

SearchEngineProviderServiceFactory::SearchEngineProviderServiceFactory()
    : BrowserContextKeyedServiceFactory(
          "SearchEngineProviderService",
          BrowserContextDependencyManager::GetInstance()) {
  DependsOn(TemplateURLServiceFactory::GetInstance());
}

SearchEngineProviderServiceFactory::~SearchEngineProviderServiceFactory() {}

KeyedService* SearchEngineProviderServiceFactory::BuildServiceInstanceFor(
    content::BrowserContext* context) const {
  return InitializeSearchEngineProviderServiceIfNeeded(
      Profile::FromBrowserContext(context));
}

content::BrowserContext*
SearchEngineProviderServiceFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return chrome::GetBrowserContextOwnInstanceInIncognito(context);
}

bool SearchEngineProviderServiceFactory::ServiceIsNULLWhileTesting() const {
  return true;
}

bool
SearchEngineProviderServiceFactory::ServiceIsCreatedWithBrowserContext() const {
  // Service should be initialized when profile is created to set proper
  // provider to TemplateURLService.
  return true;
}
