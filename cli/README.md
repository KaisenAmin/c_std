# CLi Library in C

**Author:** amin tahmasebi
**Release Date:** 2024
**License:** ISC License


## Example 1 : Create a parser the deallocate parser and resource with `cli_parser_create` and `cli_parser_deallocate`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    CliParser *parser = cli_parser_create("MyApp");
    
    if (parser == NULL) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    // Deallocate the CLI parser and all associated resources
    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 2 : setting custome usage message `cli_set_custom_usage`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    CliParser *parser = cli_parser_create("MyApp");
    if (parser == NULL) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    // Set a custom usage message
    const char *customUsage = "Usage: MyApp [options]\n"
                              "Options:\n"
                              "  -h, --help    Display this help message\n"
                              "  -v, --version Display version information\n";
    cli_set_custom_usage(parser, customUsage);

    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 3: Enabling Strict Mode in CLI Parser with 1 `cli_enable_strict_mode`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    CliParser *parser = cli_parser_create("MyStrictApp");
    if (parser == NULL) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    // Enable strict mode
    cli_enable_strict_mode(parser, true);

    cli_parser_deallocate(parser);
    return 0;
}
```


## Example 4 : set Custom error handler with `cli_set_error_handler`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

// Custom error handler function
void customErrorHandler(const CliParser *parser, const char *error, void *userData) {
    (void)parser;
    (void)userData;

    fmt_fprintf(stderr, "Custom Error: %s\n", error);
    // Additional error handling logic can be placed here
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    CliParser *parser = cli_parser_create("ExampleApp");
    if (parser == NULL) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    // Set the custom error handler
    cli_set_error_handler(parser, customErrorHandler);

    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 5: Registering and Using a Command with `cli_register_command`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <string.h>

void greetCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command; // Unused parameter
    (void)userData; // Unused parameter

    if (argc > 1) {
        fmt_fprintf(stdout, "Hello, %s!\n", argv[1]);
    } 
    else {
        fmt_fprintf(stdout, "Hello, world!\n");
    }
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    CliParser *parser = cli_parser_create("GreetApp");
    if (parser == NULL) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    // Define the "greet" command
    CliCommand greetCommand = {
        .name = "greet",
        .handler = greetCommandHandler,
        .description = "Greets the user. Usage: greet [name]",
        .userData = NULL
    };

    // Register the "greet" command with the parser
    if (!cli_register_command(parser, &greetCommand)) {
        fmt_fprintf(stderr, "Failed to register 'greet' command\n");
        cli_parser_deallocate(parser);
        return -1;
    }

    // Simulate command-line input
    char *fakeArgs[] = {"greet", "John Doe"};
    int fakeArgc = 2;

    // Check if the first argument matches the "greet" command
    if (fakeArgc >= 1 && strcmp(fakeArgs[0], greetCommand.name) == 0) {
        greetCommand.handler(&greetCommand, fakeArgc - 1, &fakeArgs[1], greetCommand.userData);
    } 
    else {
        fmt_fprintf(stderr, "Unknown command: %s\n", fakeArgs[0]);
    }

    cli_parser_deallocate(parser);
    return 0;
}
```

### Example 6: Demonstrating `cli_print_usage` with Commands and Options `cli_register_option`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <string.h>

// Command handler for the "greet" command
void greetCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command; // Unused parameter
    (void)userData; // Unused parameter
    fmt_fprintf(stdout, "Hello, %s!\n", argc > 1 ? argv[1] : "world");
}

// Option handler for the "--help" option
void helpOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;
    (void)userData;
    CliParser *parser = (CliParser *)userData;
    cli_print_usage(parser);
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("DemoApp");
    if (parser == NULL) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    // Set a custom usage message
    cli_set_custom_usage(parser, "Usage: DemoApp [options] [commands]\n"
                                  "Options:\n"
                                  "  -h, --help    Display this help message\n"
                                  "Commands:\n"
                                  "  greet [name]  Greet someone by name, or the world if no name is given");

    // Register the "--help" option
    CliOption helpOption = {
        .longOpt = "--help",
        .shortOpt = 'h',
        .optionType = CLI_NO_ARG,
        .handler = helpOptionHandler,
        .validator = NULL,
        .description = "Display this help message",
        .userData = (void *)parser
    };

    if (!cli_register_option(parser, &helpOption)) {
        fmt_fprintf(stderr, "Failed to register '--help' option\n");
        cli_parser_deallocate(parser);
        return -1;
    }

    // Register the "greet" command
    CliCommand greetCommand = {
        .name = "greet",
        .handler = greetCommandHandler,
        .description = "Greets the user. Usage: greet [name]",
        .userData = NULL
    };

    if (!cli_register_command(parser, &greetCommand)) {
        fmt_fprintf(stderr, "Failed to register 'greet' command\n");
        cli_parser_deallocate(parser);
        return -1;
    }

    // Check for "--help" or "-h" option
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        helpOptionHandler(&helpOption, NULL, parser);
    } 
    else if (argc >= 2 && strcmp(argv[1], "greet") == 0) {
        // Execute the greet command if present
        greetCommandHandler(&greetCommand, argc - 1, &argv[1], greetCommand.userData);
    } 
    else {
        // Print usage information if the input does not match any command or option
        cli_print_help(parser);
    }

    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 7 : display error with `cli_display_errors`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <string.h>

// Custom error handler function
void customErrorHandler(const CliParser *parser, const char *error, void *userData) {
    (void)parser; 
    (void)userData; 

    fmt_fprintf(stderr, "Custom Error: %s\n", error);
}

void greetCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command; 
    (void)userData; 

    if (argc > 1) {
        fmt_fprintf(stdout, "Hello, %s!\n", argv[1]);
    } 
    else {
        fmt_fprintf(stdout, "Hello, world!\n");
    }
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("ErrorDemoApp");
    if (parser == NULL) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    // Set the custom error handler
    cli_set_error_handler(parser, customErrorHandler);

    CliCommand greetCommand = {
        .name = "greet",
        .handler = greetCommandHandler,
        .description = "Greets the user. Usage: greet [name]",
        .userData = NULL
    };

    // Register the "greet" command
    if (!cli_register_command(parser, &greetCommand)) {
        fmt_fprintf(stderr, "Failed to register 'greet' command\n");
        cli_parser_deallocate(parser);
        return -1;
    }

    // Simulate checking the command input
    if (argc >= 2 && strcmp(argv[1], "greet") == 0) {
        greetCommandHandler(&greetCommand, argc - 1, &argv[1], greetCommand.userData);
    } 
    else {
        char errorMessage[256];
        snprintf(errorMessage, sizeof(errorMessage), "Unrecognized command: '%s'", argc >= 2 ? argv[1] : "none");
        cli_display_error(parser, errorMessage);
    }

    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 8 : Using `cli_find_option` and `cli_print_version`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <string.h>

void versionOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option; // Unused parameter
    (void)value;  // Unused parameter
    (void)userData; // Unused parameter

    cli_print_version(userData, "1.0.0");
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("VersionApp");
    if (!parser) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    CliOption versionOption = {
        .longOpt = "--version",
        .shortOpt = 'v',
        .optionType = CLI_NO_ARG,
        .handler = versionOptionHandler,
        .validator = NULL,
        .description = "Prints the version of the application",
        .userData = NULL
    };

    cli_register_option(parser, &versionOption);

    if (argc > 1) {
        const CliOption *foundOption = cli_find_option(parser, argv[1], '\0');
        if (foundOption && strcmp(foundOption->longOpt, "--version") == 0) {
            foundOption->handler(foundOption, NULL, parser);
        } 
        else {
            fmt_fprintf(stderr, "Unknown option: %s\n", argv[1]);
        }
    } 
    else {
        fmt_fprintf(stdout, "No options provided. Use --version to print the version.\n");
    }

    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 9 : Using `cli_find_command`

```c
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
```

## Example 10 : update cli description with `cli_update_description`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include "string/string.h"

void greetCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command; // Unused parameter
    (void)userData; // Unused parameter

    if (argc > 1) {
        fmt_fprintf(stdout, "Greetings, %s! How are you today?\n", argv[1]);
    } 
    else {
        fmt_fprintf(stdout, "Hello, world! How are you today?\n");
    }
}

int main(int argc, char *argv[]) {
    (void)argc;    // Unused parameter
    (void)argv;    // Unused parameter
    CliParser *parser = cli_parser_create("GreetApp");

    if (parser == NULL) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    CliCommand greetCommand = {
        .name = "greet",
        .handler = greetCommandHandler,
        .description = string_strdup("Greets the user. Usage: greet [name]"),
        .userData = NULL
    };

    cli_register_command(parser, &greetCommand);

    // Update the "greet" command description
    cli_update_description(parser, "greet", "Greet someone by name with a friendly message. Usage: greet [name]", true);

    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 11 : how to send parameters 

**python .\compile.py r subtract 10 20**
**python .\compile.py r add 10 20**
**python .\compile.py r --version**
**python .\compile.py r --help**

```c
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
```