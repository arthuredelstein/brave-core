/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "components/keyed_service/core/keyed_service.h"

namespace webcompat_exceptions {

class WebcompatExceptionsKeyedService : public KeyedService {
  WebcompatExceptionsKeyedService();
  ~WebcompatExceptionsKeyedService() override;
};

}  // namespace webcompat_exceptions
                                    