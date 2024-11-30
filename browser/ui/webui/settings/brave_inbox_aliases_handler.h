/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_WEBUI_SETTINGS_BRAVE_INBOX_ALIASES_HANDLER_H_
#define BRAVE_BROWSER_UI_WEBUI_SETTINGS_BRAVE_INBOX_ALIASES_HANDLER_H_

#include <optional>
#include <string>

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "chrome/browser/ui/webui/settings/settings_page_ui_handler.h"

class Profile;

namespace network {
  class SimpleURLLoader;
}

class BraveInboxAliasesHandler : public settings::SettingsPageUIHandler {
 public:
  BraveInboxAliasesHandler();
  BraveInboxAliasesHandler(const BraveInboxAliasesHandler&) = delete;
  BraveInboxAliasesHandler& operator=(const BraveInboxAliasesHandler&) = delete;
  ~BraveInboxAliasesHandler() override;

  void GenerateNewAlias(const base::Value::List& args);
  void OnGenerateNewAliasResponse(
    const std::string callback_id, std::optional<std::string> response_body);

 private:
  // SettingsPageUIHandler overrides
  void RegisterMessages() override;

  void OnJavascriptAllowed() override {}
  void OnJavascriptDisallowed() override {}

  raw_ptr<Profile> profile_ = nullptr;

  std::unique_ptr<network::SimpleURLLoader> simple_url_loader_;

  base::WeakPtrFactory<BraveInboxAliasesHandler> weak_factory_{this};
};

#endif  // BRAVE_BROWSER_UI_WEBUI_SETTINGS_BRAVE_INBOX_ALIASES_HANDLER_H_
