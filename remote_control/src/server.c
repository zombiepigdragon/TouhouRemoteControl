#include "server.h"
#include "remote_control.h"
#include <ws2tcpip.h>

int server_setup() // TODO: Log errors here
{
    WSADATA wsaData; // This is never used, but it's needed for the WSAStartup call
    return !WSAStartup(MAKEWORD(2,2), &wsaData); // Require version 2.2
}

int server_cleanup()
{
    return WSACleanup();
}

SOCKET create_listening_socket(const char* port)
{
    // Configure our request for the port
    struct addrinfo *result = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;  //Use IPv4
    hints.ai_socktype = SOCK_STREAM;    //Stream socket type
    hints.ai_protocol = IPPROTO_TCP;    //TCP Protocol
    hints.ai_flags = AI_PASSIVE;    //Use for bind() later

    // Request the port
    if (getaddrinfo(NULL, port, &hints, &result) != 0) // Sets address to a valid type
    {
        return INVALID_SOCKET;
    }

    // Bind a socket to the port
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
    freeaddrinfo(result); // The configuration is now used and done with

    // Make the socket listen
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        return INVALID_SOCKET;
    }
    
    return listenSocket;
}

SOCKET accept_client(SOCKET listeningSocket) // FIXME: Blocks, make nonblocking
{
    TIMEVAL maxTime;
    maxTime.tv_sec = 0;
    maxTime.tv_usec = 0; // Make the call nonblocking
    fd_set fdset;
    FD_SET(listeningSocket, &fdset);
    int result = select(0, &fdset, NULL, NULL, &maxTime);
    switch (result)
    {
        case SOCKET_ERROR:
            return SOCKET_ERROR;
        case 0: // The number of connections available is the return value
            return INVALID_SOCKET;
        default:
            return accept(listeningSocket, NULL, NULL);
    }
}

int handle_connection(SOCKET clientSocket, struct SharedData* sharedData)
{
    const int bufferLen = 512;
    int bytesRead;
    char buffer[bufferLen];
    if (!(bytesRead = read_from_socket(clientSocket, buffer, bufferLen)))
    {
        remote_log_puts("Failed to read from socket!");
        closesocket(clientSocket);
        return 1;
    }
    // TODO: Use read information to provide requested information and allow modification of data
    char* output = json_dumps(sharedData->output, JSON_COMPACT);
    if (write_to_socket(clientSocket, output, strlen(output)) != 0)
    {
        remote_log_puts("Failed to write to socket!");
        closesocket(clientSocket);
        return 2;
    }
    closesocket(clientSocket);
    return 0;
}

int read_from_socket(SOCKET socket, char* buffer, int bufferLen)
{
    return recv(socket, buffer, bufferLen, 0);
}

int write_to_socket(SOCKET socket, char* buffer, int bufferLen)
{
    int bytesSent = 0;
    while (bytesSent < bufferLen)
    {
        int sendResult = send(socket, buffer + bytesSent, bufferLen, 0);
        if (sendResult == SOCKET_ERROR)
        {
            return -1;
        }
        bytesSent += sendResult;
    }
    return 0;
}