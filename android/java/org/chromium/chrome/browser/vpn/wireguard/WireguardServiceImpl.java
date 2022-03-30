package org.chromium.chrome.browser.vpn.wireguard;

import static org.chromium.chrome.browser.vpn.wireguard.WireguardUtils.getInterface;
import static org.chromium.chrome.browser.vpn.wireguard.WireguardUtils.getPeers;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Build;
import android.os.CountDownTimer;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;

import com.wireguard.android.backend.Backend;
import com.wireguard.android.backend.BackendException;
import com.wireguard.android.backend.GoBackend;
import com.wireguard.android.backend.Statistics;
import com.wireguard.android.backend.Tunnel;
import com.wireguard.config.Config;

import org.chromium.base.ContextUtils;
import org.chromium.chrome.R;
import org.chromium.chrome.browser.notifications.channels.BraveChannelDefinitions;
import org.chromium.chrome.browser.vpn.utils.BraveVpnUtils;

public class WireguardServiceImpl
        extends WireguardService.Impl implements TunnelModel.TunnelStateUpdateListener {
    private final IBinder mBinder = new WireguardServiceBinder();

    private Backend mBackend;
    private TunnelModel mTunnelModel;
    private NotificationManager mNotificationManager;

    public TunnelModel getTunnelModel() {
        return mTunnelModel;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mBackend = new GoBackend(ContextUtils.getApplicationContext());
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        new Thread() {
            @Override
            public void run() {
                try {
                    startVpn();
                } catch (BackendException e) {
                    if (e.getReason() == BackendException.Reason.VPN_NOT_AUTHORIZED) {
                        Log.e("BraveVPN", e.getReason().name());
                    }
                } catch (Exception e) {
                    Log.e("BraveVPN", e.getMessage());
                }
            }
        }.start();
        getService().startForeground(BraveChannelDefinitions.ChannelId.BRAVE_BROWSER_CHANNEL_INT,
                BraveVpnUtils.getBraveVpnNotification(ContextUtils.getApplicationContext()));
        return Service.START_NOT_STICKY;
    }

    private void startVpn() throws Exception {
        // if (!WireguardConfigUtils.isConfigExist(ContextUtils.getApplicationContext())) {
        //     getService().stopForeground(true);
        //     getService().stopSelf();
        // }
        Config config = WireguardConfigUtils.loadConfig(ContextUtils.getApplicationContext());
        // if (WireguardConfigUtils.isConfigExist(ContextUtils.getApplicationContext())) {
        //     config = WireguardConfigUtils.loadConfig(ContextUtils.getApplicationContext());
        // } else {
        //     config = WireguardConfigUtils.createConfig(ContextUtils.getApplicationContext());
        // }
        mTunnelModel = TunnelModel.createTunnel(config, this);
        mBackend.setState(mTunnelModel, Tunnel.State.UP, config);
    }

    @Override
    public void onDestroy() {
        try {
            mBackend.setState(mTunnelModel, Tunnel.State.DOWN, null);
            // WireguardConfigUtils.deleteConfig(ContextUtils.getApplicationContext(), tunnelName);
        } catch (Exception e) {
            e.printStackTrace();
        }
        super.onDestroy();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public void onTunnelStateUpdated(TunnelModel tunnelModel) {
        // sendVpnStatusBroadcast(tunnelModel);
        if (tunnelModel.getState() == Tunnel.State.DOWN) {
            getService().stopForeground(true);
            getService().stopSelf();
        }
    }

    // private void sendVpnStatusBroadcast(TunnelModel tunnelModel) {
    //     Intent vpnStatusIntent = new Intent();
    //     vpnStatusIntent.setAction("vpn_status");
    //     vpnStatusIntent.putExtra("name", tunnelModel.getName());
    //     vpnStatusIntent.putExtra("status", tunnelModel.getState() == Tunnel.State.UP);
    //     sendBroadcast(vpnStatusIntent);
    // }

    public class WireguardServiceBinder extends Binder {
        public WireguardServiceImpl getService() {
            return WireguardServiceImpl.this;
        }
    }
}
