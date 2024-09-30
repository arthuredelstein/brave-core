import { BraveVPNState } from "components/brave_new_tab_ui/reducers/brave_vpn";
import * as BraveVPN from '../../../api/braveVpn';
import * as React from 'react';
import { getLocale } from '$web-common/locale'
import * as style from './vpn_banner.style'

export const VPNBannerWidget = ({
  purchasedState,
} : BraveVPNState) => {
  //const dispatch = useDispatch();
  return (
    <style.root hidden={purchasedState !== BraveVPN.PurchasedState.NOT_PURCHASED}>
      <style.heading>
    {getLocale('braveVpnBannerTitle1')}
    </style.heading>
    <style.text>
     {getLocale('braveVpnBannerText1')}
     </style.text>
     <div style={{display: "flex", flexDirection: "row"}}>
     <button style={{margin: "20px", border: "none", padding: "10px", textAlign: "center", lineHeight: "12px", color: "white", paddingTop: "20px",  fontSize: "18px", fontWeight: "bold", backgroundColor: "rgb(76,84,210)", verticalAlign: "center", borderRadius: "10px"}}>
        Start free trial
     </button>
     <div style={{margin: "10px", paddingTop: "20px", fontSize: "18px", fontWeight: "bold"}}>
        <a href="" style={{ textDecoration: "none", color:"rgb(76,4,210)"}}>No thanks</a>
     </div>
     </div>

    </style.root>
  )
}