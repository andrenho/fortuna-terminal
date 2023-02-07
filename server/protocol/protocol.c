#include "protocol.h"

#include <string.h>

#include "../comm/comm.h"
#include "ansi.h"
#include "error/error.h"

#define INPUTBUF_SZ  (32 * 1024)
#define OUTPUTBUF_SZ (32 * 1024)

typedef struct {
    int  (* process_pending_input)(const uint8_t* buffer, size_t bufsz, Scene* scene);
    void (* terminal_event)(FP_Command* command, uint8_t* buffer, size_t* sz, size_t max_bufsz);
} ProtocolFunctions;
static ProtocolFunctions protocol_f = { NULL, NULL };

static uint8_t         input_buf_[INPUTBUF_SZ];
static uint8_t         output_buf_[OUTPUTBUF_SZ];
static size_t          input_buf_sz_ = 0,
                       output_buf_sz_ = 0;

int protocol_init(Options* options)
{
    switch (options->protocol) {
        case PR_ANSI:
            protocol_f = (ProtocolFunctions) { ansi_process_pending_input, ansi_terminal_event };
            return 0;
        default:
            return ERR_NOT_IMPLEMENTED;
    }
}

void protocol_process_pending_data(Scene* scene)
{
    // process pending inputs
    input_buf_sz_ += comm_unload_input_queue(input_buf_, INPUTBUF_SZ);
    int bytes_processed = protocol_f.process_pending_input(input_buf_, input_buf_sz_, scene);
    memmove(input_buf_, &input_buf_[bytes_processed], input_buf_sz_ - bytes_processed);

    // process pending outputs
    if (output_buf_sz_ > 0) {
        comm_add_to_output_queue(output_buf_, output_buf_sz_);
        output_buf_sz_ = 0;
    }
}

void protocol_terminal_event(FP_Command* command)
{
    protocol_f.terminal_event(command, output_buf_, &output_buf_sz_, OUTPUTBUF_SZ);
}