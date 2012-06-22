#include <process.h>
#include <WinSock2.h>
#include "Webserver.h"

// ---------- Webserver Constructor -------------------------------------------
Webserver::Webserver(short port) {

	Webserver::startWSA();

	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	for (int i = 0; i < 8; i++) { addr.sin_zero[i] = 0; }

	sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock_ == INVALID_SOCKET) {
		throw "INVALID SOCKET";
	}

	if (bind(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))!=0) {
		closesocket(sock_);
		throw "INVALID SOCKET";
	}

	listen(sock_, SOMAXCONN);

	unsigned ret;
	while (true) {
		cSocket* s = this->Accept();
		_beginthreadex(0,0,Request,(void*) s,0,&ret);
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
	SOCKET newsocket = accept(sock_, 0, 0);

	if (newsocket == INVALID_SOCKET) {
		throw "Invalid Socket";
	}

	cSocket* s = new cSocket(newsocket);
	return s;
}

// ---------- Initiate WSA ----------------------------------------------------
void Webserver::startWSA() {
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2,0), &wsaData) == 0) {

		// Check if major version is at least 2
		if (!(LOBYTE(wsaData.wVersion) >= 2)) {
			throw "required version not supported!";
		}
	}
	else {
		throw "startup failed!\n";
	}
}

// ---------- Stop WSA --------------------------------------------------------
void Webserver::stopWSA() {
	WSACleanup();
}