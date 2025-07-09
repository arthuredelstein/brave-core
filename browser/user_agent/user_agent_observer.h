#ifndef BRAVE_BROWSER_USER_AGENT_USER_AGENT_OBSERVER_H_
#define BRAVE_BROWSER_USER_AGENT_USER_AGENT_OBSERVER_H_

#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents.h"
#include "third_party/blink/public/common/user_agent/user_agent_metadata.h"
#include "content/public/browser/web_contents_user_data.h"

namespace brave {

class UserAgentObserver : public content::WebContentsUserData<UserAgentObserver>, public content::WebContentsObserver {
 public:
  static void CreateForWebContents(content::WebContents* web_contents);

  // content::WebContentsObserver
  void DidStartNavigation(content::NavigationHandle* navigation_handle) override;

 private:
  friend class content::WebContentsUserData<UserAgentObserver>;
  explicit UserAgentObserver(content::WebContents* web_contents);
  blink::UserAgentMetadata custom_metadata_;
  FRIEND_TEST_ALL_PREFIXES(UserAgentObserverTest, Basic);
  WEB_CONTENTS_USER_DATA_KEY_DECL();
};

}  // namespace brave

#endif  // BRAVE_BROWSER_USER_AGENT_USER_AGENT_OBSERVER_H_
