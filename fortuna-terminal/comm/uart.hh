#ifndef UART_HH_
#define UART_HH_

#include "fdcomm.hh"
#include "../options.hh"

class UART : public FDComm {
public:
    explicit UART(UartOptions const& uart_options);
};

#endif //UART_HH_
