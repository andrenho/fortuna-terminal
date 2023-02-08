#include "comm.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "error/error.h"
#include "echo.h"
#include "pty.h"
#include "tcpip.h"

#define INPUTBUF_SZ  (32 * 1024)
#define OUTPUTBUF_SZ (32 * 1024)

typedef struct {
    int (* recv)(uint8_t* byte);
    int (* send)(const uint8_t* data, size_t sz);
    int (* finalize)();
} CommFunctions;
static CommFunctions comm_f = { NULL, NULL, NULL };

static pthread_t       thread_input_ = 0,
                       thread_output_ = 0;
static pthread_mutex_t mutex_input_,
                       mutex_output_;
static pthread_cond_t  output_has_data_;
static bool            threads_running_ = false;

// synchronized buffers
static uint8_t         input_buf_[INPUTBUF_SZ];
static uint8_t         output_buf_[OUTPUTBUF_SZ];
static size_t          input_buf_sz_ = 0,
                       output_buf_sz_ = 0;

static bool            debug_mode = false;

int comm_init(Options* options, size_t lines, size_t columns)
{
    (void) lines; (void) columns;

    debug_mode = options->debug_mode;

    switch (options->comm_mode) {
        case CM_ECHO:
            comm_f = (CommFunctions) { echo_recv, echo_send, echo_finalize };
            return echo_init();
        case CM_TCPIP:
            comm_f = (CommFunctions) { tcpip_recv, tcpip_send, tcpip_finalize };
            return tcpip_init(&options->tcpip);
#ifdef COMM_PTY
        case CM_PTY:
            comm_f = (CommFunctions) { pty_recv, pty_send, pty_finalize };
            return pty_init(&options->pty, lines, columns);
#endif
#ifdef COMM_UART
        case CM_UART:
            comm_f = (CommFunctions) { uart_recv, uart_send, uart_finalize };
            return uart_init(&options->serial);
#endif
        default:
            return ERR_NOT_IMPLEMENTED;
    }
}

static void print_bytes(const uint8_t* bytes, size_t sz, int color)
{
    for (size_t i = 0; i < sz; ++i) {
#ifdef _WIN32
        if (bytes[i] >= 32 && bytes[i] < 127)
            printf("%c[%c] ", color == 31 ? 'I' : 'O', bytes[i]);
        else
            printf("%c[%02X] ", color == 31 ? 'I' : 'O', bytes[i]);
#else
        if (bytes[i] >= 32 && bytes[i] < 127)
            printf("\e[0;%dm[%c]\e[0m ", color, bytes[i]);
        else
            printf("\e[0;%dm[%02X]\e[0m ", color, bytes[i]);
#endif
    }
}

static void* comm_input_thread_run()
{
    while (threads_running_) {
        uint8_t byte = 0;
        int r = comm_f.recv(&byte);
        if (r == ERR_NO_DATA)
            continue;
        if (debug_mode)
            print_bytes(&byte, 1, 31);
        error_check(r);

        if (input_buf_sz_ >= INPUTBUF_SZ)
            error_check(ERR_BUF_OVERFLOW);

        pthread_mutex_lock(&mutex_input_);
        input_buf_[input_buf_sz_++] = byte;
        pthread_mutex_unlock(&mutex_input_);
    }

    return NULL;
}

static void* comm_output_thread_run()
{
    while (threads_running_) {

        uint8_t buffer[OUTPUTBUF_SZ];
        size_t sz = 0;

        pthread_mutex_lock(&mutex_output_);

        pthread_cond_wait(&output_has_data_, &mutex_output_);

        if (output_buf_sz_ > 0) {
            memcpy(buffer, output_buf_, output_buf_sz_);
            sz = output_buf_sz_;
            output_buf_sz_ = 0;
        }

        pthread_mutex_unlock(&mutex_output_);

        error_check(comm_f.send(buffer, sz));
        if (debug_mode)
            print_bytes(buffer, sz, 32);
    }

    return NULL;
}

size_t comm_unload_input_queue(uint8_t* dest, size_t max_sz)
{
    size_t sz = (input_buf_sz_ < max_sz) ? input_buf_sz_ : max_sz;

    if (sz > 0) {
        pthread_mutex_lock(&mutex_input_);
        memcpy(dest, input_buf_, sz);
        input_buf_sz_ = 0;
        pthread_mutex_unlock(&mutex_input_);
    }

    return sz;
}

int comm_add_to_output_queue(uint8_t* data, uint8_t sz)
{
    if (sz == 0)
        return 0;

    if ((output_buf_sz_ + sz) > OUTPUTBUF_SZ)
        error_check(ERR_BUF_OVERFLOW);

    pthread_mutex_lock(&mutex_output_);
    memcpy(&output_buf_[output_buf_sz_], data, sz);
    output_buf_sz_ += sz;
    pthread_cond_signal(&output_has_data_);
    pthread_mutex_unlock(&mutex_output_);

    return 0;
}

int comm_run()
{
    threads_running_ = true;
    pthread_mutex_init(&mutex_input_, NULL);
    pthread_mutex_init(&mutex_output_, NULL);
    pthread_cond_init(&output_has_data_, NULL);
    error_check(pthread_create(&thread_input_, NULL, comm_input_thread_run, NULL));
    error_check(pthread_create(&thread_output_, NULL, comm_output_thread_run, NULL));
    return 0;
}

int comm_notify_vsync()
{
    // TODO
    return 0;
}

int comm_finalize()
{
    threads_running_ = false;
    usleep(100000);

    pthread_kill(thread_input_, 9);
    pthread_kill(thread_output_, 9);

    pthread_cond_destroy(&output_has_data_);

    pthread_mutex_destroy(&mutex_input_);
    pthread_mutex_destroy(&mutex_output_);

    return comm_f.finalize();
}
