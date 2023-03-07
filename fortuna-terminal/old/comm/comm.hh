#ifndef COMMUNICATIONMODULE_HH_
#define COMMUNICATIONMODULE_HH_

#include <cstdint>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "old/common/noncopyable.hh"
#include "old/options.hh"

class CommunicationModule : NonCopyable {
public:
    virtual ~CommunicationModule() = default;

    virtual std::vector<uint8_t>   read_blocking(size_t n) = 0;
    virtual std::optional<uint8_t> read_blocking();

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

    static std::unique_ptr<CommunicationModule> create_unique(Options const& options);

protected:
    CommunicationModule() = default;
};

using CommUniquePtr = std::unique_ptr<CommunicationModule>;

#endif //COMMUNICATIONMODULE_HH_
