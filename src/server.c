#include "server.h"
#include <ws2tcpip.h>

// The current status of the server
enum server_state serverState;
// Mutex around both GameData and serverState
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

// Implementation based off of MSDN tutorial, should probably be checked
SOCKET create_listening_socket(const char* port)
{
    WSADATA wsaData; // This is never used, but it's needed for the WSAStartup call
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) // Require version 2.2
    {
        return INVALID_SOCKET;
    }

    // addrinfo defines protocols
    // hints is requests, result is filled by OS to be valid
    struct addrinfo *result = NULL, hints;  
    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;  //Use IPv4
    hints.ai_socktype = SOCK_STREAM;    //Stream socket type
    hints.ai_protocol = IPPROTO_TCP;    //TCP Protocol
    hints.ai_flags = AI_PASSIVE;    //Signal use for bind() later

    if (getaddrinfo(NULL, port, &hints, &result) != 0) // Make result valid
    {
        return INVALID_SOCKET;
    }

    SOCKET listenSocket = INVALID_SOCKET;
    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET)
    {
        freeaddrinfo(result);
        return INVALID_SOCKET;
    }

    if (bind(listenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
    {
        freeaddrinfo(result);
        closesocket(listenSocket);
        return INVALID_SOCKET;
    }
    freeaddrinfo(result);

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        return INVALID_SOCKET;
    }

    return listenSocket;
}

SOCKET accept_client(SOCKET listeningSocket)
{
    return accept(listeningSocket, NULL, NULL);
}

// Just a cleaner looking (IMO) recv
int read_from_socket(SOCKET socket, char* buffer, int bufferLen)
{
    return recv(socket, buffer, bufferLen, 0);
}

// Simplifies the return somewhat, so at least we're more useful than ReadFromSocket()
int send_to_socket(SOCKET socket, char* buffer, int bufferLen)
{
    int sendResult = send(socket, buffer, bufferLen, 0);
    if (sendResult == SOCKET_ERROR || sendResult != bufferLen)
    {
        return -1;
    }
    return 0;
}

DWORD WINAPI server_threadmain(LPVOID lpdwThreadParam) // FIXME: Put memory reading in main loop, clean this all up
{
    struct GameData* data = (struct GameData*)lpdwThreadParam;
    log_print("Server thread started!");
    const int bufferLen = 512;
    char buffer[bufferLen];
    SOCKET listenSocket = create_listening_socket("8646");
    SOCKET clientSocket = INVALID_SOCKET;
    if (listenSocket == INVALID_SOCKET)
    {
        if (lock_server_mutex())
        {
            log_print("Failed to create socket.");
            serverState = SERVER_PORT_FAIL;
            unlock_server_mutex();
            return SERVER_PORT_FAIL;
        }
        else
        {
            log_print("Failed to use a mutex, server closing.");
            serverState = SERVER_THREAD_FAIL; // Write without mutex, because we're screwed anyway
            return SERVER_THREAD_FAIL; // Return value is unimportant, this just matches types
        }
    }
    log_printf("[Remote Control] Opened socket on port 8646.\n");
    if (lock_server_mutex())
    {
        serverState = SERVER_SUCCESS;
    }
    else
    {
        log_print("Failed to use a mutex, server closing.");
        serverState = SERVER_THREAD_FAIL; // Write without mutex, because we're screwed anyway
        return SERVER_THREAD_FAIL; // Return value is unimportant, this just matches types
    }
    do 
    {
        unlock_server_mutex();
        if (clientSocket == INVALID_SOCKET)
        {
            clientSocket = accept_client(listenSocket);
            lock_server_mutex(); // Make sure we're in a clean state
            continue;
        }
        int readLength = read_from_socket(clientSocket, buffer, bufferLen);
        lock_server_mutex();
        if (!ReadProcessMemory(GetCurrentProcess(), data->values[RA_SCORE].game_address, &data->score, data->values[RA_SCORE].size, NULL))
        {
            log_printf("[Remote Control] Failed to read %d bytes from memory value 0x%p.\n", data->values[RA_SCORE].size, data->values[RA_SCORE].game_address);
        }
        readLength = snprintf(buffer, bufferLen, "%d", data->score * 10);
        send_to_socket(clientSocket, buffer, readLength);
        
    } while(serverState == SERVER_SUCCESS);
    closesocket(listenSocket);
    WSACleanup();
    if (lock_server_mutex())
    {
        serverState = SERVER_SHUTDOWN;
        unlock_server_mutex();
        return SERVER_SUCCESS;
    }
    else
    {
        return SERVER_THREAD_FAIL; // Why not
    }
}

int server_initialize(const char* port, struct GameData* data)
{
    serverState = SERVER_STARTING;
    serverMutex = CreateMutex(NULL, FALSE, NULL);
    HANDLE thread = CreateThread(NULL, 0, server_threadmain, data, 0, NULL);
    if (!thread)
    {
        return SERVER_THREAD_FAIL;
    }
    while (TRUE) // Loop until the server reports starting or an error
    {
        if (lock_server_mutex())
        {
            if (serverState != SERVER_STARTING)
            {
                unlock_server_mutex();
                return serverState;
            }
        }
        unlock_server_mutex();
    }
}

int server_shutdown()
{
    if (lock_server_mutex())
    {
        serverState = SERVER_SHUTTINGDOWN;
    }
    while (TRUE)
    {
        if (lock_server_mutex())
        {
            if (serverState != SERVER_SHUTTINGDOWN)
            {
                unlock_server_mutex();
                return serverState == SERVER_SHUTDOWN;
            }
        }
        unlock_server_mutex();
    }
}