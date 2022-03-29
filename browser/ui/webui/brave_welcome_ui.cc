/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/brave_welcome_ui.h"

#include <algorithm>
#include <memory>
#include <string>

#include "base/feature_list.h"
#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "brave/browser/ui/webui/brave_webui_source.h"
#include "brave/browser/ui/webui/settings/brave_import_data_handler.h"
#include "brave/browser/ui/webui/settings/brave_search_engines_handler.h"
#include "brave/common/pref_names.h"
#include "brave/common/webui_url_constants.h"
#include "brave/components/brave_welcome/common/features.h"
#include "brave/components/brave_welcome/resources/grit/brave_welcome_generated_map.h"
#include "brave/components/p3a/pref_names.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/chrome_pages.h"
#include "chrome/common/pref_names.h"
#include "chrome/common/webui_url_constants.h"
#include "components/country_codes/country_codes.h"
#include "components/grit/brave_components_resources.h"
#include "components/prefs/pref_change_registrar.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/page_navigator.h"
#include "content/public/browser/web_ui_data_source.h"
#include "content/public/browser/web_ui_message_handler.h"

using content::WebUIMessageHandler;

namespace {

void OpenJapanWelcomePage(Profile* profile) {
  DCHECK(profile);
  Browser* browser = chrome::FindBrowserWithProfile(profile);
  if (browser) {
    content::OpenURLParams open_params(
        GURL("https://brave.com/ja/desktop-ntp-tutorial"), content::Referrer(),
        WindowOpenDisposition::NEW_BACKGROUND_TAB,
        ui::PAGE_TRANSITION_AUTO_TOPLEVEL, false);
    browser->OpenURL(open_params);
  }
}

// Returns whether the P3A opt-in prompt should be shown.
bool IsP3AOptInEnabled() {
  bool enabled = base::FeatureList::IsEnabled(brave_welcome::features::kP3AOptIn);
  VLOG(1) << "IsP3AOptInEnabled: " << enabled;
  return enabled;
}

void RecordP3AHistogram(int screen_number, bool finished) {
  int answer = 0;
  if (finished) {
    answer = 3;
  } else {
    answer = std::min(screen_number, 2);
  }
  VLOG(1) << "RecordP3AHistogram value " << answer;
  UMA_HISTOGRAM_EXACT_LINEAR("Brave.Welcome.InteractionStatus", answer, 3);
}

void RecordP3AOptIn(int screen_number, bool opt_in) {
  // Record nothing if the feature is disabled.
  if (!IsP3AOptInEnabled()) {
    return;
  }
  int answer = 0; // Did not see prompt.
  if (screen_number > 2) {
    answer = 1 + opt_in; // Saw prompt, didn't or did opt in.
  }
  VLOG(1) << "RecordP3AOptIn histogram value " << answer;
  UMA_HISTOGRAM_EXACT_LINEAR("Brave.Welcome.P3AOptIn", answer, 2);
}

// The handler for Javascript messages for the chrome://welcome page
class WelcomeDOMHandler : public WebUIMessageHandler {
 public:
  WelcomeDOMHandler() = default;
  WelcomeDOMHandler(const WelcomeDOMHandler&) = delete;
  WelcomeDOMHandler& operator=(const WelcomeDOMHandler&) = delete;
  ~WelcomeDOMHandler() override;

  // WebUIMessageHandler implementation.
  void RegisterMessages() override;

 private:
  void HandleImportNowRequested(base::Value::ConstListView args);
  void HandleRecordP3A(base::Value::ConstListView args);
  void HandleSetP3AEnable(base::Value::ConstListView args);
  Browser* GetBrowser();

  int screen_number_ = 0;
  bool finished_ = false;
  bool skipped_ = false;
  bool p3a_opt_in_ = false;
};

WelcomeDOMHandler::~WelcomeDOMHandler() {
  VLOG(1) << "WelcomeDOMHandler dtor: recording p3a values";
  RecordP3AHistogram(screen_number_, finished_);
  RecordP3AOptIn(screen_number_, p3a_opt_in_);
}

Browser* WelcomeDOMHandler::GetBrowser() {
  return chrome::FindBrowserWithWebContents(web_ui()->GetWebContents());
}

void WelcomeDOMHandler::RegisterMessages() {
  web_ui()->RegisterMessageCallback(
      "importNowRequested",
      base::BindRepeating(&WelcomeDOMHandler::HandleImportNowRequested,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "recordP3A", base::BindRepeating(&WelcomeDOMHandler::HandleRecordP3A,
                                       base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "setP3AEnable", base::BindRepeating(&WelcomeDOMHandler::HandleSetP3AEnable,
                                       base::Unretained(this)));
}

void WelcomeDOMHandler::HandleImportNowRequested(
    base::Value::ConstListView args) {
  chrome::ShowSettingsSubPageInTabbedBrowser(GetBrowser(),
                                             chrome::kImportDataSubPage);
}


void WelcomeDOMHandler::HandleRecordP3A(base::Value::ConstListView args) {
  if (!args[0].is_int() || !args[1].is_bool() || !args[2].is_bool())
    return;
  screen_number_ = args[0].GetInt();
  finished_ = args[1].GetBool();
  skipped_ = args[2].GetBool();

  VLOG(1) << "HandleRecordP3A "
    << "screen_number: " << screen_number_ << ", "
    << "finished: " << finished_ << ", "
    << "skipped: " << skipped_ << ", "
    << "p3a_opt_in: " << p3a_opt_in_;

  if (screen_number_) {
    // It is 1-based on JS side, we want 0-based.
    screen_number_--;
  }
  RecordP3AHistogram(screen_number_, finished_);
  RecordP3AOptIn(screen_number_, p3a_opt_in_);
}

void WelcomeDOMHandler::HandleSetP3AEnable(base::Value::ConstListView args) {
  VLOG(1) << "HandleEnableP3A: " << args.size() << " args";
  for (auto& arg : args) {
    VLOG(1) << "  " << arg.type() << ": " << arg;
  }
  DCHECK(args.size() >= 1);
  if (!args[0].is_bool()) {
    DLOG(WARNING) << "Wrong argument type " << args[0].type()
      << " passed to HandleSetP3AEnable";
    return;
  }
  p3a_opt_in_ = args[0].GetBool();
  // TODO: change kP3AEnabled pref
}

// Converts Chromium country ID to 2 digit country string
// For more info see src/components/country_codes/country_codes.h
std::string CountryIDToCountryString(int country_id) {
  if (country_id == country_codes::kCountryIDUnknown)
    return std::string();

  char chars[3] = {static_cast<char>(country_id >> 8),
                   static_cast<char>(country_id), 0};
  std::string country_string(chars);
  DCHECK_EQ(country_string.size(), 2U);
  return country_string;
}


}  // namespace

BraveWelcomeUI::BraveWelcomeUI(content::WebUI* web_ui, const std::string& name)
    : WebUIController(web_ui) {
  content::WebUIDataSource* source = CreateAndAddWebUIDataSource(
      web_ui, name, kBraveWelcomeGenerated, kBraveWelcomeGeneratedSize,
      IDR_BRAVE_WELCOME_HTML,
      /*disable_trusted_types_csp=*/true);

  web_ui->AddMessageHandler(std::make_unique<WelcomeDOMHandler>());
  web_ui->AddMessageHandler(
      std::make_unique<settings::BraveImportDataHandler>());

  Profile* profile = Profile::FromWebUI(web_ui);
  // added to allow front end to read/modify default search engine
  web_ui->AddMessageHandler(
      std::make_unique<settings::BraveSearchEnginesHandler>(profile));

  // Open additional page in Japanese region
  int country_id = country_codes::GetCountryIDFromPrefs(profile->GetPrefs());
  if (!profile->GetPrefs()->GetBoolean(prefs::kHasSeenWelcomePage)) {
    if (country_id == country_codes::CountryStringToCountryID("JP")) {
      base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
          FROM_HERE, base::BindOnce(&OpenJapanWelcomePage, profile),
          base::Seconds(3));
    }
  }
  // Variables considered when determining which onboarding cards to show
  source->AddString("countryString", CountryIDToCountryString(country_id));
  source->AddBoolean(
      "showRewardsCard",
      base::FeatureList::IsEnabled(brave_welcome::features::kShowRewardsCard));
  source->AddBoolean("showP3AOptIn", IsP3AOptInEnabled());

  profile->GetPrefs()->SetBoolean(prefs::kHasSeenWelcomePage, true);
}

BraveWelcomeUI::~BraveWelcomeUI() = default;
