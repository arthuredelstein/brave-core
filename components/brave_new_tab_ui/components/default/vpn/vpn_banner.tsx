import { BraveVPNState } from "components/brave_new_tab_ui/reducers/brave_vpn";
import * as BraveVPN from '../../../api/braveVpn';
import * as React from 'react';
import Icon from "@brave/leo/react/icon";
import { getLocale } from '$web-common/locale'
import * as style from './vpn_banner.style'

export const VPNBannerWidget = ({
  purchasedState,
} : BraveVPNState) => {
  //const dispatch = useDispatch();
  return (
    <style.root hidden={purchasedState !== BraveVPN.PurchasedState.NOT_PURCHASED}>
      <style.heading>
    <Icon name="product-vpn" />
    Your IP address is not hidden.
    </style.heading>
    <style.text>
     {getLocale('braveVpnBannerText1')}
      <div>No thanks</div><div>Tell me more</div>
    </style.text>
    </style.root>
  )
}