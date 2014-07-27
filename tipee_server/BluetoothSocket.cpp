#include "BluetoothSocket.h"

#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

/**
 * <!--  advanceBuffer():  -->
 */
static void* advanceBuffer( void* buf, int size ) {
	char* c = (char*)buf;
	c += size;
	return (void*)c;
}

/**
 * <!--  advanceBuffer():  -->
 */
static const void* advanceBuffer( const void* buf, int size ) {
	const char* c = (const char*)buf;
	c += size;
	return (const void*)c;
}

/**
 * <!--  BluetoothSocket():  -->
 */
BluetoothSocket::BluetoothSocket() {
	s = 0;
}

/**
 * <!--  init():  -->
 */
void BluetoothSocket::init(int s_) {
	s = s_;
}

/**
 * <!--  write():  -->
 */
int BluetoothSocket::write( const void *buffer, int length ) {
	int s_ = s;
	const void* buf = buffer;
	int sentSize = 0;

	while( sentSize < length ) {
		int ret = send( s_, (const char*)buf, length - sentSize, 0 );
		if( ret == EOF ) {
			return -1;
		}
		sentSize += ret;
		buf = advanceBuffer(buf, ret);
	}

	return sentSize;
}

/**
 * <!--  read():  -->
 */
int BluetoothSocket::read( void *buffer, int length ) {
	void* buf = buffer;
	int receievdSize = 0;
	
	while( receievdSize < length ) {
		int ret = ::read( s, (char*)buf, length - receievdSize);
        if( ret == EOF ) {
			return -1;
        } else if( ret == 0 ) {
			// connection closed by peer.
			return 0;
		}
		receievdSize += ret;
		buf = advanceBuffer(buf, ret);
	}
	return receievdSize;
}
