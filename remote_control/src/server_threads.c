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
        remote_log_puts("Server thread failed to find server mutex, aborting.");
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
        remote_log_printf("[Remote Control] Failed to open port %s, aborting.\n", sharedData->port);
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
        if (clientSocket != INVALID_SOCKET)
        {
            if (clientSocket == SOCKET_ERROR)
            {
                log_print("Socket error, continuing.");
            }
            else
            {
                handle_connection(clientSocket, sharedData);
            }
        }
        lock_server_mutex();
    } while (serverState == SERVER_SUCCESS);
    closesocket(listenSocket);
    int result = server_cleanup();
    serverState = SERVER_SHUTDOWN;
    unlock_server_mutex();
    return result;
}

int server_initialize(struct SharedData* data)
{
    if (serverMutex == NULL)
    {
        serverMutex = CreateMutex(NULL, FALSE, NULL);
    }
    if (!lock_server_mutex())
    {
        remote_log_puts("Failed to acquire a server mutex, aborting.");
        return 1;
    }
    serverState = SERVER_STARTING;
    if (!CreateThread(NULL, 0, server_threadmain, data, 0, NULL))
    {
        remote_log_puts("Couldn't create thread, aborting.");
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