/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/settings/brave_inbox_aliases_handler.h"

#include <memory>
#include <string>
#include <utility>

#include "base/functional/bind.h"
#include "base/json/values_util.h"
#include "base/values.h"
#include "brave/browser/brave_browser_process.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_finder.h"
#include "content/public/browser/web_ui.h"

BraveInboxAliasesHandler::BraveInboxAliasesHandler() = default;

BraveInboxAliasesHandler::~BraveInboxAliasesHandler() = default;

void BraveInboxAliasesHandler::RegisterMessages() {
  profile_ = Profile::FromWebUI(web_ui());
  web_ui()->RegisterMessageCallback(
      "inbox_aliases.generateNewAlias",
      base::BindRepeating(&BraveInboxAliasesHandler::GenerateNewAlias,
                          base::Unretained(this)));
}

void BraveInboxAliasesHandler::GenerateNewAlias(const base::Value::List& args) {
  AllowJavascript();
  ResolveJavascriptCallback(args[0], base::Value("tweedledum-tweedledee@gmail.com"));
}