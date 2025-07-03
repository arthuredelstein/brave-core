/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/net/brave_user_agent_network_delegate_helper.h"

#include <memory>

#include "base/strings/string_util.h"
#include "brave/components/brave_user_agent/browser/brave_user_agent_exceptions.h"
#include "net/base/net_errors.h"
#include "net/http/http_request_headers.h"

namespace brave {

int OnBeforeStartTransaction_UserAgentWork(
    net::HttpRequestHeaders* headers,
    const ResponseCallback& next_callback,
    std::shared_ptr<BraveRequestInfo> ctx) {
  if (ctx) {
    auto* exceptions = brave_user_agent::BraveUserAgentExceptions::GetInstance();
    if (exceptions) {
      bool show_brave = exceptions->CanShowBrave(ctx->tab_origin);
      if (!show_brave) {
        std::optional<std::string> sec_ch_ua = headers->GetHeader("Sec-CH-UA");
        if (sec_ch_ua) {
          std::string sec_ch_ua_value = sec_ch_ua.value();
          base::ReplaceFirstSubstringAfterOffset(
              &sec_ch_ua_value, /*start_offset=*/0, "\"Brave\"", "\"Google Chrome\"");
          headers->SetHeader("Sec-CH-UA", sec_ch_ua_value);
        }
      }
    }
  }
  return net::OK;
}

}  // namespace brave
