#include "libfortuna.h"

typedef struct FortunaTerminal {
    void(* receive_data)(FP_Command* cmds, size_t sz);
    void* data;
} FortunaTerminal;

void ft_init(FortunaTerminal* ft, void(* receive_data)(FP_Command* cmds, size_t sz), void* data)
{
    ft->receive_data = receive_data;
    ft->data = data;
}

void ft_send(FP_Command* cmds, size_t sz)
{
}

void ft_finalize(FortunaTerminal* ft)
{
}
