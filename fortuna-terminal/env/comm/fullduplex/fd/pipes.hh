#ifndef PIPES_HH_
#define PIPES_HH_

#include "fdcomm.hh"

class Pipes : public FDComm {
public:
    explicit Pipes(size_t readbuf_sz);

    [[nodiscard]] std::string description() const override;
};

#endif //PIPES_HH_
