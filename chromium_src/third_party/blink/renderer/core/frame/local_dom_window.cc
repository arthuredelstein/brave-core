/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/events/mouse_event.h"
#include "third_party/blink/renderer/core/events/pointer_event.h"
#include "src/third_party/blink/renderer/core/execution_context/execution_context.h"

#define outerHeight outerHeightOriginal
#define outerWidth outerWidthOriginal
#define screenX screenXOriginal
#define screenY screenYOriginal

#include "src/third_party/blink/renderer/core/frame/local_dom_window.cc"

#undef outerHeight
#undef outerWidth
#undef screenX
#undef screenY

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
  // Prevent fingerprinter use of outerHeight by returning a farbled value near innerHeight instead:
  const int inner = innerHeight();
  return brave::FarbledInteger(
    GetExecutionContext(), "window_innerHeight",
    inner, inner + 8, outerHeightOriginal());
}

int LocalDOMWindow::outerWidth() const {
  // Prevent fingerprinter use of outerWidth by returning a farbled value near innerWidth instead:
  const int inner = innerWidth();
  return brave::FarbledInteger(
    GetExecutionContext(), "window_innerWidth",
    inner, inner + 8, outerWidthOriginal());
}

int LocalDOMWindow::screenX() const {
  // Prevent fingerprinter use of screenX, screenLeft by returning 0:
  return brave::FarbledInteger(GetExecutionContext(), "window_screenX", 0, 8, screenXOriginal());
}

int LocalDOMWindow::screenY() const {
  // Prevent fingerprinter use of screenY, screenTop by returning 0:
  return brave::FarbledInteger(GetExecutionContext(), "window_screenY", 0, 8, screenYOriginal());
}

}  // namespace blink
