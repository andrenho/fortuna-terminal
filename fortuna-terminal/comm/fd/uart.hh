#ifndef UART_HH_
#define UART_HH_

#include "commfd.hh"
#include "application/options.hh"

class UART : public CommFileDescriptor {
public:
    UART(UartOptions const& uart_options, size_t readbuf_sz);

    std::string description() const override;

private:
    UartOptions uart_options_;
};

#endif //UART_HH_
