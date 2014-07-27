// -*- C++ -*-
#ifndef BLUETOOTHSOCKET_HEADER
#define BLUETOOTHSOCKET_HEADER

class BluetoothSocket {
private:
	int s; // socket desc

public:
	BluetoothSocket();
	void init(int s_);
	int write( const void *buffer, int length );
	int read( void *buffer, int length );
};


#endif
