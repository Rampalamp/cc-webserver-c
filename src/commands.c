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
    printf("Help...\n");
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

int handle_user_command(const char *command_input)
{

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
            run_start();
        case COMMAND_UNKNOWN:
        default:
            printf("Command Unknown...\n");
            break;
    }

    return EXIT_SUCCESS;
}