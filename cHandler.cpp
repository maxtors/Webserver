#include "Webserver.h"
#include <fstream>
#include <string>
#include <map>
#include <sstream>

// ---------- STATIC EXTERNALS ------------------------------------------------
string_map Webserver::contenttypes;
string_map Webserver::statuscodes;
string_map Webserver::statuspages;
string_map Webserver::routes;

// ---------- cHandler Constructor --------------------------------------------
cHandler::cHandler(cSocket* s) {
    std::string line;                               // For holding request
    page.data.content   = NULL;                     // Set data content to null
    page.sock_          = s;                        // Set the socket
    line                = page.sock_->rxLine();     // Receive a line

    if (!line.empty() && line.find("GET") == 0) {   // If GOOD request
        createPage(line);                           // Create a page

        std::cout << "[" << page.status_ << "] - "  // Show some information
                  << page.path_ << " ("             // about response
                  << page.contentType_ << ")\n";

        sendPage();                                 // Send the page
    }
    else {                                          // If BAD request
        // send some kind of error message
    }
}

// ---------- cHandler Deconstructor ------------------------------------------
cHandler::~cHandler() {
    delete [] page.data.content;                    // Delete allocated content
}

// ---------- Send a finnished page to the client -----------------------------
void cHandler::sendPage() {
    std::stringstream ss_size;                      // Stringstream
    ss_size << page.data.size;                      // Size(int) to string

    // Transmit the header (status, connection, type, length)
    // Then transmit the data itself...
    page.sock_->txLine(Webserver::statuscodes[page.status_] + "\r");
    page.sock_->txLine("Connection: close\r");
    page.sock_->txLine("Content-Type: " + page.contentType_ + "\r");
    page.sock_->txLine("Content-Length: " + ss_size.str() + "\r");
    page.sock_->txLine("\r");
    page.sock_->txData(page.data.content, page.data.size);
}

// ---------- Create a Page from GET Request ----------------------------------
void cHandler::createPage(std::string l) {
    page.path_ = parsePath(l);                      // Set the PATH
    readData(page.path_);                           // Get file content

    if (page.data.content != NULL) {                // If the data was found
        page.status_        = "200";                        // Set status OK
        page.contentType_   = parseContentType(page.path_);	// Content type
    }
    else {                                          // Data was not found
        if (page.path_ == "favicon.ico") {          // If request for favicon
            page.status_    = "204";                // No Content
        }
        else {
            page.status_   = "404";                 // Set to 404
            page.path_     = "404.html";            // Set path
        }

        constData(page.status_);                    // Read const Page
        page.contentType_ = "text/html";            // Always same
    }
}

// ---------- cHandler: get path from REQUEST ----------------------------------
std::string cHandler::parsePath(std::string l) {

    std::cout << l << "\n";

    std::string::size_type start, stop;
    std::string result;
    string_map::iterator it;

    start   = l.find_first_of(" ");                     // Start PATH
    stop    = l.find(" ", start + 1);                   // End of PATH
    result  = l.substr((start + 1), (stop-start - 1));  // Get PATH

    if (result == "/")	result = "index.html";                       // Index
    else                result = l.substr((start+2),(stop-start-2)); // File

    it = Webserver::routes.find(result);
    if (it != Webserver::routes.end()) {
        result = it->second;
    }

    return result;                                      // Return PATH
}

// ---------- cHandler: Get content type from PATH ----------------------------
std::string cHandler::parseContentType(std::string p) {
    std::string temp, type = "";
    string_map::iterator it;
    std::string::size_type start, stop = p.length();

    start   = p.find_last_of(".");
    temp    = p.substr(start+1, stop-1);

    it = Webserver::contenttypes.find(temp);
    if (it != Webserver::contenttypes.end())    type = it->second;
    else                                        type = "application/unknown";

    return type;
}

// ---------- cHandler: READ the wanted content data from PATH ----------------
void cHandler::readData(std::string f) {
    int end, begin;                     // Start and stop of file

    page.data.size      = 0;            // Set size to zero
    page.data.content   = NULL;         // Set content char* to null

    std::ifstream file(f.c_str(), std::ios::binary);    // Create filestream

    if (file) {                         // If the file can be opend
        file.seekg(0, std::ios::beg);   // Seek to start of file
        begin = (int)file.tellg();      // Get position

        file.seekg(0, std::ios::end);   // Seek to end of file
        end = (int)file.tellg();        // Get position
        page.data.size = (end - begin);	// Calculate file size
        file.seekg(0, std::ios::beg);   // Seek back to start

        if (page.data.content != NULL) {        // Delete memory if allready
            delete [] page.data.content;        // allocated
        }
        page.data.content = new char[page.data.size + 1];   // Allocate new mem
        file.read(page.data.content, page.data.size);       // Read the content
        page.data.content[page.data.size] = '\0';           // Terminate array
    }
    else {
        // some kind of error message
    }
}

// ---------- Get wanted content data from constant data ----------------------
void cHandler::constData(std::string s) {
    string_map::iterator it;                    // String map itterator

    it = Webserver::statuspages.find(s);        // Try to find wanted status
    if (it != Webserver::statuspages.end()) {   // If it is not at end of map
        page.data.size = it->second.size();     // Set content size

        if (page.data.content != NULL) {        // Delete memory if allready
            delete [] page.data.content;        // allocated
        }
        page.data.content = new char[page.data.size + 1];   // Allocate new mem
        strcpy(page.data.content, it->second.c_str());      // Copy content
    }
    else {
        // Some kind of error
    }
}

// ---------- cHandler: Parse data (dynamic content) --------------------------
void cHandler::parseData(char* d) {
    // ...
}