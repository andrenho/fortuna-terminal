#ifndef COMMUNICATIONMODULE_HH_
#define COMMUNICATIONMODULE_HH_

#include <cstdint>

#include <string>
#include <vector>

#include "../common/noncopyable.hh"

class CommunicationModule : NonCopyable {
public:
    virtual ~CommunicationModule() = default;

    virtual std::vector<uint8_t> read_blocking(size_t n) = 0;
    virtual uint8_t              read_blocking() { return read_blocking(1).at(0); }

    virtual void                 write(std::vector<uint8_t> const& data) = 0;

    virtual bool                 release_locks() { return false; }

    void write(std::string const& str) {
        std::vector<uint8_t> vec(str.begin(), str.end());
        write(vec);
    }

    void write(uint8_t byte) {
        std::vector<uint8_t> vec = { byte };
        write(vec);
    }

protected:
    CommunicationModule() = default;
};

#endif //COMMUNICATIONMODULE_HH_
