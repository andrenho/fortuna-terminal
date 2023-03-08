#include "tcpip.hh"
#include "common/exceptions/fortunaexception.hh"
#include "common/exceptions/libcexception.hh"

#ifdef _WIN32
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

#include <iostream>

#define BACKLOG 1

using namespace std::string_literals;

TCPIP::TCPIP(TcpIpOptions const &options)
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

std::vector<uint8_t> TCPIP::read_blocking(size_t n)
{
    if (fd_ == 0) {   // client disconnected
        struct sockaddr_storage client_addr;
        socklen_t sin_size = sizeof client_addr;
        SOCKET fd = accept(sock_fd, (struct sockaddr *) &client_addr, &sin_size);
        if (fd == INVALID_SOCKET)
            on_read_error("Error on accept");
#ifdef _WIN32
        fd_ = (int) fd;
#else
        fd_ = fd;
#endif

        std::cout << "Client connected.\n";

        return {};

    } else {
        std::vector<uint8_t> data(n);
        int r = recv(fd_, (char *) data.data(), (int) n, 0);
        if (r < 0)
            on_read_error(strerror(errno));
        else if (r == 0)
            on_read_zero();
        else if (r < (int) n)
            data.resize(n);
        return data;
    }
}

void TCPIP::write(std::vector<uint8_t> const &data)
{
    if (fd_ == 0)
        return;

    int n = send(fd_, (const char *) data.data(), (int) data.size(), 0);
    if (n == 0)
        on_write_error(strerror(errno));
    else if (n < 0)
        on_write_zero();
}
