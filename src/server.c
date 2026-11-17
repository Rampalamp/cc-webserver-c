#include <netdb.h>
#include <netinet/in.h>
#include <server.h>
#include <stdio.h>
#include <sys/socket.h>

int server_init(char *run_port) {
    struct addrinfo hints = {0};
    struct addrinfo *server_info;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, run_port, &hints, &server_info);

    if (status != 0) {
        return status;
    }

    int count = 0;

    for (struct addrinfo *current = server_info; current != NULL;
         current = current->ai_next) {
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        getnameinfo(
            current->ai_addr, current->ai_addrlen, host, sizeof(host), service,
            sizeof(service), NI_NUMERICHOST | NI_NUMERICSERV
        );

        printf("$Result %d\n", ++count);
        printf(" Family:    %d\n", current->ai_family);
        printf(" Socket:    %d\n", current->ai_socktype);
        printf(" Protocol:    %d\n", current->ai_protocol);
        printf(" Address:    %s\n", host);
        printf(" Port:    %s\n\n", service);
    }

    freeaddrinfo(server_info);

    return 0;
}

int server_listen(int *file_descriptors, int max_listeners) {
    int running = 1;

    while (running) {
        running = 0;
    }

    return 0;
}

int server_shutdown(void) {
    return 0;
}