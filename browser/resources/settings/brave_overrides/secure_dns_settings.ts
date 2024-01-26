// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

import {RegisterPolymerTemplateModifications} from 'chrome://resources/brave/polymer_overriding.js'

RegisterPolymerTemplateModifications({
  'settings-secure-dns': (templateContent) => {
    // if DoH fallback is enabled
    const dohProviderMenu = templateContent.getElementById('resolverSelect');
    if (!dohProviderMenu) {
      console.error('[Brave Settings Overrides] Could not find resolverSelect id on security page.')
    } else {
      const automaticWithFallbackOption = document.createElement("option");
      automaticWithFallbackOption.setAttribute('value', 'automaticWithFallback');
      automaticWithFallbackOption.innerText = 'OS default or Quad9';//loadTimeData.getString('automaticWithFallbackDescription');
      dohProviderMenu.insertBefore(automaticWithFallbackOption, dohProviderMenu.firstChild);
    }
    // end if DoH fallback is enabled
  }
})
