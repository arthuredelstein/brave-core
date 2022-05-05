/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/frame/local_dom_window.h"

#define outerHeight outerHeightOriginal
#define outerWidth outerWidthOriginal

#include "src/third_party/blink/renderer/core/frame/local_dom_window.cc"

#undef outerHeight
#undef outerWidth

namespace blink {

void LocalDOMWindow::SetEphemeralStorageOrigin(
    const SecurityOrigin* ephemeral_storage_origin) {
  DCHECK(ephemeral_storage_origin);
  ephemeral_storage_key_ = BlinkStorageKey(ephemeral_storage_origin);
}

const SecurityOrigin* LocalDOMWindow::GetEphemeralStorageOrigin() const {
  return ephemeral_storage_key_
             ? ephemeral_storage_key_->GetSecurityOrigin().get()
             : nullptr;
}

const BlinkStorageKey& LocalDOMWindow::GetEphemeralStorageKeyOrStorageKey()
    const {
  return ephemeral_storage_key_ ? *ephemeral_storage_key_ : storage_key_;
}

const SecurityOrigin*
LocalDOMWindow::GetEphemeralStorageOriginOrSecurityOrigin() const {
  return ephemeral_storage_key_
             ? ephemeral_storage_key_->GetSecurityOrigin().get()
             : GetSecurityOrigin();
}

int LocalDOMWindow::outerHeight() const {
  // Prevent fingerprinter use of outerHeight by returning innerHeight instead:
  return innerHeight();
}

int LocalDOMWindow::outerWidth() const {
  // Prevent fingerprinter use of outerWidth by returning innerWidth instead:
  return innerWidth();
}

}  // namespace blink
