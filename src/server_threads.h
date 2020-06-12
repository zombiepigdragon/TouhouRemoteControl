#pragma once
#include <winsock2.h>
#include "shared_data.h"

// States the server might be in
enum server_state
{
    // The server is operating normally
    SERVER_SUCCESS,
    // The server is still starting
    SERVER_STARTING,
    // The server failed and should be shut down
    SERVER_FAIL,
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

// The function ran directly on the server thread
DWORD WINAPI server_threadmain(LPVOID lpdwThreadParam);

// Create a thread to run the server, and open the server
int server_initialize(struct SharedData* data);
// Handles cleanly exiting the thread
int server_shutdown();