#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
    CM_NOT_CHOSEN, CM_ECHO, CM_UART, CM_I2C, CM_SPI, CM_TCPIP, CM_EMCC, CM_DEBUG, CM_PTY
} CommunicationMode;

typedef enum {
    PR_FORTUNA, PR_ANSI, PR_FORTUNA_ANSI
} ProtocolType ;

#define PORT_SZ 1024
typedef struct {
    size_t      baud;
    char        port[PORT_SZ];
    char        parity;
    uint8_t     stop_bits;
} SerialOptions;

typedef struct {
    uint16_t port;
} TCPIPOptions;

#define SHELL_SZ 1024
typedef struct {
    char shell[SHELL_SZ];
} PTYOptions;

typedef struct {
    CommunicationMode comm_mode;
    ProtocolType      protocol;
    bool              debug_mode;
    bool              debug_bytes;
    bool              window_mode;
    SerialOptions     serial;
    TCPIPOptions      tcpip;
    PTYOptions        pty;
} Options;

int options_parse_cmdline(int argc, char *argv[], Options *options);

#endif //OPTIONS_H_
