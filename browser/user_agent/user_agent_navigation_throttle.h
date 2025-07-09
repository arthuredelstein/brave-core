#ifndef BRAVE_BROWSER_USER_AGENT_USER_AGENT_NAVIGATION_THROTTLE_H_
#define BRAVE_BROWSER_USER_AGENT_USER_AGENT_NAVIGATION_THROTTLE_H_

#include "content/public/browser/navigation_throttle.h"

namespace brave {

class UserAgentNavigationThrottle : public content::NavigationThrottle {
 public:
  explicit UserAgentNavigationThrottle(content::NavigationThrottleRegistry& registry);
  ~UserAgentNavigationThrottle() override;

  ThrottleCheckResult WillStartRequest() override;
  const char* GetNameForLogging() override;

  // Static helper to register the throttle for a navigation if desired.
  static void MaybeCreateAndAdd(content::NavigationThrottleRegistry& registry);
};

}  // namespace brave

#endif  // BRAVE_BROWSER_USER_AGENT_USER_AGENT_NAVIGATION_THROTTLE_H_
