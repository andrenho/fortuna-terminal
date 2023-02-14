#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>
#include <sys/types.h>

typedef enum FT_Result {
    FT_OK,
    FT_ERR_APP,
    FT_ERR_LIBC,
    FT_ERR_SDL,
} FT_Result;

void error_set(const char* fmt, ...);
void error_print(FT_Result result);

// return error from function, doesn't do anything else
// TODO - set str
#define E_CHECK(result, ...) \
    { if ((result) != FT_OK) { error_set("" __VA_ARGS__); return result; } }

// report error to stderr and abort
#define E_STDERR_ABORT(result, ...) \
    { if ((result) != FT_OK) { error_set("" __VA_ARGS__); error_print(result); abort(); } }

// report error to UI and attempts to reset terminal
#define E_UI(result, ...) \
    { if ((result) != FT_OK) abort(); }

// report error to UI, wait for a keypress and then abort
#define E_UI_ABORT(result, ...) \
    { if ((result) != FT_OK) abort(); }

// abort with message
#define ABORT(...) \
    abort();

#endif //ERROR_H_
