#include "commands.h"
#include "server.h"
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LISTENERS 2

int main(int argc, char *argv[])
{
    char *port;

    if (argc == 1) {
        port = "8080";
    } else if (argc == 2) {
        port = argv[1];
    } else {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        fprintf(stderr, "Default: If no port is supplied port 8080 will be used.");
        exit(EXIT_FAILURE);
    }
    // Ignore SIGPIPE: failed writes return EPIPE
    struct sigaction ignore_signal_action = {.sa_handler = SIG_IGN};
    sigemptyset(&ignore_signal_action.sa_mask);
    if (sigaction(SIGPIPE, &ignore_signal_action, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    int init_status = server_init(port);

    if (init_status != 0) {
        fprintf(
            stderr, "Failed while calling server_init. gai error: %s\n",
            gai_strerror(init_status)
        );
        exit(EXIT_FAILURE);
    }

    int listener_socket_fds[MAX_LISTENERS];
    int listener_count = server_listen(listener_socket_fds, MAX_LISTENERS);

    if (listener_count <= 0) {
        fprintf(
            stderr,
            "Oh deary me, something happened and no listeners were started on "
            "port %s.\n",
            port
        );
        exit(EXIT_FAILURE);
    }

    printf(
        "Now listening on port %s (%d listener%s)\n", port, listener_count,
        listener_count == 1 ? "" : "s"
    );

    struct pollfd watched_descriptors[MAX_LISTENERS + 1];
    watched_descriptors[0] = (struct pollfd){.fd = STDIN_FILENO, .events = POLLIN};
    for (int listener_index = 0; listener_index < listener_count; listener_index++)
        watched_descriptors[listener_index + 1] =
            (struct pollfd){.fd = listener_socket_fds[listener_index], .events = POLLIN};

    int should_quit = 0;
    fputs("> ", stdout);
    fflush(stdout);

    while (!should_quit) {
        if (poll(watched_descriptors, listener_count + 1, -1) < 0) {
            if (errno == EINTR)
                continue;
            perror("poll");
            break;
        }

        if (watched_descriptors[0].revents & (POLLIN | POLLHUP)) {
            char command_input[256];
            if (fgets(command_input, sizeof command_input, stdin) == NULL)
                should_quit = 1; // EOF Ctrl + D
            else
                should_quit = handle_user_command(command_input);

            if (!should_quit) {
                fputs("> ", stdout);
                fflush(stdout);
            }
        }

        for (int descriptor_index = 1; descriptor_index <= listener_count; descriptor_index++)
            if (watched_descriptors[descriptor_index].revents & POLLIN)
                server_accept_conn();
    }

    if (server_shutdown() != 0) {
        fprintf(stderr, "Failed while calling server_shutdown.\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}