#include <process.h>
#include <WinSock2.h>
#include <fstream>
#include <sstream>
#include "Webserver.h"

// ---------- Webserver Constructor -------------------------------------------
Webserver::Webserver(short port) {
	try {

        // ---------- READ THE CONFIG FILES -----------------------------------
        contenttypes = readMap("config/contenttypes.dta");
        statuscodes  = readMap("config/statuscodes.dta");
        statuspages  = readMap("config/statushtml.dta");

        if      (contenttypes.empty())  throw "MISSING CONTENTTYPES";
        else if (statuscodes.empty())   throw "MISSING STATUSCODES";
        else if (statuspages.empty())   throw "MISSING STATUSPAGES";

        // ---------- INITIATE THE SERVER SOCKET ------------------------------
        Webserver::startWSA();                  // Initiate WSA
        sockaddr_in addr;

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.S_un.S_addr = INADDR_ANY;
        memset(addr.sin_zero, 0, 8);
        sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (sock_ == INVALID_SOCKET) throw "INVALID SOCKET";

        if (bind(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))!=0) {
            closesocket(sock_);
            throw "INVALID SOCKET";
        }

        listen(sock_, SOMAXCONN);               // Start to listen on socket

        // ---------- ACCEPT AND HANDLE NEW CONNECTIONS -----------------------
        unsigned ret;                           // Return value of thread
        while (true) {
            cSocket* s = this->Accept();                    // New connection
            _beginthreadex(0,0,Request,(void*) s,0,&ret);	// Start a thread
        }
    }
    catch (char* e) {                           // Catch an error message
        std::cout << e << "\n";                 // Display message
        system("pause");                        // Pause so user can view
    }
}

// ---------- Webserver Deonstructor ------------------------------------------
Webserver::~Webserver() 	{ Webserver::stopWSA(); }

// ---------- Stop WSA --------------------------------------------------------
void Webserver::stopWSA() 	{ WSACleanup(); }

// ---------- Create a Request Handler ----------------------------------------
unsigned Webserver::Request(void* ptrSock) {
    cHandler requestHandler((reinterpret_cast<cSocket*>(ptrSock)));
    return 0;
}

// ---------- Accept an incomming SOCKET --------------------------------------
cSocket* Webserver::Accept() {
    SOCKET newsocket = accept(sock_, 0, 0);     // Create a new socket...
    if (newsocket == INVALID_SOCKET) throw "INVALID SOCKET";
    cSocket* s = new cSocket(newsocket);
    return s;                                   // Return new cSocket
}

// ---------- Initiate WSA ----------------------------------------------------
void Webserver::startWSA() {
	WSADATA wsaData;                                    // WSA data struct

    if (WSAStartup(MAKEWORD(2,0), &wsaData) == 0) {     // Start WSA
        if (!(LOBYTE(wsaData.wVersion) >= 2)) {         // Check the version
            throw "REQUIRED VERSION NOT SUPPORTED";
        }
    }
	else {                                              // If error at startup
        throw "STARTUP FAILED";
    }
}

// ---------- Read filecontent to string map ----------------------------------
string_map Webserver::readMap(std::string f) {
    string_map vars;                        // String map for file content
    std::string tempKey, tempItem;          // Temp vars for map key and item
    std::ifstream file(f.c_str());          // input file stream object

    if (file) {                             // if the file could be open
        file >> tempKey;                    // Read the key value
        std::getline(file, tempItem, '\n'); // Read the item value
        tempItem = tempItem.substr(1, tempItem.length() - 1);
        while (!file.eof()) {

            // Add the found pair (key, item) to the "vars" string map
            vars.insert(std::pair<std::string, std::string>(tempKey,tempItem));
            file >> tempKey;                    // Read the key value
            std::getline(file, tempItem, '\n'); // Read the item value
            tempItem = tempItem.substr(1, tempItem.length() - 1);
        }
    }
    return vars;                                // return map
}