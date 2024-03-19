/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/webcompat_exceptions/common/webcompat_exceptions.mojom.h"
#include "mojo/public/cpp/bindings/receiver.h"

namespace webcompat_exceptions {

class WebcompatExceptionsImpl
    : public webcompat_exceptions::mojom::WebcompatExceptions {
 public:
  explicit WebcompatExceptionsImpl(
      mojo::PendingReceiver<webcompat_exceptions::mojom::WebcompatExceptions>
          receiver);
  ~WebcompatExceptionsImpl() override;
  void GetWebcompatExceptions(const GURL& url,
                              GetWebcompatExceptionsCallback reply) override;

 private:
  mojo::Receiver<webcompat_exceptions::mojom::WebcompatExceptions> receiver_;
};

}  // namespace webcompat_exceptions
