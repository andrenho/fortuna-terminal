#ifndef PIPES_HH_
#define PIPES_HH_

#include "commbuffered.hh"

class Pipes : public CommBuffered {
public:
    Pipes(size_t readbuf_sz);

    [[nodiscard]] std::string description() const override { return "Pipes (stdin/stdout)"; }
};

#endif //PIPES_HH_
