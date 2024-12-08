/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/inbox_aliases_bubble_view.h"

#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/location_bar/location_bar_view.h"
#include "content/public/browser/browser_context.h"
#include "ui/views/controls/webview/webview.h"
#include "chrome/browser/profiles/profile.h"
#include "url/gurl.h"

// static
void InboxAliasesBubbleView::Show(Browser* browser) {
  auto* browser_view = BrowserView::GetBrowserViewForBrowser(browser);
  views::View* anchor_view = browser_view->GetLocationBarView();
  views::Widget* const widget = views::BubbleDialogDelegateView::CreateBubble(
      std::make_unique<InboxAliasesBubbleView>(anchor_view, browser->profile()));
  widget->Show();
}

InboxAliasesBubbleView::InboxAliasesBubbleView(views::View* anchor_view, content::BrowserContext* browser_context)
    : BubbleDialogDelegateView(anchor_view, views::BubbleBorder::TOP_RIGHT) {
    auto* web_view = new views::WebView(browser_context);
    web_view->LoadInitialURL(GURL("https://example.com"));
    AddChildView(web_view);
}

InboxAliasesBubbleView::~InboxAliasesBubbleView() {
}

void InboxAliasesBubbleView::OnWidgetVisibilityChanged(views::Widget* widget,
                                                         bool visible) {
  BubbleDialogDelegateView::OnWidgetVisibilityChanged(widget, visible);
}

BEGIN_METADATA(InboxAliasesBubbleView)
END_METADATA
