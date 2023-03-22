#ifndef COMM_HH_
#define COMM_HH_

#include <memory>

#include "application/options.hh"
#include "common/syncqueue.hh"
#include "common/types/noncopyable.hh"

class CommunicationModule : NonCopyable {
public:
    virtual ~CommunicationModule() = default;

    virtual void                   execute_threads(SyncQueueByte* input_queue_, SyncQueueByte* output_queue_, bool debug_comm) = 0;
    virtual void                   notify_threads() {}
    virtual void                   finalize_threads() = 0;

    static std::unique_ptr<CommunicationModule> create(Options const& options);

    [[nodiscard]] virtual std::string description() const = 0;

    [[nodiscard]] virtual bool is_overwhelmed() const { return false; }

protected:
    void debug_byte(uint8_t byte, bool is_input);

private:
    std::mutex debug_mutex_ {};
};

using CommUniquePtr = std::unique_ptr<CommunicationModule>;

#endif //COMM_HH_
