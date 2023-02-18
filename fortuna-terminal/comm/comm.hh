#ifndef COMMUNICATIONMODULE_HH_
#define COMMUNICATIONMODULE_HH_

#include <cstdint>

#include <chrono>
#include <string>
#include <vector>

#include "../common/noncopyable.hh"

class CommunicationModule : NonCopyable {
public:
    virtual std::vector<uint8_t> read_blocking(size_t n) = 0;
    virtual std::vector<uint8_t> read_for(std::chrono::duration<double> duration) = 0;

    virtual void                 write(std::vector<uint8_t> const& data) = 0;

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
