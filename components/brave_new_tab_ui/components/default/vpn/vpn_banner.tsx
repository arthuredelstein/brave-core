import { BraveVPNState } from "components/brave_new_tab_ui/reducers/brave_vpn";
import * as BraveVPN from '../../../api/braveVpn';
import * as React from 'react';

export const VPNBannerWidget = ({
  purchasedState,
} : BraveVPNState) => {
  //const dispatch = useDispatch();
  return (
    purchasedState === BraveVPN.PurchasedState.NOT_PURCHASED ?
    (<div>
      BUY THE VPN!!
    </div>) : <span></span>
  )
};