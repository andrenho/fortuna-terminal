#include "ftclient-tcpip.h"

#ifdef _WIN32
#  include <fcntl.h>
#  include <sys/types.h>
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

static SOCKET connect_to_terminal(const char* address, int port, char* error, size_t err_sz)
{
#ifdef _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        snprintf(error, err_sz, "WSAStartup failed: %d", iResult);
        return INVALID_SOCKET;
    }
#endif
    char s_port[16];
    snprintf(s_port, sizeof(s_port), "%d", port);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(address, s_port, &hints, &peer_address)){
        snprintf(error, err_sz, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return INVALID_SOCKET;
    }

    /*
    struct sockaddr_in *ipv4 = (struct sockaddr_in *) peer_address->ai_addr;
    char ip_address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipv4->sin_addr), ip_address, INET_ADDRSTRLEN);
    printf("IP address: %s\n", ip_address);
    */

    SOCKET fd = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
    if (!ISVALIDSOCKET(fd)){
        snprintf(error, err_sz, "socket() failed (%d).\n", GETSOCKETERRNO());
        return INVALID_SOCKET;
    }

    if (connect(fd, peer_address->ai_addr, peer_address->ai_addrlen)){
        snprintf(error, err_sz, "connect() failed (%d).\n", GETSOCKETERRNO());
        return INVALID_SOCKET;
    }

    freeaddrinfo(peer_address);

    /*
#ifdef _WIN32
    u_long iMode = 1;
    ioctlsocket(fd, FIONBIO, &iMode);
#else
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif
    */

    return fd;
}

static int write_cb(const char* buf, size_t bufsz, void* data)
{
    return send((SOCKET) data, buf, (int) bufsz, 0);
}

static int read_cb(char* buf, size_t bufsz, void* data)
{
    return recv((SOCKET) data, buf, (int) bufsz, 0);
}

static int ftclient_tcpip_finalize(FTClient* ft_client, void* data)
{
    (void) data;
#ifdef _WIN32
    return closesocket((SOCKET) ft_client->data);
#else
    return close((SOCKET) ft_client->data);
#endif
}

int ftclient_tcpip_init(FTClient* ft_client, const char* address, int port, char* error, size_t err_sz)
{
    SOCKET fd = connect_to_terminal(address, port, error, err_sz);
    if (fd == INVALID_SOCKET)
        return -1;
    return ftclient_init(ft_client, write_cb, read_cb, ftclient_tcpip_finalize, (void *) fd, FT_RECOMMENDED_BUFSZ);
}