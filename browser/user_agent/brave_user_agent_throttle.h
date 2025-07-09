#ifndef BRAVE_BROWSER_USER_AGENT_BRAVE_USER_AGENT_THROTTLE_H_
#define BRAVE_BROWSER_USER_AGENT_BRAVE_USER_AGENT_THROTTLE_H_

#include "third_party/blink/public/common/loader/url_loader_throttle.h"

namespace network {
struct ResourceRequest;
}  // namespace network

namespace brave {

class BraveUserAgentThrottle : public blink::URLLoaderThrottle {
 public:
  BraveUserAgentThrottle();
  ~BraveUserAgentThrottle() override;

  void WillStartRequest(network::ResourceRequest* request, bool* defer) override;
};

}  // namespace brave

#endif  // BRAVE_BROWSER_USER_AGENT_BRAVE_USER_AGENT_THROTTLE_H_
