#include "commands.h"
#include "server.h"
#include <ccutils/cc_parser.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFAULT_MAX_LISTENERS 2
#define DEFAULT_PORT "8080"
#define MIN_PORT_NUM 1
#define MAX_PORT_NUM 65535

typedef struct {
    char *port_number;
    int max_listeners;
} ServerConfiguration;

static void print_usage(const char *program_name)
{
    fprintf(
        stderr,
        "Usage: %s [--port N] [--maximum-listeners N]\n"
        "  -p, --port                port to listen on (1-65535, default %d)\n"
        "  -l, --maximum-listeners   listener limit (default %d)\n"
        "  -h, --help                show this message\n",
        program_name, DEFAULT_PORT, DEFAULT_MAX_LISTENERS
    );
}

int main(int argc, char *argv[])
{
    ServerConfiguration config = {
        .port_number = DEFAULT_PORT,
        .max_listeners = DEFAULT_MAX_LISTENERS,
    };

    static const struct option long_options[] = {
        {"port", required_argument, NULL, 'p'},
        {"maximum-listeners", required_argument, NULL, 'l'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0},
    };

    int option_character;
    while ((option_character = getopt_long(argc, argv, "p:l:h", long_options, NULL)) != -1) {
        switch (option_character) {
            case 'p':
                if (!(ccp_parse_int_range(optarg, MIN_PORT_NUM, MAX_PORT_NUM, NULL))) {
                    fprintf(stderr, "Invalid port '%s'\n", optarg);
                    exit(EXIT_FAILURE);
                }
                config.port_number = optarg;
                break;
            case 'l':
                if (!ccp_parse_int_range(optarg, 1, INT_MAX, &config.max_listeners)) {
                    fprintf(stderr, "Invalid maximum listeners '%s'\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        fprintf(stderr, "Unexpected argument '%s'\n", argv[optind]);
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Ignore SIGPIPE: failed writes return EPIPE
    struct sigaction ignore_signal_action = {.sa_handler = SIG_IGN};
    sigemptyset(&ignore_signal_action.sa_mask);
    if (sigaction(SIGPIPE, &ignore_signal_action, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    int init_status = server_init(config.port_number);

    if (init_status != 0) {
        fprintf(
            stderr, "Failed while calling server_init. gai error: %s\n",
            gai_strerror(init_status)
        );
        exit(EXIT_FAILURE);
    }

    int listener_socket_fds[config.max_listeners];
    int listener_count = server_listen(listener_socket_fds, config.max_listeners);

    if (listener_count <= 0) {
        fprintf(
            stderr,
            "Oh deary me, something happened and no listeners were started on "
            "port %s.\n",
            config.port_number
        );
        exit(EXIT_FAILURE);
    }

    printf(
        "Now listening on port %s (%d listener%s)\n", config.port_number, listener_count,
        listener_count == 1 ? "" : "s"
    );

    struct pollfd watched_descriptors[config.max_listeners + 1];
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