#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>

typedef enum {
    OK = 0,
    ERR_SDL = -1,
    ERR_FAIL = -2,
    ERR_NOT_IMPLEMENTED = -3,
    ERR_LIBC = -4,
    ERR_BUF_OVERFLOW = -5,
    ERR_COMMUNICATION_CLOSED = -6,
    ERR_NO_DATA = -7,  // not an error
} Error;

void error_check(int f);

#endif //ERROR_H_
