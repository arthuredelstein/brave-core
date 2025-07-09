#include "brave/browser/user_agent/user_agent_observer.h"

#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/web_contents.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "third_party/blink/public/common/user_agent/user_agent_metadata.h"

namespace brave {

WEB_CONTENTS_USER_DATA_KEY_IMPL(UserAgentObserver);

void UserAgentObserver::CreateForWebContents(content::WebContents* web_contents) {
  content::WebContentsUserData<UserAgentObserver>::CreateForWebContents(web_contents);
}

UserAgentObserver::UserAgentObserver(content::WebContents* web_contents)
    : content::WebContentsUserData<UserAgentObserver>(*web_contents),
      content::WebContentsObserver(web_contents) {
  // Set up a custom UserAgentMetadata for demonstration.
  custom_metadata_.brand_version_list = {{"BraveCustom", "123"}};
  custom_metadata_.full_version = "123.0.0.0";
  custom_metadata_.platform = "CustomOS";
  custom_metadata_.platform_version = "1.0";
  custom_metadata_.architecture = "x86_64";
  custom_metadata_.model = "CustomModel";
  custom_metadata_.mobile = false;
}

void UserAgentObserver::DidStartNavigation(content::NavigationHandle* navigation_handle) {
  auto* contents_impl = static_cast<content::WebContentsImpl*>(web_contents());
  if (!contents_impl)
    return;
  std::cout << "UserAgentObserver::DidStartNavigation: " << navigation_handle->GetURL().spec() << std::endl;
  blink::UserAgentOverride override;
  override.ua_metadata_override = custom_metadata_;
  override.ua_string_override = "CustomUserAgent/123.0.0.0";
  contents_impl->SetUserAgentOverride(override, false);
}

}  // namespace brave
