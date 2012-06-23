#include "Webserver.h"
#include <fstream>
#include <string>
#include <map>
#include <sstream>

string_map Webserver::contenttypes;					// Static string map

// ---------- cHandler Constructor --------------------------------------------
Webserver::cHandler::cHandler(cSocket* s) {
	std::string line;								// For holding request

	page.sock_	= s;								// Set the socket
	line		= page.sock_->rxLine();				// Receive a line

	if (!line.empty() && line.find("GET") == 0) {	// If GOOD request
		createPage(line);							// Create a page

		std::cout << "[" << page.status_ << "] - "	// Show some information
				  << page.path_ << " ("				// about response
				  << page.contentType_ << ")\n";

		sendPage();									// Send the page
	}
	else {											// If BAD request
		// send some kind of error message
	}
}

// ---------- cHandler Deconstructor ------------------------------------------
Webserver::cHandler::~cHandler() {
	delete [] page.data.content;					// Delete allocated content
}

// ---------- Send a finnished page to the client -----------------------------
void Webserver::cHandler::sendPage() {
	std::stringstream ss_size;						// Stringstream
	ss_size << page.data.size;						// Size(int) to string

	// Transmit the header (status, connection, type, length)
	// Then transmit the data itself...
	page.sock_->txLine("HTTP/1.1 " + page.status_ + "\r");
	page.sock_->txLine("Connection: close\r");
	page.sock_->txLine("Content-Type: " + page.contentType_ + "\r");
	page.sock_->txLine("Content-Length: " + ss_size.str() + "\r");
	page.sock_->txLine("\r");

	if (page.status_ != "204 No Content") {
		page.sock_->txData(page.data.content, page.data.size);
	}
}

// ---------- Create a Page from GET Request ----------------------------------
void Webserver::cHandler::createPage(std::string l) {
	page.path_			= parsePath(l);					// Set the PATH
	page.data			= readData(page.path_);			// Get file content

	if (page.data.content == NULL) {					// IF no data read
		if (page.path_ == "favicon.ico") {
			page.status_	= "204 No Content";
		}
		else {
			page.status_	= "404 Not Found";				// Set to 404
			page.path_		= "404.html";					// Set path
			page.data		= readData(page.path_);			// Read 404 Page
		}

		page.contentType_	= "text/html";
	}
	else {
		page.status_		= "200 OK";						// Set to OK
		page.contentType_	= parseContentType(page.path_);	// Content type
	}
}

// ---------- cHandler: get path from REQUEST ----------------------------------
std::string Webserver::cHandler::parsePath(std::string l) {
	std::string::size_type start, stop;
	std::string result;

	start	= l.find_first_of(" ");						// Start PATH
	stop	= l.find(" ", start + 1);					// End of PATH
	result	= l.substr((start + 1), (stop-start - 1));	// Get PATH

	if (result == "/")	result = "index.html";						 // Index
	else				result = l.substr((start+2),(stop-start-2)); // File

	return result;												// Return PATH
}

// ---------- cHandler: Get content type from PATH ----------------------------
std::string Webserver::cHandler::parseContentType(std::string p) {
	std::string temp, type = "";
	string_map::iterator it;
	std::string::size_type start, stop = p.length();

	start	= p.find_last_of(".");
	temp	= p.substr(start+1, stop-1);

	it = Webserver::contenttypes.find(temp);
	if (it != Webserver::contenttypes.end())	type = it->second;
	else										type = "application/unknown";

	return type;
}

// ---------- cHandler: READ the wanted content data from PATH ----------------
Webserver::cHandler::Page::Data Webserver::cHandler::readData(std::string f) {
	cHandler::Page::Data d;				// Temporary Page::Data struct object
	int end, begin;

	d.size		= 0;					// Set size to zero
	d.content	= NULL;					// Set content char* to null

	std::ifstream file(f.c_str(), std::ios::binary);	// Create filestream

	if (file) {							// If the file can be opend
		file.seekg(0, std::ios::beg);	// Seek to start of file
		begin = (int)file.tellg();		// Get position

		file.seekg(0, std::ios::end);	// Seek to end of file
		end = (int)file.tellg();		// Get position
		d.size = (end - begin);			// Calculate file size
		file.seekg(0, std::ios::beg);	// Seek back to start

		d.content = new char[d.size + 1];	// Allocate memory for content
		file.read(d.content, d.size);		// Read the content from file
		d.content[d.size] = '\0';			// Terminate the char array
	}
	else {
		// some kind of error message
	}
	return d;							// Return the content
}

// ---------- cHandler: Parse data (dynamic content) --------------------------
void Webserver::cHandler::parseData(char* d) {
	// ...
}