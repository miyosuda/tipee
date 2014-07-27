#include "BluetoothServerSocket.h"
#include "BluetoothSocket.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

/**
 * <!--  ServerSocket():  -->
 */
BluetoothServerSocket::BluetoothServerSocket() {
	s = 0;
}

/**
 * <!--  bind():  -->
 */
bool BluetoothServerSocket::bind(int channel) {
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	struct sockaddr_rc loc_addr = { 0 };

	loc_addr.rc_family = AF_BLUETOOTH;
	loc_addr.rc_bdaddr = *BDADDR_ANY;
	loc_addr.rc_channel = (uint8_t) channel;
	::bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
	
	int ret = ::listen(s, 1);
	return ret != -1;
}

/**
 * <!--  accept():  -->
 */
bool BluetoothServerSocket::accept( BluetoothSocket& socket ) {
	struct sockaddr_rc rem_addr = { 0 };
	socklen_t opt = sizeof(rem_addr);

	int clientS = ::accept(s, (struct sockaddr *)&rem_addr, &opt);
	if( clientS == -1 ) {
		return false;
	}

	char buf[256] = { 0 };
	ba2str( &rem_addr.rc_bdaddr, buf );
	printf("accepted connection from %s\n", buf);
	
	socket.init(clientS);
	return true;
}

/**
 * <!--  close():  -->
 */
bool BluetoothServerSocket::close() {
	int ret = ::close(s);
	if( ret ) {
		s = 0;
		return false;
	}
	s = 0;
	return true;
}
