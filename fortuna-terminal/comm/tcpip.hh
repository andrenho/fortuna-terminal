#ifndef TCPIP_HH_
#define TCPIP_HH_

#include "comm_module.hh"

#if _WIN32
#include <winsock.h>
#endif

class TCPIP : public CommunicationModule {
public:
    explicit TCPIP(OutputQueue& output_queue, InputQueue& input_queue, Protocol& protocol, TCPIPOptions const& tcpip_options)
        : CommunicationModule(output_queue, input_queue, protocol), tcpip_options_(tcpip_options) {}

    void initialize(size_t lines, size_t columns) override;

    void run_input_from_device_thread() override;
    void run_output_to_device_thread() override;

    void finalize() override;

private:
    TCPIPOptions const &tcpip_options_;

#ifndef _WIN32
#define SOCKET int
#endif
    SOCKET sock_fd = 0;
    SOCKET client_fd = 0;

    static const int BACKLOG = 1;

    bool client_connected = false;
};

#endif //TCPIP_HH_
