#pragma once
#include <winsock2.h>
#include "shared_data.h"

// Does any system initialization that may be needed
int server_setup();
// Shutdown winsock
int server_cleanup();

// Creates a socket on the specified port that is ready to accept connections
SOCKET create_listening_socket(const char* port);

// Accepts a client or returns INVALID_SOCKET if no client is connecting
SOCKET accept_client(SOCKET listeningSocket);

// Do any and all processing needed once a client socket connects
int handle_connection(SOCKET clientSocket, struct SharedData* sharedData);

// Reads bufferLen bytes from socket to buffer
// Returns number of bytes read or -1 on error
int read_from_socket(SOCKET socket, char* buffer, int bufferLen);
// Writes bufferLen bytes from buffer to socket, accounting for buffering
// Returns 0 on success, anything else on error
int write_to_socket(SOCKET socket, char* buffer, int bufferLen);