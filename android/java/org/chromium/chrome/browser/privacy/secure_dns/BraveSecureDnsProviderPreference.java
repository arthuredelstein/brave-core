// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.privacy.secure_dns;

import android.content.Context;
import android.util.AttributeSet;

import androidx.preference.PreferenceViewHolder;

import org.chromium.base.BraveFeatureList;
import org.chromium.chrome.browser.flags.ChromeFeatureList;
import org.chromium.chrome.browser.privacy.secure_dns.SecureDnsProviderPreference;
import org.chromium.components.browser_ui.widget.RadioButtonWithDescription;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * SecureDnsProviderPreference is the user interface that is shown when Secure DNS is enabled.
 * When Secure DNS is disabled, the SecureDnsProviderPreference is hidden.
 */
class BraveSecureDnsProviderPreference extends SecureDnsProviderPreference {
    private String mPrimaryText;
    private String mDescriptionText;

    public BraveSecureDnsProviderPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
        if (ChromeFeatureList.isEnabled(BraveFeatureList.BRAVE_FALLBACK_DOH_PROVIDER)) {
            int index = ChromeFeatureList.getFieldTrialParamByFeatureAsInt(
                    BraveFeatureList.BRAVE_FALLBACK_DOH_PROVIDER,
                    "BraveFallbackDoHProviderEndpointIndex", 0);
            switch (index) {
                case 1:
                    mPrimaryText =
                            context.getString(R.string.settings_automatic_mode_with_quad9_label);
                    break;
                case 2:
                    mPrimaryText = context.getString(
                            R.string.settings_automatic_mode_with_wikimedia_label);
                    break;
                case 3:
                    mPrimaryText = context.getString(
                            R.string.settings_automatic_mode_with_cloudflare_label);
                    break;
                default:
                    mPrimaryText = context.getString(R.string.settings_automatic_mode_label);
                    break;
            }
            mDescriptionText =
                    context.getString(R.string.settings_automatic_mode_description_fallback);
        }
    }

    @Override
    public void onBindViewHolder(PreferenceViewHolder holder) {
        super.onBindViewHolder(holder);
        if (ChromeFeatureList.isEnabled(BraveFeatureList.BRAVE_FALLBACK_DOH_PROVIDER)) {
            RadioButtonWithDescription automaticModeButton =
                    (RadioButtonWithDescription) holder.findViewById(R.id.automatic);
            automaticModeButton.setPrimaryText(mPrimaryText);
            automaticModeButton.setDescriptionText(mDescriptionText);
        }
    }
}
