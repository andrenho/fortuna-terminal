#include "libfortuna.h"

typedef struct FortunaTerminal {
    int    (* send)(uint8_t* bytes, size_t sz);
    ssize_t(* recv)(uint8_t* bytes, size_t max_sz);
    void* data;
} FortunaTerminal;

void ft_init(FortunaTerminal* ft,
             int    (* send)(uint8_t* bytes, size_t sz),
             ssize_t(* recv)(uint8_t* bytes, size_t max_sz),
             void* data)
{
    ft->send = send;
    ft->recv = recv;
    ft->data = data;
}

void ft_finalize(FortunaTerminal* ft)
{
    (void) ft;
}

int ft_send(struct FortunaTerminal* ft, FP_Command cmds[], size_t sz)
{
    return 0;
}

int ft_recv(struct FortunaTerminal* ft, FP_Command cmds[], size_t max_sz)
{
    return 0;
}

