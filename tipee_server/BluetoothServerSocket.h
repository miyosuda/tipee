// -*- C++ -*-
#ifndef BLUETOOTHSERVERSOCKET_HEADER
#define BLUETOOTHSERVERSOCKET_HEADER

class BluetoothSocket;

class BluetoothServerSocket {
private:
	int s; // socket desc

public:
	BluetoothServerSocket();
	bool bind(int channel);
	bool accept( BluetoothSocket& socket );
	bool close();
};

#endif
