#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>
#include <sys/types.h>

typedef enum {
    OK = 0,
    ERR_NO_DATA = -1,  // not an error
    ERR_SDL = -2,
    ERR_FAIL = -3,
    ERR_NOT_IMPLEMENTED = -4,
    ERR_LIBC = -5,
    ERR_BUF_OVERFLOW = -6,
    ERR_COMMUNICATION_CLOSED = -7,
    ERR_MESSAGE = -8,
} Error;

#define ERR_MSG_SZ 512
extern char error_message[ERR_MSG_SZ];

void error_check(ssize_t f);

#endif //ERROR_H_
