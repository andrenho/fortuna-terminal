#if _WIN32
#  include <winsock2.h>
#  include <wspiapi.h>
#  define SOCKET int
#else
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <unistd.h>
#  define INVALID_SOCKET -1
#endif

#include "../fortuna-protocol.h"

SOCKET fd = 0;

static int sendf(uint8_t const* buffer, size_t sz) {
    return send(fd, (const char *) buffer, (int) sz, 0);
}

static int recvf(uint8_t* buf, size_t sz) {
    return recv(fd, (char *) buf, (int) sz, 0);
}

int main()
{

#if _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
        printf("WSAStartup failed: %d", iResult);
#endif

    fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8076);

    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed. Error");
        return 1;
    }

    FP_Message msg = {
            .command = FP_TEXT_PRINT_CHAR,
            .chr = 'A',
    };
    int error;
    fp_msg_send(&msg, sendf, recvf, &error);

#if _WIN32
    closesocket(fd);
#else
    close(fd);
#endif

    return 0;
}