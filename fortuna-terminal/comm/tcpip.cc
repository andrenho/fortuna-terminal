#include "tcpip.hh"
#include "../exceptions/fortunaexception.hh"
#include "../exceptions/libcexception.hh"

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

#define BACKLOG 1

using namespace std::string_literals;

TCPIP::TCPIP(TcpIpOptions const &options)
{
#if _WIN32
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

    // loop through all the results and bind to the first we can

    struct addrinfo* p;
    for (p = servinfo; p != nullptr; p = p->ai_next) {
        if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET)
            throw LibcException("Error opening socket");

        int yes = 1;
        if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &yes, sizeof(int)) == -1)
            throw LibcException("Error setting socket options");

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

    if (p == nullptr)
        throw FortunaException("Failed to bind");

    if (listen(sock_fd, BACKLOG) == -1)
        throw LibcException("Error on listening");
}

void TCPIP::action_on_rw_zero()
{
    // TODO
}
