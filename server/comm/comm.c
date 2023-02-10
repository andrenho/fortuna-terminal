#include "comm.h"

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "error/error.h"
#include "echo.h"
#include "pty.h"
#include "uart.h"
#include "tcpip.h"

#define BUFFER_SZ (32 * 1024)

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

int comm_init(size_t lines, size_t columns)
{
    (void) lines; (void) columns;

    switch (options.comm_mode) {
        case CM_ECHO:
            comm_f = (CommFunctions) { echo_recv, echo_send, echo_finalize };
            return echo_init();
        case CM_TCPIP:
            comm_f = (CommFunctions) { tcpip_recv, tcpip_send, tcpip_finalize };
            return tcpip_init();
#ifdef COMM_PTY
        case CM_PTY:
            comm_f = (CommFunctions) { pty_recv, pty_send, pty_finalize };
            return pty_init(lines, columns);
#endif
#ifdef COMM_UART
        case CM_UART:
            comm_f = (CommFunctions) { uart_recv, uart_send, uart_finalize };
            return uart_init();
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
            printf("\e[0;%dm%c\e[0m", color, bytes[i]);
        else
            printf("\e[0;%dm[%02X]\e[0m", color, bytes[i]);
#endif
        if (bytes[i] == '\n')
            printf("\n");
    }
    fflush(stdout);
}

static void* comm_input_thread_run(void* input_buffer_ptr)
{
    Buffer* input_buffer = input_buffer_ptr;

    while (threads_running_) {

        uint8_t byte = 0;
        int r = comm_f.recv(&byte);
        if (r == ERR_NO_DATA)
            continue;
        error_check(r);

        /*
        if (debug_mode)
            print_bytes(&byte, 1, 31);
        */

        buffer_add_byte(input_buffer, byte);
    }

    return NULL;
}

static void* comm_output_thread_run(void* output_buffer_ptr)
{
    Buffer* output_buffer = output_buffer_ptr;
    uint8_t comm_output_array[BUFFER_SZ];

    while (threads_running_) {

        buffer_wait(output_buffer);    // wait until there's data available

        ssize_t sz = buffer_move_data_to_array(output_buffer, comm_output_array, BUFFER_SZ);

        error_check(comm_f.send(comm_output_array, sz));
        /*
        if (debug_mode)
            print_bytes(comm_output_array, sz, 32);
        */
    }

    return NULL;
}

int comm_run_input(Buffer* input_buffer)
{
    threads_running_ = true;
    pthread_mutex_init(&mutex_input_, NULL);
    error_check(pthread_create(&thread_input_, NULL, comm_input_thread_run, input_buffer));

    return 0;
}

int comm_run_output(Buffer* output_buffer)
{
    threads_running_ = true;
    pthread_mutex_init(&mutex_output_, NULL);
    pthread_cond_init(&output_has_data_, NULL);
    error_check(pthread_create(&thread_output_, NULL, comm_output_thread_run, output_buffer));

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
