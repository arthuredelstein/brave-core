#include "brave/browser/ui/webui/email_aliases_bubble_ui.h"

#include "brave/browser/ui/webui/settings/brave_email_aliases_handler.h"

EmailAliasesBubbleUI::EmailAliasesBubbleUI(content::WebUI* web_ui)
    : content::WebUIController(web_ui) {
  web_ui->AddMessageHandler(
      std::make_unique<BraveEmailAliasesHandler>());
}
