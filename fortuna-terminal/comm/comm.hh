#ifndef COMM_HH_
#define COMM_HH_

#include <memory>
#include <vector>

#include "application/options.hh"
#include "common/syncqueue.hh"
#include "common/types/noncopyable.hh"

class CommunicationModule : NonCopyable {
public:
    virtual ~CommunicationModule() = default;

    // this request must return immediately
    [[nodiscard]] virtual std::string exchange(std::string_view data_to_send) = 0;

    [[nodiscard]] virtual std::string description() const = 0;
    [[nodiscard]] virtual bool is_overwhelmed() const { return false; }

    virtual void vsync() {}

    static std::unique_ptr<CommunicationModule> create(CommType comm_type, Options const& options);

protected:
    CommunicationModule() = default;

    void debug_byte(uint8_t byte, bool is_input);

private:
    std::mutex debug_mutex_ {};
};

using CommUniquePtr = std::unique_ptr<CommunicationModule>;

#endif //COMM_HH_
