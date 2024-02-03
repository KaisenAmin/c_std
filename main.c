#include "cli/cli.h"
#include "fmt/fmt.h"
#include <string.h>

void helpCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command; // Unused parameter
    (void)argc;    // Unused parameter
    (void)argv;    // Unused parameter
    (void)userData; // Unused parameter

    fmt_fprintf(stdout, "Available commands:\n");
    fmt_fprintf(stdout, "  help    - Displays this help message\n");
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("HelpApp");
    if (!parser) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    CliCommand helpCommand = {
        .name = "help",
        .handler = helpCommandHandler,
        .description = "Displays help information",
        .userData = NULL
    };

    cli_register_command(parser, &helpCommand);

    if (argc > 1) {
        const CliCommand *foundCommand = cli_find_command(parser, argv[1]);
        if (foundCommand) {
            foundCommand->handler(foundCommand, argc - 1, argv + 1, parser);
        } 
        else {
            fmt_fprintf(stderr, "Unknown command: %s\n", argv[1]);
        }
    } 
    else {
        fmt_fprintf(stdout, "No command provided. Use 'help' for more information.\n");
    }

    cli_parser_deallocate(parser);
    return 0;
}
