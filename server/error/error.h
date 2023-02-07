#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>

typedef enum {
    OK = 0,
    ERR_SDL = -1,
    ERR_FAIL = -2,
    ERR_NOT_IMPLEMENTED = -3,
} Error;

void error_check(int f);

#endif //ERROR_H_
