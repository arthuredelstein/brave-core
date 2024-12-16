#ifndef BRAVE_BROWSER_UI_WEBUI_EMAIL_ALIASES_BUBBLE_UI_H_
#define BRAVE_BROWSER_UI_WEBUI_EMAIL_ALIASES_BUBBLE_UI_H_

#include "content/public/browser/web_ui_controller.h"
#include "content/public/browser/webui_config.h"
#include "content/public/common/url_constants.h"
#include "brave/components/constants/webui_url_constants.h"

class EmailAliasesBubbleUI : public content::WebUIController {
 public:
  explicit EmailAliasesBubbleUI(content::WebUI* web_ui);
};

class EmailAliasesBubbleUIConfig
    : public content::DefaultWebUIConfig<EmailAliasesBubbleUI> {
 public:
  EmailAliasesBubbleUIConfig()
      : DefaultWebUIConfig(content::kChromeUIScheme, kEmailAliasesBubbleHost) {}
};

#endif  // BRAVE_BROWSER_UI_WEBUI_EMAIL_ALIASES_BUBBLE_UI_H_