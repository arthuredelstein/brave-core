#include "brave/browser/user_agent/brave_user_agent_throttle.h"
#include "services/network/public/cpp/resource_request.h"

namespace brave {

BraveUserAgentThrottle::BraveUserAgentThrottle() = default;
BraveUserAgentThrottle::~BraveUserAgentThrottle() = default;

void BraveUserAgentThrottle::WillStartRequest(network::ResourceRequest* request, bool* defer) {
  if (request) {
    request->headers.SetHeader("Sec-CH-UA", "test-ua-value");
  }
}

}  // namespace brave
