#pragma once
#include "remote_control.h"
#include <winsock2.h>

// States the server might be in
enum server_state
{
    // The server is operating normally
    SERVER_SUCCESS = 0,
    // The server is still starting
    SERVER_STARTING,
    // The server thread failed to start
    SERVER_THREAD_FAIL,
    // The server could not open the port
    SERVER_PORT_FAIL,
    // The server should begin its cleanup/shutdown process
    SERVER_SHUTTINGDOWN,
    // The server has performed cleanup and exited
    SERVER_SHUTDOWN
};

// A wrapper around WaitForSingleObject, makes it cleaner to write
// `if (lock_server_mutex()) {...} else {return ERROR;} unlock_server_mutex();`
int lock_server_mutex();
// To match lock_server_mutex
int unlock_server_mutex();

// Creates a socket on the specified port that is ready to accept connections
SOCKET create_listening_socket(const char* port);

// Blocks until a client appears or the timeout happens
SOCKET accept_client(SOCKET listeningSocket);

// Reads bufferLen bytes from socket to buffer
// Returns number of bytes read or -1 on error
int read_from_socket(SOCKET socket, char* buffer, int bufferLen);
// Writes bufferLen bytes from buffer to socket
// Returns 0 on success, anything else on error
int send_to_socket(SOCKET socket, char* buffer, int bufferLen);

// The function ran directly by the server thread
DWORD WINAPI server_threadmain(LPVOID lpdwThreadParam);

// Create a thread to run the server, and open the server
int server_initialize(const char* port, struct GameData* data);
int server_shutdown();