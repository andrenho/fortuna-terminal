#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#  include <winsock2.h>
#  include <wspiapi.h>
#include <unistd.h>

#  define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#  define CLOSESOCKET(s) closesocket(s)
#  define GETSOCKETERRNO()(WSAGetLastError())
#else
#  include <arpa/inet.h>
#  include <ifaddrs.h>
#  include <fcntl.h>
#  include <net/if.h>
#  include <netdb.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <unistd.h>
#  define ISVALIDSOCKET(s) ((s) >= 0)
#  define CLOSESOCKET(s) close(s)
#  define GETSOCKETERRNO()(errno)
#  define SOCKET int
#endif

#include "../ft-client.h"

static SOCKET connect_to_terminal(const char* address)
{
#ifdef _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        fprintf(stderr, "WSAStartup failed: %d", iResult);
        exit(1);
    }
#endif
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(address, "8076", &hints, &peer_address)){
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        exit(1);
    }

    SOCKET fd = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
    if (!ISVALIDSOCKET(fd)){
        fprintf(stderr, "socket() failed (%d).\n", GETSOCKETERRNO());
        exit(1);
    }

    if (connect(fd, peer_address->ai_addr, peer_address->ai_addrlen)){
        fprintf(stderr, "connect() failed (%d).\n", GETSOCKETERRNO());
        exit(1);
    }

    freeaddrinfo(peer_address);

    return fd;
}

int write_cb(const char* buf, size_t bufsz, void* data)
{
    return send((SOCKET) data, buf, (int) bufsz, 0);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s IP_ADDRESS\n", argv[0]);
        exit(1);
    }

    SOCKET fd = connect_to_terminal(argv[1]);

    FTClient ft;
    ftclient_init(&ft, write_cb, NULL, (void *) fd, FT_RECOMMENDED_BUFSZ);
    ft_print(&ft, "Hello world!");

    sleep(2);
}