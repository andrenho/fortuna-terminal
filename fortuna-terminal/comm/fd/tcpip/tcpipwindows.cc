#include "tcpipwindows.hh"

#include <winsock2.h>
#include <wspiapi.h>

#include "common/exceptions/fortunaexception.hh"

using namespace std::string_literals;

void TCPIP_Windows::initialize_tcpip()
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
        throw FortunaException("WSAStartup failed: "s + std::to_string(iResult));
}

std::string TCPIP_Windows::find_listen_ipv4_addr()
{
    char ipstr[INET_ADDRSTRLEN];
    strcpy(ipstr, "127.0.0.1");

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        fprintf(stderr, "gethostname failed: %d\n", WSAGetLastError());
        exit(1);
    }

    // Get local IP addresses
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4 only
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        fprintf(stderr, "getaddrinfo failed: %d\n", WSAGetLastError());
        exit(1);
    }

    // Loop through the addresses and print out the first private IPv4 address
    for (p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            void *addr = &(ipv4->sin_addr);
            inet_ntop(AF_INET, addr, ipstr, sizeof ipstr);
            if (strncmp(ipstr, "10.", 3) == 0 ||
                strncmp(ipstr, "192.168.", 8) == 0 ||
                strncmp(ipstr, "172.", 4) == 0) {
                break;
            }
        }
    }

    freeaddrinfo(res);

    return ipstr;
}


void TCPIP_Windows::configure_socket(SOCKET fd)
{
    u_long mode = 1; // 1 for non-blocking mode, 0 for blocking mode
    ioctlsocket(fd, FIONBIO, &mode);
}

void TCPIP_Windows::close_socket(SOCKET fd)
{
    closesocket(fd);
}

bool TCPIP_Windows::error_is_try_again() const
{
    return WSAGetLastError() == WSAEWOULDBLOCK;
}

int TCPIP_Windows::write_flags() const
{
    return 0;
}
