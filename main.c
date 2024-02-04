#include "cli/cli.h"
#include "fmt/fmt.h"
#include <string.h>
#include <stdlib.h>

// Command Handlers
void addCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    if (argc != 3) {
        fmt_printf("Usage: %s add <num1> <num2>\n", (char *)userData);
        return;
    }
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);
    fmt_printf("Addition is %d\n", num1 + num2);
}

void subtractCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    if (argc != 3) {
        fmt_printf("Usage: %s subtract <num1> <num2>\n", (char *)userData);
        return;
    }
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);
    fmt_printf("Subtraction is %d\n", num1 - num2);
}

// Option Handlers
void helpOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;
    CliParser *parser = (CliParser *)userData;
    cli_print_help(parser);
}

void versionOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)userData;
    (void)value;
    fmt_printf("Version 1.0.0\n");
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("calc");

    // Set custom usage message
    cli_set_custom_usage(parser, "calc [command] [options]\n"
                                  "Commands:\n"
                                  "  add <num1> <num2>     Add two numbers\n"
                                  "  subtract <num1> <num2> Subtract two numbers\n"
                                  "Options:\n"
                                  "  --help, -h             Show this help message\n"
                                  "  --version, -v          Show version information");

    // Register commands
    CliCommand addCommand = {.name = "add", .handler = addCommandHandler, .description = "Add two numbers"};
    CliCommand subtractCommand = {.name = "subtract", .handler = subtractCommandHandler, .description = "Subtract two numbers"};
    cli_register_command(parser, &addCommand);
    cli_register_command(parser, &subtractCommand);

    // Register options
    CliOption helpOption = {.longOpt = "--help", .shortOpt = 'h', .handler = helpOptionHandler, .description = "Show help message", .userData = (void *)parser};
    CliOption versionOption = {.longOpt = "--version", .shortOpt = 'v', .handler = versionOptionHandler, .description = "Show version information"};
    cli_register_option(parser, &helpOption);
    cli_register_option(parser, &versionOption);

    // Parse CLI arguments
    if (argc > 1) {
        if (strcmp(argv[1], "add") == 0 || strcmp(argv[1], "subtract") == 0) {
            cli_parse_args(parser, argc, argv);
        } 
        else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            helpOptionHandler(&helpOption, NULL, parser);
        } 
        else if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            versionOptionHandler(&versionOption, NULL, NULL);
        } 
        else {
            fmt_printf("Unknown command or option. Use '--help' for usage information.\n");
        }
    } else {
        fmt_printf("No command provided. Use '--help' for usage information.\n");
    }

    cli_parser_deallocate(parser);
    return 0;
}
