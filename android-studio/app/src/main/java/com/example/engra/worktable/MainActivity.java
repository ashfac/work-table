package com.example.engra.worktable;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.annotation.StringRes;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.example.androidbluetoothserial.BluetoothManager;
import com.example.androidbluetoothserial.SimpleBluetoothDeviceInterface;

import java.util.ArrayList;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import io.reactivex.schedulers.Schedulers;

public class MainActivity extends AppCompatActivity {

    private boolean DEBUG = false;

    private static class DeviceWorkTable {
        private static String name = "H-C-2010-06-01";
        private static String mac = "98:D3:31:40:22:DB";
    };

    private static class DeviceHpSpectreX360 {
        private static String name = "HP-SPECTRE-X360";
        private static String mac = "18:5E:0F:4E:A3:4E";
    };

    Parser parser;

    private ToggleButton btnSpotLights;
    private ToggleButton btnBarLight;
    private ToggleButton btnUnderTableSockets;
    private ToggleButton btnTableTopSockets;
    private ToggleButton btnPower12V;
    private ToggleButton btnPower18V;
    private ToggleButton btnPower24V;
    private ToggleButton btnPowerUSB;

    // A CompositeDisposable that keeps track of all of our asynchronous tasks
    private CompositeDisposable compositeDisposable = new CompositeDisposable();

    // Our BluetoothManager!
    private BluetoothManager bluetoothManager;

    // Our Bluetooth Device! When disconnected it is null, so make sure we know that we need to deal with it potentially being null
    @Nullable
    private SimpleBluetoothDeviceInterface deviceInterface;


    // A variable to help us not double-connect
    private boolean connectionAttemptedOrMade = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        btnSpotLights = findViewById(R.id.button_spot_lights);
        btnBarLight = findViewById(R.id.button_bar_light);
        btnUnderTableSockets = findViewById(R.id.button_under_table_sockets);
        btnTableTopSockets = findViewById(R.id.button_table_top_sockets);
        btnPower12V = findViewById(R.id.button_power_12V);
        btnPower18V = findViewById(R.id.button_power_18V);
        btnPower24V = findViewById(R.id.button_power_24V);
        btnPowerUSB = findViewById(R.id.button_power_USB);

        btnSpotLights.setOnClickListener(v -> {
            HandleButtonClick(Protocol.RELAY_SPOT_LIGHTS, btnSpotLights.isChecked());
        });

        btnBarLight.setOnClickListener(v -> {
            HandleButtonClick(Protocol.RELAY_BAR_LIGHT, btnBarLight.isChecked());
        });

        btnUnderTableSockets.setOnClickListener(v -> {
            HandleButtonClick(Protocol.RELAY_UNDER_TABLE_SOCKETS, btnUnderTableSockets.isChecked());
        });

        btnTableTopSockets.setOnClickListener(v -> {
            HandleButtonClick(Protocol.RELAY_TABLE_TOP_SOCKETS, btnTableTopSockets.isChecked());
        });

        btnPower12V.setOnClickListener(v -> {
            HandleButtonClick(Protocol.RELAY_12V, btnPower12V.isChecked());
        });

        btnPower18V.setOnClickListener(v -> {
            HandleButtonClick(Protocol.RELAY_18V, btnPower18V.isChecked());
        });

        btnPower24V.setOnClickListener(v -> {
            HandleButtonClick(Protocol.RELAY_24V, btnPower24V.isChecked());
        });

        btnPowerUSB.setOnClickListener(v -> {
            HandleButtonClick(Protocol.RELAY_USB, btnPowerUSB.isChecked());
        });

        parser = new Parser();
        parser.setListeners(this::onMessageReceived, this::onSendAcknowledgement);

        // Setup our BluetoothManager
        bluetoothManager = BluetoothManager.getInstance();
        if (bluetoothManager == null) {
            // Bluetooth unavailable on this device :( tell the user
            toast(R.string.bluetooth_unavailable);
        } else {
            connect();
        }
    }

    void HandleButtonClick(byte relay, boolean activate) {
        if(activate) {
            sendMessage(activateRelay(relay));
            sendMessage(getRelayStatus());
        } else {
            sendMessage(deActivateRelay(relay));
            sendMessage(getRelayStatus());
        }
    }

    byte[] activateRelay(byte relay) {
        byte[] value = {relay};
        return Protocol.composeMessage(Protocol.TAG_ACTIVATE_RELAY, value);
    }

    byte[] deActivateRelay(byte relay) {
        byte[] value = {relay};
        return Protocol.composeMessage(Protocol.TAG_DEACTIVATE_RELAY, value);
    }

    byte[] activateAllRelays() {
        return Protocol.composeMessage(Protocol.TAG_ACTIVATE_ALL_RELAYS);
    }

    byte[] deActivateAllRelays() {
        return Protocol.composeMessage(Protocol.TAG_DEACTIVATE_ALL_RELAYS);
    }

    byte[] getRelayStatus() {
        return Protocol.composeMessage(Protocol.TAG_GET_RELAY_STATUS);
    }



    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
//        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
/*
        if (id == R.id.action_settings) {
            return true;
        }
*/

        return super.onOptionsItemSelected(item);
    }

    // Called when the user presses the connect button
    public void connect() {
        // Check we are not already connecting or connected
        if (!connectionAttemptedOrMade) {
            // Connect asynchronously
            compositeDisposable.add(bluetoothManager.openSerialDevice(DeviceWorkTable.mac)
                    .subscribeOn(Schedulers.io())
                    .observeOn(AndroidSchedulers.mainThread())
                    .subscribe(device -> onConnected(device.toSimpleDeviceInterface()), t -> {
                        toast(R.string.connection_failed);
                        connectionAttemptedOrMade = false;
                    }));
            // Remember that we made a connection attempt.
            connectionAttemptedOrMade = true;
        }
    }

    // Called when the user presses the disconnect button
    public void disconnect() {
        // Check we were connected
        if (connectionAttemptedOrMade && deviceInterface != null) {
            connectionAttemptedOrMade = false;
            // Use the library to close the connection
            bluetoothManager.closeDevice(deviceInterface);
            // Set it to null so no one tries to use it
            deviceInterface = null;
        }
    }

    // Called once the library connects a bluetooth device
    private void onConnected(SimpleBluetoothDeviceInterface deviceInterface) {
        this.deviceInterface = deviceInterface;
        if (this.deviceInterface != null) {
            // Setup the listeners for the interface
            this.deviceInterface.setListeners(this::onBytesReceived, this::onBytesSent, this::onError);
            // Tell the user we are connected.
            toast(R.string.connected);
            sendMessage(getRelayStatus());
        } else {
            // deviceInterface was null, so the connection failed
            toast(R.string.connection_failed);
        }
    }

    private void onBytesReceived(byte[] bytes) {
        if(DEBUG) {
            showToast(bytes.toString());
        }

        parser.parse(bytes);
    }

    private void onBytesSent(byte[] bytes) {
        if(DEBUG) {
            showToast(bytes.toString());
        }
    }

    private void onError(Throwable error) {
        showToast(error.toString());
    }

    private void onMessageReceived(byte tag, ArrayList<Byte> value) {
        if(tag == Protocol.TAG_RES_ACKNOWLEDGEMENT) {
            if(value.size() == 1) {
                byte ack = value.get(0);

                if (ack == Protocol.RES_ACK_OK) {
                    if(DEBUG) {
                        showToast("ACK OK");
                    }
                } else if (ack == Protocol.RES_ACK_CHECKSUM_FAILED) {
                    showToast("ACK Checksum failed");
                } else if (ack == Protocol.RES_ACK_MSG_TOO_LONG) {
                    showToast("ACK Message too long");
                } else if (ack == Protocol.RES_ACK_MSG_PARSING_ERROR) {
                    showToast("ACK Error parsing message");
                } else {
                    showToast("Error parsing acknowledgement");
                }

            } else {
                showToast("Error parsing acknowledgement");
            }
        } else if (tag == Protocol.TAG_RES_RELAY_STATUS) {
            if(value.size() == 1) {
                int relay_status = value.get(0);
                btnSpotLights.setChecked((relay_status & Protocol.MASK_RELAY_SPOT_LIGHTS) == Protocol.MASK_RELAY_SPOT_LIGHTS);
                btnBarLight.setChecked((relay_status & Protocol.MASK_RELAY_BAR_LIGHT) == Protocol.MASK_RELAY_BAR_LIGHT);
                btnUnderTableSockets.setChecked((relay_status & Protocol.MASK_RELAY_UNDER_TABLE_SOCKETS) == Protocol.MASK_RELAY_UNDER_TABLE_SOCKETS);
                btnTableTopSockets.setChecked((relay_status & Protocol.MASK_RELAY_TABLE_TOP_SOCKETS) == Protocol.MASK_RELAY_TABLE_TOP_SOCKETS);
                btnPower12V.setChecked((relay_status & Protocol.MASK_RELAY_12V) == Protocol.MASK_RELAY_12V);
                btnPower18V.setChecked((relay_status & Protocol.MASK_RELAY_18V) == Protocol.MASK_RELAY_18V);
                btnPower24V.setChecked((relay_status & Protocol.MASK_RELAY_24V) == Protocol.MASK_RELAY_24V);
                btnPowerUSB.setChecked((relay_status & Protocol.MASK_RELAY_USB) == Protocol.MASK_RELAY_USB);
            } else {
                showToast("Error parsing relay status");
            }
        } else {
            showToast("Unknown message received");
        }
    }

    private void onSendAcknowledgement(byte ack) {

    }

    // Send a message
    public void sendMessage(byte[] message) {
        // Check we have a connected device and the message is not empty, then send the message
        if (deviceInterface != null && message.length > 0) {
            deviceInterface.sendMessage(message);
        }
    }

    // Called when the activity finishes - clear up after ourselves.
    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Dispose any asynchronous operations that are running
        compositeDisposable.dispose();
        // Shutdown bluetooth connections
        bluetoothManager.close();
    }

    // Helper method to create toast messages.
    private void showToast(String message) {
        Toast.makeText(getApplication(), message, Toast.LENGTH_SHORT).show();
    }

    private void toast(@StringRes int messageResource) { Toast.makeText(getApplication(), messageResource, Toast.LENGTH_LONG).show(); }
}

