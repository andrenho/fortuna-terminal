#ifndef COMM_HH_
#define COMM_HH_

#include <optional>
#include <memory>
#include <vector>

#include "application/options.hh"
#include "common/noncopyable.hh"

enum class Channels {
    InputAndOutput, Exchange,
};

class CommunicationModule : NonCopyable {
public:
    virtual ~CommunicationModule() = default;

    virtual Channels channels() const = 0;

    static std::unique_ptr<CommunicationModule> create(Options const& options);
};

using CommUniquePtr = std::unique_ptr<CommunicationModule>;

#endif //COMM_HH_
