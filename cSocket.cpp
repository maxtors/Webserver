#include <WinSock2.h>
#include <string>
#include "Webserver.h"

#define CSOCKET_ERROR   -1
#define CSOCKET_END      0
#define CSOCKET_VERSION  2

// ---------- cScoket Constructor ---------------------------------------------
cSocket::cSocket(SOCKET s)   { sock_ = s;            }

// ---------- cScoket Deonstructor --------------------------------------------
cSocket::~cSocket()          { close();              }

// ---------- Close the socket ------------------------------------------------
inline void cSocket::close() { closesocket(sock_);   }

// ---------- Receive Data ----------------------------------------------------
std::string cSocket::rxData() {
    char buffer[1024];
    int retval;                         // Number of bytes received

    while (true) {
        retval = recv(sock_, buffer, sizeof(buffer)-1, 0);

        if (retval == CSOCKET_END) {                // If 0 bytes received
            break;                                  // Connection was closed
        }
        else if (retval == CSOCKET_ERROR) {         // If error value received
            throw "socket error while receiving.";
        }
        else {
            buffer[retval] = 0;	        // Terminate buffer with \0
            return buffer;              // Return buffer
        }
    }
    return "";                          // Fallback return
}

// ---------- Receive a single line -------------------------------------------
std::string cSocket::rxLine() {
	std::string result;                 // String to hold the received line
	char r;                             // Buffer for receiving char
	int rStatus;                        // Receive status

    while (true) {                          // Loop until line is read
        rStatus = recv(sock_, &r, 1, 0);    // Receive a char
		
        if      (rStatus == CSOCKET_END)    return result;
        else if (rStatus == CSOCKET_ERROR)  return "";

        result += r;                    // Append to result
        if (r == '\n')	return result;  // If newline, return result
    }
    return result;                      // Fallback return
}

// ---------- Transmit data ---------------------------------------------------
void cSocket::txData(char* data, int size) {
    send(sock_, data, size, 0);         // Send data on socket
}

// ---------- Transmit a single line ------------------------------------------
void cSocket::txLine(std::string line) {
    line += "\n";                                   // Append newline char
    send(sock_, line.c_str(), line.length(), 0);    // Send line on socket
}