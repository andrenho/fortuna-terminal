#ifndef LIBFORTUNA_H_
#define LIBFORTUNA_H_

#include <stdint.h>

#include "../fortuna-protocol.h"

struct FortunaTerminal;

void ft_init(struct FortunaTerminal* ft,
             int    (* send)(uint8_t* bytes, size_t sz),
             ssize_t(* recv)(uint8_t* bytes, size_t max_sz),
             void* data);
void ft_finalize(struct FortunaTerminal* ft);

int ft_send(struct FortunaTerminal* ft, FP_Command cmds[], size_t sz);
int ft_recv(struct FortunaTerminal* ft, FP_Command cmds[], size_t max_sz);

#endif //LIBFORTUNA_H_
