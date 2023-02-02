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

#include "tcpip.hh"

#include <thread>
#include <fcntl.h>

#include "../global.hh"

void TCPIP::initialize()
{
#if _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        error_message("WSAStartup failed: " + std::to_string(iResult), false);
        return;
    }
#endif
    struct addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv;
    struct addrinfo* servinfo;
    if ((rv = getaddrinfo(nullptr, options.tcpip.port.c_str(), &hints, &servinfo)) != 0) {
        error_message(std::string("Error getting addrinfo: ") + gai_strerror(rv), false);
        return;
    }

    // loop through all the results and bind to the first we can

    struct addrinfo* p;
    for ( p = servinfo; p != nullptr; p = p->ai_next) {
        if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET) {
            error_message("socket failed: ", true);
            continue;
        }

        int yes = 1;
        if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &yes, sizeof(int)) == -1) {
            error_message("setsockopt failed: ", true);
            return;
        }

        if (bind(sock_fd, p->ai_addr, (int) p->ai_addrlen) == -1) {
#if _WIN32
            closesocket(sock_fd);
#else
            close(sock_fd);
#endif
            error_message("bind error: ", true);
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == nullptr)  {
        error_message("server: failed to bind\n", false);
        return;
    }

    if (listen(sock_fd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
}

void TCPIP::run_input_from_device_thread()
{
    struct sockaddr_storage client_addr {};
    socklen_t sin_size;

    while (running_) {

        if (!client_connected) {

            // menage new connetions here

            sin_size = sizeof client_addr;
            client_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &sin_size);
            if (client_fd == INVALID_SOCKET) {
                error_message("server: failed to accept: ", true);
            }
            client_connected = true;
            
            if (options.debug_mode)
                std::cout << "Client connected.\n";

        } else {

            // read data from client

            uint8_t c;
            int n = recv(client_fd, reinterpret_cast<char *>(&c), 1, 0);
            if (n == 0) {
                if (options.debug_mode)
                    std::cout << "Client disconnected.\n";
                client_connected = false;
            } else if (n < 0) {
                error_message("Failure reading from client", true);
            } else {
                debug_received_byte(c);
                protocol->input_char(c);
            }
        }
    }
}

void TCPIP::run_output_to_device_thread()
{
    while (running_) {
        uint8_t c = output_queue.dequeue_block();
        if (client_connected) {
            int n = send(client_fd, reinterpret_cast<char const *>(&c), 1, 0);
            if (n == 0) {
                if (options.debug_mode)
                    std::cout << "Client disconnected.\n";
                client_connected = false;
            } else if (n < 0) {
                error_message("Failure writing to client", true);
            } else {
                debug_sent_byte(c);
            }
        }
    }
}

void TCPIP::finalize()
{
    CommunicationModule::finalize();
}
