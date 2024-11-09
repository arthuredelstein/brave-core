// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import {
  html,
  RegisterPolymerTemplateModifications,
  RegisterPolymerComponentReplacement,
  RegisterPolymerComponentBehaviors
} from 'chrome://resources/brave/polymer_overriding.js'
import {BraveSettingsAutofillPageElement} from '../brave_autofill_page/brave_autofill_page.js'
import {loadTimeData} from '../i18n_setup.js'
import {Router} from '../router.js';

RegisterPolymerComponentReplacement(
  'settings-autofill-page', BraveSettingsAutofillPageElement
)
/*
RegisterPolymerComponentBehaviors({
  'settings-autofill-page': [
    {
      onInboxAliasesClicked_: () => {
        const router = Router.getInstance()
        router.navigateTo((router.getRoutes() as any).INBOX_ALIASES)
      }
    }
  ]
})*/

// chrome://settings/inbox_aliases_page/email_shield.svg

RegisterPolymerTemplateModifications({
  'settings-autofill-page': (templateContent) => {
    const parentManagerButton = templateContent.getElementById('paymentManagerButton')
    parentManagerButton.parentNode.insertBefore(html`
      <cr-link-row id="inboxAliasesButton"
          start-icon=""
          label="${loadTimeData.getString('inboxAliasesLabel')}"
          on-click="onInboxAliasesClicked_"
          role-description="$i18n{subpageArrowRoleDescription}"></cr-link-row>
    `, parentManagerButton)
    templateContent.appendChild(html`
        <settings-toggle-button
          class="hr"
          label="${loadTimeData.getString('autofillInPrivateSettingLabel')}"
          sub-label="${loadTimeData.getString('autofillInPrivateSettingDesc')}"
          id="autofill-private-profies"
          pref="{{prefs.brave.autofill_private_windows}}"
          hidden=[[!isAutofillPage_]]
        </settings-toggle-button>
      `)
    },
  }
)
