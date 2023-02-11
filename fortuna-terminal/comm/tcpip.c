#if _WIN32
#  include <winsock2.h>
#  include <wspiapi.h>
#else
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <unistd.h>
#  define INVALID_SOCKET -1
#endif

#include "../error/error.h"
#include "tcpip.h"

#ifndef _WIN32
#define SOCKET int
#endif
static SOCKET sock_fd = 0;
static SOCKET client_fd = 0;

#define BACKLOG 1

static bool client_connected = false;

#include <fcntl.h>
#include <string.h>
#include <stdio.h>

int tcpip_init()
{
#if _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        sprintf(error_message, "WSAStartup failed: %d", iResult);
        error_check(ERR_MESSAGE);
    }
#endif
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv;
    struct addrinfo* servinfo;
    char port[10];
    sprintf(port, "%d", options.tcpip.port);
    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        sprintf(error_message, "Error getting addrinfo: %s", gai_strerror(rv));
        error_check(ERR_MESSAGE);
    }

    // loop through all the results and bind to the first we can

    struct addrinfo* p;
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET)
            error_check(ERR_LIBC);

        int yes = 1;
        if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &yes, sizeof(int)) == -1)
            error_check(ERR_LIBC);

        if (bind(sock_fd, p->ai_addr, (int) p->ai_addrlen) == -1) {
#if _WIN32
            closesocket(sock_fd);
#else
            close(sock_fd);
#endif
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL)  {
        strcpy(error_message, "server: failed to bind");
        error_check(ERR_MESSAGE);
    }

    if (listen(sock_fd, BACKLOG) == -1)
        error_check(ERR_LIBC);

    return 0;
}

int tcpip_recv(uint8_t* byte)
{
    if (!client_connected) {

        // manage new connetions here

        struct sockaddr_storage client_addr;
        socklen_t sin_size = sizeof client_addr;
        client_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &sin_size);
        if (client_fd == INVALID_SOCKET)
            error_check(ERR_LIBC);
        client_connected = true;

        printf("Client connected.\n");

        return ERR_NO_DATA;

    } else {

        uint8_t c;
        int n = recv(client_fd, (char *) &c, 1, 0);
        if (n == 0) {
            printf("Client disconnected.\n");
            client_connected = false;
            return ERR_NO_DATA;
        } else if (n < 0) {
            error_check(ERR_LIBC);
        } else {
            *byte = c;
        }
    }

    return 0;
}

int tcpip_send(const uint8_t* data, size_t sz)
{
    if (client_connected) {
        int n = send(client_fd, (char *) data, (int) sz, 0);
        if (n == 0) {
            printf("Client disconnected.\n");
            client_connected = false;
            return ERR_NO_DATA;
        } else if (n < 0) {
            error_check(ERR_LIBC);
        }
    }
    return 0;
}

int tcpip_finalize()
{
    // TODO
    return 0;
}
