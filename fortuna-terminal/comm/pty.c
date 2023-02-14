#include "pty.h"
#include "../error/error.h"

#include <fcntl.h>
#include <unistd.h>
#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

static int master_fd = 0;

FT_Result pty_init(size_t lines, size_t columns)
{
    struct winsize winp = { lines, columns, 0 , 0 };

    pid_t pid = forkpty(&master_fd, NULL, NULL, &winp);
    if (pid < 0) {
        return FT_ERR_LIBC;
    } else if (pid == 0) {
        setenv("LANG", "C", 1);
        setenv("TERM", "ansi", 1);
        // this is the child process that will execute the shell
        execl(options.pty.shell, options.pty.shell, NULL);
    }

    // make read blocking
    int flags = fcntl(master_fd, F_GETFL);
    flags &= ~O_NONBLOCK;
    fcntl(master_fd, F_SETFL, flags);

    return 0;
}

static void pty_client_disconnected()
{
    close(master_fd);
    master_fd = 0;
    printf("PTY client disconnected.\n");
    exit(EXIT_SUCCESS);
}

FT_Result pty_recv(uint8_t* byte, bool* data_received)
{
    int n = read(master_fd, byte, 1);
    if (n == 0)
        pty_client_disconnected();
    else if (n < 0)
        return FT_ERR_LIBC;
    *data_received = true;
    return FT_OK;
}

FT_Result pty_send(const uint8_t* data, size_t sz)
{
    int n = write(master_fd, data, sz);
    if (n == 0)
        pty_client_disconnected();
    else if (n < 0)
        return FT_ERR_LIBC;
    return FT_OK;
}

void pty_finalize()
{
    if (master_fd != 0)
        close(master_fd);
}
