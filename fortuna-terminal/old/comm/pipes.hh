#ifndef PIPES_HH_
#define PIPES_HH_

#include "fdcomm.hh"

class Pipes : public FDComm {
public:
    Pipes();

    void on_read_zero() override;
};

#endif //PIPES_HH_
