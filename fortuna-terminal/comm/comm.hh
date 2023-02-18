#ifndef COMMUNICATIONMODULE_HH_
#define COMMUNICATIONMODULE_HH_

#include <cstdint>

#include <chrono>
#include <string>
#include <vector>

#include "../common/noncopyable.hh"

class CommunicationModule : NonCopyable {
public:
    virtual std::vector<uint8_t> read_blocking(size_t n) const = 0;
    virtual std::vector<uint8_t> read_for(std::chrono::duration<double> duration) const = 0;

    virtual void                 write(std::vector<uint8_t> const& data) const = 0;

    void write(std::string const& str) const {
        std::vector<uint8_t> vec(str.begin(), str.end());
        write(vec);
    }

    void write(uint8_t byte) const {
        std::vector<uint8_t> vec = { byte };
        write(vec);
    }

protected:
    CommunicationModule() = default;
};

#endif //COMMUNICATIONMODULE_HH_
