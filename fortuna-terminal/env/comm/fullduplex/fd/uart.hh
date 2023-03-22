#ifndef UART_HH_
#define UART_HH_

#include "fdcomm.hh"
#include "application/options.hh"

class UART : public FDComm {
public:
    explicit UART(UartOptions const& uart_options, size_t readbuf_sz);

    [[nodiscard]] std::string description() const override;

private:
    UartOptions uart_options_;
};

#endif //UART_HH_
