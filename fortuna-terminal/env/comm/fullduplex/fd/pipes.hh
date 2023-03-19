#ifndef PIPES_HH_
#define PIPES_HH_

#include "fdcomm.hh"

class Pipes : public FDComm {
public:
    Pipes();

    [[nodiscard]] std::string description() const override;
};

#endif //PIPES_HH_
