# CLI Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

The CLI Library is a versatile and easy-to-use tool for parsing command-line arguments and options in C programs. It supports various functionalities such as custom commands, options, strict and interactive modes, error handling, and more. This document explains the available functions and their usage.

## Compilation

To compile the Cli library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./cli/cli.c 
g++ -std=c++14 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.cpp

```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Cli library in your project, include the `cli.h` header file in your source code.

in these examples i rewrite cpp example in Bitset code 

```c
#include "cli/cli.h"
```

---

## Function Explanations

### CLI Parser Management

### `CliParser* cli_parser_create(const char *progName)`
- **Description:** Initializes a new CLI parser with the specified program name. The program name is used in usage messages and helps identify the application when displaying help or error messages.
- **Parameters:** 
  - `progName`: The name of the program (e.g., `"MyApp"`).
- **Returns:** A pointer to the `CliParser` structure, or `NULL` if memory allocation fails.
- **Usage Example:** `CliParser *parser = cli_parser_create("MyApp");`

### `void cli_parser_deallocate(CliParser *parser)`
- **Description:** Frees all resources allocated by the CLI parser, including options, commands, and other dynamically allocated memory. This function should be called at the end of the program to prevent memory leaks.
- **Parameters:** 
  - `parser`: The `CliParser` instance to be deallocated.
- **Returns:** Nothing.
- **Usage Example:** `cli_parser_deallocate(parser);`

### Command and Option Registration

### `bool cli_register_command(CliParser *parser, const CliCommand *command)`
- **Description:** Registers a new command with the CLI parser. Commands define specific actions or behaviors in the application that can be triggered by command-line arguments.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `command`: A pointer to the `CliCommand` structure that defines the command.
- **Returns:** `true` if the command is registered successfully, `false` otherwise.
- **Usage Example:** 
  ```c
  CliCommand myCommand = {.name = "greet", .handler = greetCommandHandler};
  cli_register_command(parser, &myCommand);
  ```

### `bool cli_unregister_command(CliParser *parser, const char *name)`
- **Description:** Removes a previously registered command from the parser.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `name`: The name of the command to be removed.
- **Returns:** `true` if the command is unregistered successfully, `false` otherwise.
- **Usage Example:** `cli_unregister_command(parser, "greet");`

### `bool cli_register_option(CliParser *parser, const CliOption *option)`
- **Description:** Registers a new option with the CLI parser. Options allow users to modify the behavior of commands or the application itself via command-line arguments.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `option`: A pointer to the `CliOption` structure that defines the option.
- **Returns:** `true` if the option is registered successfully, `false` otherwise.
- **Usage Example:** 
  ```c
  CliOption helpOption = {.longOpt = "--help", .shortOpt = 'h'};
  cli_register_option(parser, &helpOption);
  ```

### `bool cli_unregister_option(CliParser *parser, const char *longOpt, char shortOpt)`
- **Description:** Unregisters an option from the parser using its long or short identifier.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `longOpt`: The long option name (e.g., `"--help"`).
  - `shortOpt`: The short option character (e.g., `'h'`).
- **Returns:** `true` if the option is unregistered successfully, `false` otherwise.
- **Usage Example:** `cli_unregister_option(parser, "--help", 'h');`

### Command and Option Handling

### `const CliOption* cli_find_option(const CliParser *parser, const char *longOpt, char shortOpt)`
- **Description:** Searches for and returns a pointer to an option by its long name or short character.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `longOpt`: The long option name (e.g., `"--help"`).
  - `shortOpt`: The short option character (e.g., `'h'`).
- **Returns:** A pointer to the `CliOption` if found, or `NULL` if not found.
- **Usage Example:** `const CliOption *opt = cli_find_option(parser, "--help", 'h');`

### `const CliCommand* cli_find_command(const CliParser *parser, const char *name)`
- **Description:** Searches for and returns a pointer to a command by its name.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `name`: The name of the command to find (e.g., `"greet"`).
- **Returns:** A pointer to the `CliCommand` if found, or `NULL` if not found.
- **Usage Example:** `const CliCommand *cmd = cli_find_command(parser, "greet");`

### `void cli_set_default_command_handler(CliParser *parser, CliCommandHandler handler)`
- **Description:** Sets a default command handler to be used when no specific command is matched.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `handler`: A function pointer to the command handler.
- **Returns:** Nothing.
- **Usage Example:** 
  ```c
  void defaultHandler(const CliCommand *cmd, int argc, char *argv[], void *userData) {
      printf("Default command executed.\n");
  }
  cli_set_default_command_handler(parser, defaultHandler);
  ```

### `void cli_add_option_group(CliParser *parser, const char *groupName, const CliOption *options, size_t numOptions)`
- **Description:** Groups related options together under a named group for better organization in help messages.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `groupName`: The name of the option group.
  - `options`: An array of `CliOption` structures representing the options in the group.
  - `numOptions`: The number of options in the group.
- **Returns:** Nothing.
- **Usage Example:** 
  ```c
  CliOption serverOptions[] = {
      {.longOpt = "--port", .shortOpt = 'p'},
      {.longOpt = "--verbose", .shortOpt = 'v'}
  };
  cli_add_option_group(parser, "Server Options", serverOptions, 2);
  ```

### `void cli_remove_option_group(CliParser *parser, const char *groupName)`
- **Description:** Removes a previously defined option group from the parser.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `groupName`: The name of the option group to remove.
- **Returns:** Nothing.
- **Usage Example:** `cli_remove_option_group(parser, "Server Options");`

### Error Handling

### `void cli_set_error_handler(CliParser *parser, CliErrorHandler handler)`
- **Description:** Sets a custom function to handle parsing errors.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `handler`: A function pointer to the error handler.
- **Returns:** Nothing.
- **Usage Example:** 
  ```c
  void customErrorHandler(const CliParser *parser, const char *error, void *userData) {
      fprintf(stderr, "Error: %s\n", error);
  }
  cli_set_error_handler(parser, customErrorHandler);
  ```

### `void cli_display_error(const CliParser *parser, const char *error)`
- **Description:** Displays an error message using the configured error handler. If no custom handler is set, the error is printed to `stderr`.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `error`: The error message to be displayed.
- **Returns:** Nothing.
- **Usage Example:** `cli_display_error(parser, "Invalid command.");`

### `CliError cli_get_last_error(const CliParser *parser)`
- **Description:** Retrieves the last error that occurred during parsing.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
- **Returns:** The last `CliError` encountered.
- **Usage Example:** `CliError lastError = cli_get_last_error(parser);`

### Parsing Arguments

### `CliStatusCode cli_parse_args(CliParser *parser, int argc, char *argv[])`
- **Description:** Parses command-line arguments according to the configured options and commands. This function is the core of the CLI parsing process.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `argc`: The argument count (number of command-line arguments).
  - `argv`: The argument vector (array of command-line arguments).
- **Returns:** A status code (`CliStatusCode`) indicating success or the type of error encountered.
- **Usage Example:** `CliStatusCode status = cli_parse_args(parser, argc, argv);`

### `bool cli_parse_args_with_delimiter(CliParser *parser, int argc, char *argv[], const char *delimiter)`
- **Description:** Parses command-line arguments using a specified delimiter, allowing for more complex argument structures

.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `argc`: The argument count.
  - `argv`: The argument vector.
  - `delimiter`: The delimiter used to separate command arguments.
- **Returns:** `true` if parsing succeeds, `false` otherwise.
- **Usage Example:** `cli_parse_args_with_delimiter(parser, argc, argv, ",");`

### `bool cli_process_option_group(CliParser *parser, const char *groupName, int argc, char *argv[])`
- **Description:** Parses command-line arguments for a specific option group.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `groupName`: The name of the option group to process.
  - `argc`: The argument count.
  - `argv`: The argument vector.
- **Returns:** `true` if the options in the group are processed successfully, `false` otherwise.
- **Usage Example:** `cli_process_option_group(parser, "Server Options", argc, argv);`

### Miscellaneous Functions

### `void cli_print_help(const CliParser *parser)`
- **Description:** Prints a detailed help message, including descriptions of all options and commands. This function is typically used when the user requests help with a `--help` or `-h` option.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
- **Returns:** Nothing.
- **Usage Example:** `cli_print_help(parser);`

### `void cli_print_version(const CliParser *parser, const char *version)`
- **Description:** Prints the version information of the application.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `version`: The version string to be displayed.
- **Returns:** Nothing.
- **Usage Example:** `cli_print_version(parser, "1.0.0");`

### `void cli_set_custom_usage(CliParser *parser, const char *usage)`
- **Description:** Allows setting a custom usage message, overriding the default usage message generated by the parser.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `usage`: The custom usage message.
- **Returns:** Nothing.
- **Usage Example:** 
  ```c
  cli_set_custom_usage(parser, "Usage: MyApp [options]");
  ```

### `void cli_enable_strict_mode(CliParser *parser, bool enable)`
- **Description:** Enables or disables strict mode, where unrecognized options trigger errors instead of being ignored. This mode is useful for ensuring that only valid options are passed to the program.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `enable`: A boolean value (`true` to enable, `false` to disable).
- **Returns:** Nothing.
- **Usage Example:** `cli_enable_strict_mode(parser, true);`

### `void cli_update_description(CliParser *parser, const char *name, const char *newDescription, bool isCommand)`
- **Description:** Updates the description of an option or command. This can be useful for dynamic help messages or changing descriptions based on context.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `name`: The name of the option or command.
  - `newDescription`: The new description string.
  - `isCommand`: A boolean value indicating whether the name refers to a command (`true`) or an option (`false`).
- **Returns:** Nothing.
- **Usage Example:** 
  ```c
  cli_update_description(parser, "greet", "Greets the user in a friendly manner", true);
  ```

### Interactive Mode and Pipelining

### `void cli_enter_interactive_mode(CliParser *parser, const char *prompt)`
- **Description:** Enters an interactive mode where commands can be input repeatedly. This mode is useful for applications that require continuous user interaction.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `prompt`: The prompt string to display in the interactive mode.
- **Returns:** Nothing.
- **Usage Example:** 
  ```c
  cli_enter_interactive_mode(parser, ">");
  ```

### `void cli_enable_pipelining(CliParser *parser, bool enable)`
- **Description:** Enables or disables pipelining, allowing the output of one command to be used as input for another, enhancing scripting capabilities.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `enable`: A boolean value (`true` to enable, `false` to disable).
- **Returns:** Nothing.
- **Usage Example:** `cli_enable_pipelining(parser, true);`

### Aliases and Dependencies

### `bool cli_register_command_alias(CliParser *parser, const char *commandName, const char *alias)`
- **Description:** Registers an alias for a command, allowing users to use alternative names for commands.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `commandName`: The name of the command to alias.
  - `alias`: The alias name.
- **Returns:** `true` if the alias is registered successfully, `false` otherwise.
- **Usage Example:** `cli_register_command_alias(parser, "greet", "hello");`

### `bool cli_add_option_alias(CliParser *parser, const char *optionName, const char *alias)`
- **Description:** Registers an alias for an option, allowing users to use alternative names for options.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `optionName`: The name of the option to alias.
  - `alias`: The alias name.
- **Returns:** `true` if the alias is registered successfully, `false` otherwise.
- **Usage Example:** `cli_add_option_alias(parser, "--verbose", "-v");`

### `bool cli_set_option_dependencies(CliParser *parser, const char *longOpt, char shortOpt, const char *dependsOnLongOpt, char dependsOnShortOpt)`
- **Description:** Defines dependencies between options, where the presence of one option requires the presence of another.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `longOpt`: The long option name that has a dependency.
  - `shortOpt`: The short option character that has a dependency.
  - `dependsOnLongOpt`: The long option name that is required.
  - `dependsOnShortOpt`: The short option character that is required.
- **Returns:** `true` if the dependency is set successfully, `false` otherwise.
- **Usage Example:** 
  ```c
  cli_set_option_dependencies(parser, "--verbose", 'v', "--log-file", 'l');
  ```

### Validation and Confirmation

### `bool cli_validate_option_argument(const CliOption *option, const char *value)`
- **Description:** Validates the argument passed to an option, typically used to check if the value meets certain criteria (e.g., a valid port number).
- **Parameters:** 
  - `option`: The `CliOption` to validate.
  - `value`: The argument value to validate.
- **Returns:** `true` if the argument is valid, `false` otherwise.
- **Usage Example:** `cli_validate_option_argument(option, "8080");`

### `bool cli_prompt_confirmation(const char *promptMessage)`
- **Description:** Prompts the user for confirmation with a customizable message and waits for a Y/N response.
- **Parameters:** 
  - `promptMessage`: The message to display in the confirmation prompt.
- **Returns:** `true` if the user confirms (Y), `false` otherwise (N).
- **Usage Example:** 
  ```c
  bool confirmed = cli_prompt_confirmation("Are you sure you want to continue? (Y/N)");
  ```

### Hooks for Execution

### `void cli_set_pre_execution_hook(CliParser *parser, CliPreExecutionHook hook)`
- **Description:** Registers a function to be called before any command execution, allowing for preparation or validation tasks.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `hook`: A function pointer to the pre-execution hook.
- **Returns:** Nothing.
- **Usage Example:** 
  ```c
  void preExecutionHook(const CliParser *parser) {
      printf("Preparing to execute a command...\n");
  }
  cli_set_pre_execution_hook(parser, preExecutionHook);
  ```

### `void cli_set_post_execution_hook(CliParser *parser, CliPostExecutionHook hook)`
- **Description:** Registers a function to be called after any command execution, allowing for cleanup or logging tasks.
- **Parameters:** 
  - `parser`: The `CliParser` instance.
  - `hook`: A function pointer to the post-execution hook.
- **Returns:** Nothing.
- **Usage Example:** 
  ```c
  void postExecutionHook(const CliParser *parser) {
      printf("Command execution completed.\n");
  }
  cli_set_post_execution_hook(parser, postExecutionHook);
  ```

---
### Examples 


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
    } 
    else {
        fmt_printf("No command provided. Use '--help' for usage information.\n");
    }

    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 12 : register gree with cli parser 

**python compile.py r --version**
**python compile.py r greet amin**

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

// Command Handlers
void greetCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)userData;
    (void)command;
    fmt_printf("Hello, %s!\n", argc > 1 ? argv[1] : "World");
}

void versionCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)userData;
    (void)command;
    (void)argc;
    (void)argv;
    fmt_printf("Version 1.0.0\n");
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("ExampleApp");
    if (!parser) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return 1;
    }

    CliCommand greetCommand = {
        .name = "greet",
        .handler = greetCommandHandler,
        .description = "Greets a user",
        .userData = NULL
    };

    CliCommand versionCommand = {
        .name = "--version",
        .handler = versionCommandHandler,
        .description = "Shows version",
        .userData = NULL
    };

    if (!cli_register_command(parser, &greetCommand)) {
        fmt_fprintf(stderr, "Failed to register 'greet' command\n");
    }

    if (!cli_register_command(parser, &versionCommand)) {
        fmt_fprintf(stderr, "Failed to register '--version' command\n");
    }

    cli_parse_args(parser, argc, argv);

    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 13 : interactive mode 

**python compile.py r calculate 10 20**
**python compile.py r --interactive**

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <stdlib.h>

// Command Handler
void calculateCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)userData;
    (void)command;
    if (argc != 3) {
        fmt_printf("Usage: calculate <number1> <number2>\n");
        return;
    }

    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);
    fmt_printf("Sum: %d\n", num1 + num2);
}

// Option Handler for --interactive mode
void interactiveOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;
    CliParser *parser = (CliParser *)userData;

    fmt_printf("Entering interactive mode. Type 'exit' to quit.\n");
    cli_enter_interactive_mode(parser, "> ");
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("InteractiveApp");
    if (!parser) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return 1;
    }

    CliCommand calculateCommand = {
        .name = "calculate",
        .handler = calculateCommandHandler,
        .description = "Performs a simple addition",
        .userData = NULL
    };

    CliOption interactiveOption = {
        .longOpt = "--interactive",
        .optionType = CLI_NO_ARG,
        .handler = interactiveOptionHandler,
        .description = "Enters interactive mode",
        .userData = (void *)parser
    };

    if (!cli_register_command(parser, &calculateCommand)) {
        fmt_fprintf(stderr, "Failed to register 'calculate' command\n");
    }

    if (!cli_register_option(parser, &interactiveOption)) {
        fmt_fprintf(stderr, "Failed to register '--interactive' option\n");
    }

    cli_parse_args(parser, argc, argv);

    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 14 : Using Option Groups and Pipelining

**This example demonstrates how to use option groups and enable pipelining for advanced command-line parsing scenarios**

`this will enable verbose mode in your program`
**python compile.py r --versbose**
`this will proccess command data`
**python compile.py r process one two**

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <stdlib.h>

// Option handler for setting verbose mode
void verboseOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value; 
    bool *verboseMode = (bool *)userData;
    *verboseMode = true;
    fmt_printf("Verbose mode enabled\n");
}

// Command handler for a 'process' command
void processCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    (void)userData; 
    fmt_printf("Processing with %d arguments\n", argc);
    for (int i = 0; i < argc; ++i) {
        fmt_printf("Arg %d: %s\n", i + 1, argv[i]);
    }
}

int main(int argc, char *argv[]) {
    bool verboseMode = false;
    CliParser *parser = cli_parser_create("PipelineApp");

    CliOption verboseOption = {
        .longOpt = "--verbose",
        .shortOpt = 'v',
        .optionType = CLI_NO_ARG,
        .handler = verboseOptionHandler,
        .description = "Enable verbose output",
        .userData = &verboseMode
    };

    CliCommand processCommand = {
        .name = "process",
        .handler = processCommandHandler,
        .description = "Process data",
        .userData = NULL
    };

    cli_register_option(parser, &verboseOption);
    cli_register_command(parser, &processCommand);

    // Enable pipelining to allow command outputs to be used as inputs for other commands
    cli_enable_pipelining(parser, true);
    cli_parse_args(parser, argc, argv);

    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 15: Interactive Mode and Command Aliases

`entering an interactive mode and using command aliases to provide alternative names for commands`
**python compile.py r**
```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <stdlib.h>

// Command handler for exiting the application
void exitCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    (void)argc;
    (void)argv;
    (void)userData;
    fmt_printf("Exiting application.\n");
    exit(0);
}

// Interactive mode prompt
void enterInteractiveMode(CliParser *parser) {
    fmt_printf("Interactive mode. Type 'exit' or 'quit' to exit.\n");
    cli_enter_interactive_mode(parser, ">");
}

int main() {
    CliParser *parser = cli_parser_create("InteractiveCLI");

    CliCommand exitCommand = {
        .name = "exit",
        .handler = exitCommandHandler,
        .description = "Exit the application",
        .userData = NULL
    };

    cli_register_command(parser, &exitCommand);

    // Register an alias for the 'exit' command
    cli_register_command_alias(parser, "exit", "quit");

    // Enter interactive mode immediately for demonstration purposes
    enterInteractiveMode(parser);

    cli_parser_deallocate(parser);
    return 0;
}
```

## Example 16 : running server with different mode 

`start server with port`
**python .\compile.py r start --port 8080**

`enabling verbose mode`
**python .\compile.py r start -v**
**python .\compile.py r start --port 8080 --verbose**

`Specifiyin a log file`
**python .\compile.py r start --port 8080 --log-file "server.log"**

`Combining Multiple Options`
**python .\compile.py r start --verbose --port 8080 --log-file "server.log"**
```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

int globalPort = 0; // Global variable to store the port
bool verboseMode = false; // Global variable to store the verbose flag

// Custom validator for the port option
bool validatePort(const char *value, void *userData) {
    (void)userData;
    long port = strtol(value, NULL, 10);
    if (port <= 0 || port > 65535) {
        fmt_printf("Port must be between 1 and 65535\n");
        return false;
    }
    return true;
}

// Handler for the --port option
void portOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)userData;
    (void)option;
    long port = strtol(value, NULL, 10);
    globalPort = (int)port;
    fmt_printf("Port set to %d\n", globalPort);
}

// Handler for the --verbose option
void verboseOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)userData;
    (void)option;
    (void)value;
    verboseMode = true;
    fmt_printf("Verbose mode enabled\n");
}

// Handler for the --log-file option
void logFileOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)userData;
    (void)option;
    if (value == NULL || strlen(value) == 0) {
        fmt_printf("Log file name must be provided\n");
        exit(EXIT_FAILURE);
    }
    fmt_printf("Logging to file: %s\n", value);
}

// Handler for the server start command
void startServerCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    (void)userData;
    (void)argc;
    (void)argv;
    fmt_printf("Server started on port %d...\n", globalPort);
    if (verboseMode) {
        fmt_printf("Verbose mode is ON\n");
    }
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("ServerApp");

    // Register the start server command
    CliCommand startServerCommand = {
        .name = "start",
        .handler = startServerCommandHandler,
        .description = "Starts the server",
    };

    CliOption serverOptions[] = {
        {.longOpt = "--port", .shortOpt = 'p', .optionType = CLI_REQUIRED_ARG, .handler = portOptionHandler, .validator = validatePort, .description = "Set server port"},
        {.longOpt = "--verbose", .shortOpt = 'v', .optionType = CLI_NO_ARG, .handler = verboseOptionHandler, .description = "Enable verbose output"},
        {.longOpt = "--log-file", .shortOpt = 'l', .optionType = CLI_REQUIRED_ARG, .handler = logFileOptionHandler, .description = "Specify log file"},
    };

    cli_register_command(parser, &startServerCommand);
    cli_add_option_group(parser, "Server Options", serverOptions, sizeof(serverOptions) / sizeof(CliOption));
    cli_parse_args(parser, argc, argv);

    cli_parser_deallocate(parser);
    return 0;
}
```


---

## License

This project is open-source and available under [ISC License].