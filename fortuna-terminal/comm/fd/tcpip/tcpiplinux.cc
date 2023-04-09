#include "tcpipunix.hh"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <net/if.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#define INVALID_SOCKET -1

#include "common/exceptions/libcexception.hh"

std::string TCPIP_Unix::find_listen_ipv4_addr()
{
    char ipstr[INET_ADDRSTRLEN];
    strcpy(ipstr, "127.0.0.1");

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
                    break;
                }
            }
        }
    }

    freeifaddrs(addrs);

    return ipstr;
}

void TCPIP_Unix::configure_socket(SOCKET fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    // set write timeout to be 1 second
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0)
        throw LibcException("Error setting socket options");
}

void TCPIP_Unix::close_socket(SOCKET fd)
{
    close(fd);
}

bool TCPIP_Unix::error_is_try_again() const
{
    return errno == EAGAIN;
}

int TCPIP_Unix::write_flags() const
{
    return MSG_NOSIGNAL;
}
