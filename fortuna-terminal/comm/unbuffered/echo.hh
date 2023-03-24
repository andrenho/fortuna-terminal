#ifndef ECHO_HH_
#define ECHO_HH_

#include <string_view>
#include "communbuffered.hh"

class Echo : public CommUnbuffered {
public:
    [[nodiscard]] std::string description() const override { return "Echo"; }

protected:
    std::string thread_exchange(std::string_view data_to_send) override;
};

#endif //ECHO_HH_
