#ifndef COMM_MODULE_HH_
#define COMM_MODULE_HH_

#include <memory>
#include <vector>

#include "../options.hh"
#include "../util/sync_queue.hh"
#include "../protocol/protocol.hh"

class CommunicationModule
{
public:
    virtual ~CommunicationModule() = default;

    virtual void initialize() {}
    virtual void run_input_from_device_thread([[maybe_unused]] Protocol*, InputQueue*) {}
    virtual void write_to_device(std::vector<uint8_t>) {}
    virtual void finalize() { running_ = false; };

    virtual void notify_vsync();

    static std::unique_ptr<CommunicationModule> make_communication_module(Options const& options);

protected:
    bool running_ = true;
};

#endif //COMM_MODULE_HH_
