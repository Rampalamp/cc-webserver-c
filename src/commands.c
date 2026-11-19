#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    COMMAND_HELP,
    COMMAND_LISTEN,
    COMMAND_STOP,
    COMMAND_START,
    COMMAND_EXIT,
    COMMAND_UNKNOWN
} CommandType;

typedef struct {
    const char *command_name;
    CommandType command_type;
} CommandTableEntry;

static const CommandTableEntry command_table[] = {
    {"help", COMMAND_HELP},
    {"listen", COMMAND_LISTEN},
    {"stop", COMMAND_STOP},
    {"start", COMMAND_START},
    {"exit", COMMAND_EXIT}
};

static CommandType parse_command(const char *command_input)
{
    if (command_input == NULL) {
        return COMMAND_UNKNOWN;
    }

    size_t table_length = sizeof(command_table) / sizeof(command_table[0]);

    for (size_t table_index = 0; table_index < table_length; table_index++) {
        if (strcmp(command_input, command_table[table_index].command_name) == 0) {
            return command_table[table_index].command_type;
        }
    }

    return COMMAND_UNKNOWN;
}

static void run_help()
{
    printf("It's dangerous to go alone! Take this.\n");
    printf("Accepts 5 case sensisitive commands...\n");
    printf("\n");
    printf("help\n");
    printf("\t-Displays this message so you dont have to go alone.\n");
    printf("listen\n");
    printf(
        "\tListen to outputs from desired socket. Accepts one parameter with two valid "
        "inputs, sock1 and sock2. Examples:\n"
    );
    printf("\t\tlisten sock1\n");
    printf("\t\tlisten sock2\n");
    printf("stop\n");
    printf(
        "\tStop/shutdown the desired socket file descriptor. Accepts one parameter with two "
        "valid inputs, sock1 and sock2. Examples:\n"
    );
    printf("\t\tstop sock1\n");
    printf("\t\tstop sock2\n");
    printf("start\n");
    printf(
        "\tStart the desired socket file descriptor. Accepts two possible parameters, the "
        "first one (socket identifier) is required, the second is port number to listen on. "
        "If no port is specified it will start on 8080 by default. First parameter accepts two"
        "valid inputs, sock1 and sock2. Examples:\n"
    );
    printf("\t\tstart sock1 8096\n");
    printf("\t\tstart sock2\n");
    printf("exit\n");
    printf("\tShutdown sockets and exit program.\n");
}

static void run_listen(const char *listen_arg)
{
    printf("Listen...\n");
}

static void run_stop()
{
    printf("Stop...\n");
}

static void run_start()
{
    printf("Start...\n");
}

static void run_exit()
{
    printf("Exit...\n");
}

int handle_user_command(char *command_input)
{
    command_input[strcspn(command_input, "\r\n")] = '\0';

    switch (parse_command(command_input)) {
        case COMMAND_HELP:
            run_help();
            break;
        case COMMAND_LISTEN:
            // parse out second arg
            run_listen("");
            break;
        case COMMAND_STOP:
            run_stop();
            break;
        case COMMAND_START:
            run_start();
            break;
        case COMMAND_EXIT:
            run_exit();
            return 1;
        case COMMAND_UNKNOWN:
        default:
            printf("Command Unknown... Type 'help' to see options.\n");
            break;
    }

    return EXIT_SUCCESS;
}