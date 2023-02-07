#include "comm.h"
#include "error/error.h"
#include "echo.h"

#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

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
static pthread_mutex_t mutex_input_ = 0,
                       mutex_output_ = 0;
static pthread_cond_t  output_has_data_ = 0;
static bool            threads_running_ = false;

// synchronized buffers
static uint8_t         input_buf_[INPUTBUF_SZ];
static uint8_t         output_buf_[OUTPUTBUF_SZ];
static size_t          input_buf_sz_ = 0,
                       output_buf_sz_ = 0;

int comm_init(Options* options)
{
    switch (options->comm_mode) {
        case CM_ECHO:
            comm_f = (CommFunctions) { echo_recv, echo_send, echo_finalize };
            return echo_init();
        default:
            return ERR_NOT_IMPLEMENTED;
    }
}

static void* comm_input_thread_run()
{
    while (threads_running_) {
        uint8_t byte;
        int r = comm_f.recv(&byte);
        if (r == ERR_NO_DATA)
            continue;
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
        // TODO - add check condition so the loop doesn't keep running
        if (output_buf_sz_ > 0) {
            uint8_t buffer[output_buf_sz_];

            pthread_mutex_lock(&mutex_output_);
            memcpy(buffer, output_buf_, output_buf_sz_);
            size_t sz = output_buf_sz_;
            output_buf_sz_ = 0;
            pthread_mutex_unlock(&mutex_output_);

            error_check(comm_f.send(buffer, sz));
        }
    }

    return NULL;
}

size_t comm_unload_input_queue(uint8_t* dest, size_t max_sz)
{
    size_t sz = (input_buf_sz_ < max_sz) ? input_buf_sz_ : max_sz;

    pthread_mutex_lock(&mutex_input_);
    memcpy(dest, input_buf_, sz);
    input_buf_sz_ = 0;
    pthread_mutex_unlock(&mutex_input_);

    return sz;
}

int comm_add_to_output_queue(uint8_t* data, uint8_t sz)
{
    if ((output_buf_sz_ + sz) > OUTPUTBUF_SZ)
        error_check(ERR_BUF_OVERFLOW);

    pthread_mutex_lock(&mutex_output_);
    memcpy(&output_buf_[output_buf_sz_], data, sz);
    output_buf_sz_ += sz;
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

    pthread_cancel(thread_input_);
    pthread_join(thread_input_, NULL);
    pthread_cancel(thread_output_);
    pthread_join(thread_output_, NULL);

    pthread_mutex_destroy(&mutex_input_);
    pthread_mutex_destroy(&mutex_output_);
    pthread_cond_destroy(&output_has_data_);

    return comm_f.finalize();
}
