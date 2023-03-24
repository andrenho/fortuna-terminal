#include "tcpip.hh"
#include "common/exceptions/libcexception.hh"
#include "common/exceptions/fortunaexception.hh"

#ifdef _WIN32
#  include <winsock2.h>
#  include <wspiapi.h>
#else
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <unistd.h>
#  include <ifaddrs.h>
#  include <net/if.h>
#  include <arpa/inet.h>
#  define INVALID_SOCKET -1
#endif


#define BACKLOG 1

using namespace std::string_literals;

TCPIP::TCPIP(TcpIpOptions const& options, size_t readbuf_sz)
    : CommFileDescriptor(readbuf_sz), port_(options.port)
{
#ifdef _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
        throw FortunaException("WSAStartup failed: "s + std::to_string(iResult));
#endif
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv;
    struct addrinfo* servinfo;
    if ((rv = getaddrinfo(nullptr, std::to_string(options.port).c_str(), &hints, &servinfo)) != 0)
        throw FortunaException("Error getting addrinfo: "s + gai_strerror(rv));

    // find local address
    listen_address_ = find_listen_ipv4_addr();

    // loop through all the results and bind to the first we can

    struct addrinfo* p;
    for (p = servinfo; p != nullptr; p = p->ai_next) {
        if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET)
            throw LibcException("Error opening socket");

        int yes = 1;
        if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &yes, sizeof(int)) == -1)
            throw LibcException("Error setting socket options_");

        if (bind(sock_fd, p->ai_addr, (int) p->ai_addrlen) == -1) {
#ifdef _WIN32
            closesocket(sock_fd);
#else
            close(sock_fd);
#endif
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == nullptr)
        throw FortunaException("Failed to bind");

    if (listen(sock_fd, BACKLOG) == -1)
        throw LibcException("Error on listening");
}

std::string TCPIP::description() const
{
    // TODO - add address
    return "TCP/IP (listening on " + listen_address_ + ":" + std::to_string(port_) + ")";
}

std::string TCPIP::find_listen_ipv4_addr()
{
    std::string address = "127.0.0.1";

    char ipstr[INET_ADDRSTRLEN];

#ifdef _WIN32
    // Get local host name
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
                address = ipstr;
                break;
            }
        }
    }

    freeaddrinfo(res);
#else
    // Get local IP addresses
    struct ifaddrs *addrs, *tmp;
    getifaddrs(&addrs);

    // Loop through the addresses and print out the first private IPv4 address
    for (tmp = addrs; tmp != NULL; tmp = tmp->ifa_next) {
        if (tmp->ifa_addr && (tmp->ifa_flags & IFF_UP) && (tmp->ifa_flags & IFF_RUNNING)) {
            if (tmp->ifa_addr->sa_family == AF_INET) {
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)tmp->ifa_addr;
                void *addr = &(ipv4->sin_addr);
                inet_ntop(AF_INET, addr, ipstr, sizeof ipstr);
                if (strncmp(ipstr, "10.", 3) == 0 ||
                    strncmp(ipstr, "192.168.", 8) == 0 ||
                    strncmp(ipstr, "172.", 4) == 0) {
                    address = ipstr;
                    break;
                }
            }
        }
    }

    freeifaddrs(addrs);
#endif

    return ipstr;
}
