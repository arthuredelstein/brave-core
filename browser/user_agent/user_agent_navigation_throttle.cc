#include "brave/browser/user_agent/user_agent_navigation_throttle.h"

#include <iostream>
#include "content/public/browser/navigation_handle.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "third_party/blink/public/common/user_agent/user_agent_metadata.h"


namespace brave {

UserAgentNavigationThrottle::UserAgentNavigationThrottle(content::NavigationThrottleRegistry& registry)
    : content::NavigationThrottle(registry) {}

UserAgentNavigationThrottle::~UserAgentNavigationThrottle() = default;

content::NavigationThrottle::ThrottleCheckResult
UserAgentNavigationThrottle::WillStartRequest() {
  std::cout << "UserAgentNavigationThrottle::WillStartRequest: "
            << navigation_handle()->GetURL().spec() << std::endl;
  auto* contents = navigation_handle()->GetWebContents();
  auto* contents_impl = static_cast<content::WebContentsImpl*>(contents);
  if (!contents_impl)
    return PROCEED;

  blink::UserAgentOverride ua_override;
  blink::UserAgentMetadata custom_metadata;
  custom_metadata.brand_version_list = {{"BraveThrottle", "456"}};
  custom_metadata.full_version = "456.0.0.0";
  custom_metadata.platform = "ThrottleOS";
  custom_metadata.platform_version = "2.0";
  custom_metadata.architecture = "arm64";
  custom_metadata.model = "ThrottleModel";
  custom_metadata.mobile = false;
  ua_override.ua_metadata_override = custom_metadata;
  ua_override.ua_string_override = "BraveThrottle/456.0.0.0";

  contents_impl->SetUserAgentOverride(ua_override, /*override_in_new_tabs=*/false);
  std::cout << "UserAgentNavigationThrottle::WillStartRequest: set ua" << std::endl;
  return PROCEED;
}

const char* UserAgentNavigationThrottle::GetNameForLogging() {
  return "UserAgentNavigationThrottle";
}

void UserAgentNavigationThrottle::MaybeCreateAndAdd(content::NavigationThrottleRegistry& registry) {
  registry.AddThrottle(std::make_unique<UserAgentNavigationThrottle>(registry));
}

}  // namespace brave
