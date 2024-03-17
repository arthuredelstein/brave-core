/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/webcompat_exceptions/browser/webcompat_exceptions_bridge.h"
#include "brave/components/webcompat_exceptions/browser/webcompat_exceptions_service.h"

namespace webcompat_exceptions {

WebcompatExceptionsImpl::WebcompatExceptionsImpl(mojo::PendingReceiver<webcompat_exceptions::mojom::WebcompatExceptions> receiver)
      : receiver_(this, std::move(receiver)) {}

void WebcompatExceptionsImpl::GetWebcompatExceptions(const GURL& url, GetWebcompatExceptionsCallback reply) {
  std::vector<mojom::WebcompatFeature> features;
  features.push_back(mojom::WebcompatFeature::kHardwareConcurrency);
  std::move(reply).Run(features);
}

WebcompatExceptionsImpl::~WebcompatExceptionsImpl() {}

}  // namespace webcompat_exceptions