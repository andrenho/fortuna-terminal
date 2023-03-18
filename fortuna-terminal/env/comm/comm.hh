#ifndef COMM_HH_
#define COMM_HH_

#include <memory>

#include "application/options.hh"
#include "common/syncqueue.hh"
#include "common/types/noncopyable.hh"

enum class Channels {
    InputAndOutput, Exchange,
};

class CommunicationModule : NonCopyable {
public:
    virtual ~CommunicationModule() = default;

    virtual bool release_locks() { return false; }

    [[nodiscard]] virtual Channels channels() const = 0;
    virtual void                   execute_threads(SyncQueueByte* input_queue_, SyncQueueByte* output_queue_, bool debug_comm) = 0;
    virtual void                   notify_threads() {}
    virtual void                   finalize_threads() = 0;

    static std::unique_ptr<CommunicationModule> create(Options const& options);

protected:
    void debug_byte(uint8_t byte, bool is_input);

private:
    std::mutex debug_mutex_ {};
};

using CommUniquePtr = std::unique_ptr<CommunicationModule>;

#endif //COMM_HH_
