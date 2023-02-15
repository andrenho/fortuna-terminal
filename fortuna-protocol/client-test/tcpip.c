#if _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <unistd.h>
#  define SOCKET int
#  define INVALID_SOCKET -1
#endif

#include <stdio.h>
#include "../fortuna-protocol.h"

SOCKET fd = INVALID_SOCKET;

static int sendf(uint8_t const* buffer, size_t sz) {
    return send(fd, (const char *) buffer, (int) sz, 0);
}

static int recvf(uint8_t* buf, size_t sz) {
    return recv(fd, (char *) buf, (int) sz, 0);
}

int main()
{

#if _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d", iResult);
        return 0;
    }
#endif

    struct addrinfo hints, *result;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("localhost", "8076", &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    struct addrinfo *ptr;
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (fd == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( fd, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(fd);
            fd = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    FP_Message msg = {
            .command = FP_TEXT_PRINT_CHAR,
            .chr = 'A',
    };
    int error;
    fp_msg_send(&msg, sendf, recvf, &error);

#if _WIN32
    closesocket(fd);
#else
    close(fd);
#endif

    return 0;
}
