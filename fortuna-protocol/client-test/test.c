#include "../fortuna-protocol.h"

#include <stdio.h>
#include <string.h>

int main()
{
    uint8_t buffer[FP_MSG_SZ];

    FP_Message msg = {
            .command = FP_TEXT_PRINT_TEXT,
    };
    strcpy((char *) msg.text, "Hello world");

    uint8_t msg_sz;
    printf("serialize result: %d\n", fp_msg_serialize(&msg, buffer, &msg_sz));
    printf("msg_sz: %d\n", msg_sz);
    for (size_t i = 0; i < msg_sz; ++i)
        printf("%02X ", buffer[i]);
    printf("\n\n");

    FP_Message msg2;
    printf("unserialize result: %d\n", fp_msg_unserialize(buffer, &msg2));

    return 0;
}