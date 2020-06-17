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
    return accept(listeningSocket, NULL, NULL);
}

int handle_connection(SOCKET clientSocket, struct SharedData* sharedData)
{
    const int bufferLen = 512;
    int bytesRead;
    char buffer[512];
    if (!(bytesRead = read_from_socket(clientSocket, buffer, bufferLen)))
    {
        log_print("Failed to read from socket!");
        closesocket(clientSocket);
        return 1;
    }
    if (!write_to_socket(clientSocket, buffer, bytesRead))
    {
        log_print("Failed to write to socket!");
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
    int sendResult = send(socket, buffer, bufferLen, 0);
    if (sendResult == SOCKET_ERROR || sendResult != bufferLen)
    {
        return -1;
    }
    return 0;
}