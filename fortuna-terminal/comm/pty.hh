#ifndef PTY_HH_
#define PTY_HH_

#include "comm_module.hh"
#include "../event/outputevent.hh"
#include "../event/inputevent.hh"
#include "../protocol/protocol.hh"

class PTY : public CommunicationModule {
public:
    PTY(OutputQueue& output_queue, InputQueue& input_queue, Protocol& protocol, PTYOptions const& pty_options)
        : CommunicationModule(output_queue, input_queue, protocol), pty_options_(pty_options) {}

    void initialize() override;
    void finalize() override;

    void run_input_from_device_thread() override;
    void run_output_to_device_thread() override;

private:
    PTYOptions const &pty_options_;
    int master_fd = 0;

    void client_disconnected();
};

#endif //PTY_HH_
