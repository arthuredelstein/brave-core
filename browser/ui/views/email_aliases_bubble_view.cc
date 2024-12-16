/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/email_aliases_bubble_view.h"

#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/location_bar/location_bar_view.h"
#include "content/public/browser/browser_context.h"
#include "ui/views/controls/webview/webview.h"
#include "chrome/browser/profiles/profile.h"
#include "url/gurl.h"
#include "ui/views/layout/fill_layout.h"
#include "brave/components/constants/webui_url_constants.h"
#include "ui/base/mojom/dialog_button.mojom.h"

// static
void EmailAliasesBubbleView::Show(Browser* browser) {
  auto* browser_view = BrowserView::GetBrowserViewForBrowser(browser);
  views::View* anchor_view = browser_view->GetLocationBarView();
  views::Widget* const widget = views::BubbleDialogDelegateView::CreateBubble(
      std::make_unique<EmailAliasesBubbleView>(anchor_view, browser));
  widget->Show();
}

EmailAliasesBubbleView::EmailAliasesBubbleView(views::View* anchor_view, Browser* browser)
    : BubbleDialogDelegateView(anchor_view, views::BubbleBorder::TOP_CENTER), browser_(browser) {
  SetLayoutManager(std::make_unique<views::FillLayout>());
  
  auto* web_view = new views::WebView(browser->profile());
  web_view->SetPreferredSize(gfx::Size(350, 300));
  AddChildView(web_view);
  SetButtons(static_cast<int>(ui::mojom::DialogButton::kNone));

  // Load URL after adding to view hierarchy
  web_view->LoadInitialURL(GURL(kEmailAliasesBubbleURL));
}

EmailAliasesBubbleView::~EmailAliasesBubbleView() {
}

void EmailAliasesBubbleView::OnWidgetVisibilityChanged(views::Widget* widget,
                                                         bool visible) {
  BubbleDialogDelegateView::OnWidgetVisibilityChanged(widget, visible);
}

BEGIN_METADATA(EmailAliasesBubbleView)
END_METADATA
