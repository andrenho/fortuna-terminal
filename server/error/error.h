#ifndef ERROR_H_
#define ERROR_H_

typedef enum {
    OK = 0,
    ERR_SDL = -1,
} Error;

void error_check(int f);

#endif //ERROR_H_
