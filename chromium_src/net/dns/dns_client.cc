/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "base/feature_list.h"
#include "net/base/features.h"
#include "net/dns/dns_util.h"
#include "net/dns/public/dns_over_https_server_config.h"

#include <vector>

namespace net {
namespace {

constexpr char const* endpointAddresses[] = {
    "",  // Not for use
    "https://dns.quad9.net/dns-query", "https://wikimedia-dns.org/dns-query",
    "https://cloudflare-dns.com/dns-query"};

std::vector<DnsOverHttpsServerConfig> MaybeAddFallbackDohServer(
    const std::vector<DnsOverHttpsServerConfig>& doh_servers) {
  static const int endpointIndex =
      net::features::kBraveFallbackDoHProviderEndpointIndex.Get();
  if (!base::FeatureList::IsEnabled(net::features::kBraveFallbackDoHProvider) ||
      endpointIndex <= 0 || endpointIndex >= (int)sizeof(endpointAddresses)) {
    return doh_servers;
  }
  std::vector<DnsOverHttpsServerConfig> extended_doh_servers = doh_servers;
  auto fallbackDohServer =
      DnsOverHttpsServerConfig::FromString(endpointAddresses[endpointIndex]);
  if (fallbackDohServer.has_value()) {
    extended_doh_servers.push_back(fallbackDohServer.value());
  }
  return extended_doh_servers;
}

}  // namespace
}  // namespace net

#define GetDohUpgradeServersFromNameservers(...) \
  MaybeAddFallbackDohServer(GetDohUpgradeServersFromNameservers(__VA_ARGS__))

#include "src/net/dns/dns_client.cc"

#undef GetDohUpgradeServersFromNameservers
