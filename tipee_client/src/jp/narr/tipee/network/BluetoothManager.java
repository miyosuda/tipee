package jp.narr.tipee.network;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.UUID;

import jp.narr.tipee.command.Command;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

public class BluetoothManager {
	private final String TAG = "tipee";

	private final BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
	// SPPのUUIDを指定
	private final UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
	private ClientThread clientThread;

	public BluetoothManager() {
	}
	
	public boolean isEnabled() {
		return bluetoothAdapter.isEnabled();
	}

	public void connect(String address) {
		clientThread = new ClientThread(address);
		clientThread.start();
	}

	public void sendCommand(Command command) {
		try {
			if (clientThread != null) {
				clientThread.sendCommand(command);
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void cancel() {
		if (clientThread != null) {
			clientThread.cancel();
			clientThread = null;
		}
	}

	/**
	 * サーバに接続する為のスレッド
	 */
	private class ClientThread extends Thread {
		private final BluetoothDevice device;
		private BluetoothSocket socket;
		private DataOutputStream out;
		private InputStream in;

		private ClientThread(String address) {
			device = bluetoothAdapter.getRemoteDevice(address);
			Log.i(TAG, "connecting to: " + device.getName());
			
			try {
				socket = device.createRfcommSocketToServiceRecord(uuid);
			} catch (IOException e) {
				Log.e(TAG, "socket create failed", e);
				e.printStackTrace();
			}
		}

		@Override
		public void run() {
			try {
				// サーバに接続する
				socket.connect();
				out = new DataOutputStream(socket.getOutputStream());
				in = socket.getInputStream();				
				loop();
			} catch (IOException e) {
				e.printStackTrace();
				Log.e(TAG, "exception in loop", e);
			} finally {
				cancel();
			}
		}

		private void cancel() {
			try {
				socket.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		public void sendCommand(Command command) throws IOException {
			command.write(out);
		}

		private void loop() throws IOException {
			int command;
			while ((command = in.read()) != -1) {
				Log.i(TAG, "received command=" + command);
			}
		}
	}
}
