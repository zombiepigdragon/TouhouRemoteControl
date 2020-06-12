#include "server_threads.h"
#include "server.h"
#include "remote_control.h"

// Lock with the mutex
enum server_state serverState;
HANDLE serverMutex = NULL;

int lock_server_mutex()
{
    switch(WaitForSingleObject(serverMutex, INFINITE))
    {
        case WAIT_OBJECT_0:
            return 1;
        default:
            return 0;
    }
}

int unlock_server_mutex()
{
    return ReleaseMutex(serverMutex);
}

DWORD WINAPI server_threadmain(LPVOID lpdwThreadParam)
{
    struct SharedData* sharedData = (struct SharedData*)lpdwThreadParam;
    if (!lock_server_mutex())
    {
        log_print("Server thread failed to find server mutex, aborting.");
        return 1;
    }
    if (!server_setup())
    {
        serverState = SERVER_FAIL;
        unlock_server_mutex();
        return 2;
    }
    SOCKET listenSocket = create_listening_socket(sharedData->port);
    if (listenSocket == INVALID_SOCKET)
    {
        log_printf("[Remote Control] Failed to open port %s, aborting.\n", sharedData->port);
        server_cleanup();
        serverState = SERVER_FAIL;
        unlock_server_mutex();
        return 3;
    }
    serverState = SERVER_SUCCESS;
    do
    {
        unlock_server_mutex();
        SOCKET clientSocket = accept_client(listenSocket);
        handle_connection(clientSocket, sharedData);
        lock_server_mutex();
    } while (serverState == SERVER_SUCCESS);
    closesocket(listenSocket);
    return server_cleanup();
}

int server_initialize(struct SharedData* data)
{
    if (serverMutex == NULL)
    {
        serverMutex = CreateMutex(NULL, FALSE, NULL);
    }
    if (!lock_server_mutex())
    {
        log_print("Failed to acquire a server mutex, aborting.");
        return 1;
    }
    // Check for a valid port
    for (int length = strlen(data->port), i = 0; i < length; i++)
    {
        char digit = data->port[i];
        if (digit < '0' || digit > '9')
        {
            log_printf("[Remote Control] The given port \"%s\" is invalid. Using default 8646.\n", data->port);
            data->port = "8646";
            break;
        }
    }
    serverState = SERVER_STARTING;
    if (!CreateThread(NULL, 0, server_threadmain, data, 0, NULL))
    {
        log_print("Couldn't create thread, aborting.");
    }
    do
    {
        unlock_server_mutex();
        Sleep(500);
        lock_server_mutex();
    } while(serverState == SERVER_STARTING);
    int result = serverState == SERVER_SUCCESS;
    unlock_server_mutex();
    return result;
}

int server_shutdown()
{
    if (!lock_server_mutex())
    {
        return 1;
    }
    serverState = SERVER_SHUTTINGDOWN;
    do
    {
        unlock_server_mutex();
        Sleep(500);
        lock_server_mutex();
    } while(serverState == SERVER_SHUTTINGDOWN);
    int result = serverState == SERVER_SHUTDOWN;
    unlock_server_mutex();
    return result;
}