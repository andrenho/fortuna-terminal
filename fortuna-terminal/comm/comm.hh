#ifndef COMMUNICATIONMODULE_HH_
#define COMMUNICATIONMODULE_HH_

#include <cstdint>
#include <vector>

#include "../common/noncopyable.hh"

class CommunicationModule : NonCopyable {
public:
    virtual std::vector<uint8_t> read_block(size_t n) const = 0;
    virtual std::vector<uint8_t> read_for(long nanoseconds) const = 0;
    virtual void                 write(std::vector<uint8_t> const& data) const = 0;

protected:
    CommunicationModule() = default;
};

#endif //COMMUNICATIONMODULE_HH_
