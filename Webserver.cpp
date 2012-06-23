#include <process.h>
#include <WinSock2.h>
#include "Webserver.h"

// ---------- Webserver Constructor -------------------------------------------
Webserver::Webserver(short port) {
	Webserver::startWSA();						// Initiate WSA
	sockaddr_in addr;							// Sockaddr struct for address

	addr.sin_family = AF_INET;					// Adressfamily = IPV4
	addr.sin_port = htons(port);				// Set addr port
	addr.sin_addr.S_un.S_addr = INADDR_ANY;		// Any IP addr
	for (int i = 0; i < 8; i++) { addr.sin_zero[i] = 0; }	// Set all to zero

	sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		// Create a socket

	if (sock_ == INVALID_SOCKET) {				// If socket is invalid
		throw "INVALID SOCKET";		
	}

	if (bind(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))!=0) {
		closesocket(sock_);						
		throw "INVALID SOCKET";		
	}

	listen(sock_, SOMAXCONN);					// Start to listen on socket

	unsigned ret;								// Return value of thread
	while (true) {
		cSocket* s = this->Accept();					// Accept new "client"
		_beginthreadex(0,0,Request,(void*) s,0,&ret);	// Start a thread
	}
}

// ---------- Webserver Deonstructor ------------------------------------------
Webserver::~Webserver() {
	Webserver::stopWSA();
}

// ---------- Create a Request Handler ----------------------------------------
unsigned Webserver::Request(void* ptrSock) {
	cHandler requestHandler((reinterpret_cast<cSocket*>(ptrSock)));
	return 0;
}

// ---------- Accept an incomming SOCKET --------------------------------------
Webserver::cSocket* Webserver::Accept() {
	SOCKET newsocket = accept(sock_, 0, 0);		// Create a new socket...

	if (newsocket == INVALID_SOCKET) {			// If accept gave error
		throw "INVALID SOCKET";
	}

	cSocket* s = new cSocket(newsocket);		// Create new cSocket	
	return s;									// Return new cSocket
}

// ---------- Initiate WSA ----------------------------------------------------
void Webserver::startWSA() {
	WSADATA wsaData;									// WSA data struct

	if (WSAStartup(MAKEWORD(2,0), &wsaData) == 0) {		// Start WSA
		if (!(LOBYTE(wsaData.wVersion) >= 2)) {			// Check the version
			throw "REQUIRED VERSION NOT SUPPORTED";
		}
	}
	else {												// If error at startup
		throw "STARTUP FAILED";
	}
}

// ---------- Stop WSA --------------------------------------------------------
void Webserver::stopWSA() {
	WSACleanup();
}