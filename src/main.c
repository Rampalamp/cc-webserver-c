#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

int main(void)
{
    int init_status = server_init();

    if (init_status != 0)
    {
        fprintf(stderr, "Failed while calling server_init. gai error: %s\n", gai_strerror(init_status));
        exit(EXIT_FAILURE);
    }

    if (server_listen() != 0)
    {
        fprintf(stderr, "Failed while running server_listen.\n");
        exit(EXIT_FAILURE);
    }

    if (server_shutdown() != 0)
    {
        fprintf(stderr, "Failed while calling server_shutdown.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}