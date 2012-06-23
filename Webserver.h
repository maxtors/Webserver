#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef WEBSERVER_H
#define WEBSERVER_H

#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <string>
#include <map>
#include <WinSock2.h>

typedef std::map<std::string, std::string> string_map;

// ---------- WEBSERVER CLASS -------------------------------------------------
class Webserver {
private:
    static string_map contenttypes;     // Different content types
    static string_map statuscodes;      // Different status codes
    static string_map statuspages;      // The HTML code for status pages
    SOCKET sock_;                       // The servers SOCKET

    // ---------- WEBSERVER STATIC FUNCTIONS -------------------------------
    static void startWSA();             // Start Windows Socket API
    static void stopWSA();              // Stop  Windows Socket API
    // --------------------------------------------------------------------

    // ---------- CSOCKET CLASS -----------------------------------------------
    class cSocket {
    private:
        SOCKET	sock_;                  // The cScoket's SOCKET

    public:
        cSocket(SOCKET s);              // cSocket's constructor
        ~cSocket();                     // cSocket's deconstructor
		
        // ---------- CSOCKET PUBLIC FUNCTIONS --------------------------------
        void        close();                        // Close a connection
        std::string rxData();                       // Receive data
        std::string rxLine();                       // Receive a line
        void        txData(char* data, int size);   // Transmit data
        void        txLine(std::string line);       // Transmit a line
    };
    // ------------------------------------------------------------------------

    // ---------- CHANDLER CLASS ----------------------------------------------
    class cHandler {
    private:
        // ---------- PAGE STRUCT ---------------------------------------------
        struct Page {
            cSocket*    sock_;                      // The connected cSocket
            std::string path_;                      // Path of the request
            std::string status_;                    // "Not found" or "OK"
            std::string contentType_;               // The type of content

            // ---------- DATA STRUCT -----------------------------------------
            struct Data {
                int     size;                       // Number of bytes
                char*   content;                    // The content itself
            } data;
            // ----------------------------------------------------------------
        } page;
        // --------------------------------------------------------------------

		// ---------- CHANDLER PRIVATE FUNCTIONS ------------------------------
        std::string parsePath(std::string l);           // Get the wanted PATH
        std::string parseContentType(std::string p);    // Set content type
        void        readData(std::string f);            // Read wanted content
        void        constData(std::string s);           // Read const data
        void        createPage(std::string l);          // Create page from REQ
        void        sendPage();                         // Send a page
        void        parseData(char* d);                 // Get requested file
        void        setPage404(Page::Data &p);          // Set page to 404
        // --------------------------------------------------------------------

    public:
        cHandler(cSocket* s);                       // cHandlers constructor
        ~cHandler();                                // cHandlers deconstructor
    };
    // ------------------------------------------------------------------------

    // ---------- WEBSERVER PRIVATE FUNCTIONS ---------------------------------
    static unsigned __stdcall   Request(void* ptrSock); // Thread a new request
    cSocket*                    Accept();               // Accept a new Socket
    string_map                  readMap(std::string f); // Read file to MAP
	// ------------------------------------------------------------------------

public:
    Webserver(short port);                          // Webservers constructor
    ~Webserver();                                   // Webservers deconstructor
};
#endif