#include "tcpip.hh"
#include "common/exceptions/libcexception.hh"
#include "common/exceptions/fortunaexception.hh"

#ifdef _WIN32
#  include <winsock2.h>
#  include <wspiapi.h>
#else
#  include <arpa/inet.h>
#  include <ifaddrs.h>
#  include <fcntl.h>
#  include <net/if.h>
#  include <signal.h>
#  include <netdb.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <unistd.h>
#  define INVALID_SOCKET -1
#endif

#define BACKLOG 10

using namespace std::string_literals;

static constexpr const char* CLIENT_CONNECTED = "\e[0;32mClient connected.\e[0m\r\n\r\n";
static constexpr const char* CLIENT_DISCONNECTED = "\r\n\e[0;31mClient disconnected.\e[0m\r\n";

void TCPIP::initialize()
{
    initialize_tcpip();

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv;
    struct addrinfo* servinfo;
    if ((rv = getaddrinfo(nullptr, std::to_string(port_).c_str(), &hints, &servinfo)) != 0)
        throw FortunaException("Error getting addrinfo: "s + gai_strerror(rv));

    // find local address
    listen_address_ = find_listen_ipv4_addr();

    // loop through all the results and bind to the first we can
    struct addrinfo* p;
    for (p = servinfo; p != nullptr; p = p->ai_next) {

        if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET)
            throw LibcException("Error opening socket");

        int no = 0;
        if (setsockopt(sock_fd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no)) != 0)
            throw LibcException("Error setting socket options");

        int yes = 1;
        if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &yes, sizeof(int)) == -1)
            throw LibcException("Error setting socket options");

        configure_socket(sock_fd);

        if (bind(sock_fd, p->ai_addr, (int) p->ai_addrlen) == -1) {
            close_socket(sock_fd);
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

std::string TCPIP::read()
{
    if (fd_ == INVALID_FILE) {   // client disconnected
        struct sockaddr_storage client_addr;
        socklen_t sin_size = sizeof client_addr;
        SOCKET fd = accept(sock_fd, (struct sockaddr *) &client_addr, &sin_size);
        if (fd == INVALID_SOCKET) {
            if (error_is_try_again())
                return {};
            throw LibcException("accept");
        }

        configure_socket(fd);
        fd_ = fd;

        return CLIENT_CONNECTED;
    } else {
        std::string rd(readbuf_sz_, 0);
        int r = ::recv(fd_, rd.data(), readbuf_sz_, 0);
        if (r == -1) {
            if (error_is_try_again()) {
                r = 0;
            } else {
                client_disconnected();
                return CLIENT_DISCONNECTED;
            }
        } else if (r == 0) {
            client_disconnected();
            return CLIENT_DISCONNECTED;
        }

        rd.resize(r);
        return rd;
    }
}

void TCPIP::write(std::string_view data_to_send)
{
    int fd = this->write_fd_.value_or(this->fd_);
    if (fd != INVALID_FILE) {
        size_t left = data_to_send.size();
        do {
            int r = ::send(fd, data_to_send.data(), data_to_send.size(), write_flags());
            if (r == -1) {
                client_disconnected();
                return;
            } else if (r == 0) {
                client_disconnected();
                return;
            } else {
                left -= r;
            }
        } while (left > 0);
    }
}

std::string TCPIP::description() const
{
    return "TCP/IP (listening on " + listen_address_ + ":" + std::to_string(port_) + ")";
}

void TCPIP::vsync()
{
    write("\xfe");
}
