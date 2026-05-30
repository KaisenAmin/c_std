# CLI Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

The CLI Library is a versatile and easy-to-use tool for parsing command-line arguments and options in C programs. It supports various functionalities such as custom commands, options, strict and interactive modes, error handling, and more. This document explains the available functions and their usage.

## Compilation

To compile the CLI library along with your main program, use the following GCC command:

```bash
gcc -std=c17 -O2 -Wall -Wextra -o main ./main.c ./cli/cli.c ./fmt/fmt.c ./string/std_string.c ./encoding/encoding.c -I.
```

## Usage

To use the Cli library in your project, include the `cli.h` header file in your source code.

---

## Function Explanations

### CLI Parser Management

### `CliParser* cli_parser_create(const char *progName)`
**Purpose**: Initializes a new CLI parser with the specified program name. The program name is used in usage messages and helps identify the application when displaying help or error messages.
**Parameters**:
  - `progName`: The name of the program (e.g., `"MyApp"`).
**Return Value**: A pointer to the `CliParser` structure, or `NULL` if memory allocation fails.
**Usage Case**: `CliParser *parser = cli_parser_create("MyApp");`

### `void cli_parser_deallocate(CliParser *parser)`
**Purpose**: Frees all resources allocated by the CLI parser, including options, commands, and other dynamically allocated memory. This function should be called at the end of the program to prevent memory leaks.
**Parameters**:
  - `parser`: The `CliParser` instance to be deallocated.
**Return Value**: None.
**Usage Case**: `cli_parser_deallocate(parser);`

### Command and Option Registration

### `bool cli_register_command(CliParser *parser, const CliCommand *command)`
**Purpose**: Registers a new command with the CLI parser. Commands define specific actions or behaviors in the application that can be triggered by command-line arguments.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `command`: A pointer to the `CliCommand` structure that defines the command.
**Return Value**: `true` if the command is registered successfully, `false` otherwise.
**Usage Case**:
  ```c
  CliCommand myCommand = {.name = "greet", .handler = greetCommandHandler};
  cli_register_command(parser, &myCommand);
  ```

### `bool cli_unregister_command(CliParser *parser, const char *name)`
**Purpose**: Removes a previously registered command from the parser.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `name`: The name of the command to be removed.
**Return Value**: `true` if the command is unregistered successfully, `false` otherwise.
**Usage Case**: `cli_unregister_command(parser, "greet");`

### `bool cli_register_option(CliParser *parser, const CliOption *option)`
**Purpose**: Registers a new option with the CLI parser. Options allow users to modify the behavior of commands or the application itself via command-line arguments.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `option`: A pointer to the `CliOption` structure that defines the option.
**Return Value**: `true` if the option is registered successfully, `false` otherwise.
**Usage Case**:
  ```c
  CliOption helpOption = {.longOpt = "--help", .shortOpt = 'h'};
  cli_register_option(parser, &helpOption);
  ```

### `bool cli_unregister_option(CliParser *parser, const char *longOpt, char shortOpt)`
**Purpose**: Unregisters an option from the parser using its long or short identifier.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `longOpt`: The long option name (e.g., `"--help"`).
  - `shortOpt`: The short option character (e.g., `'h'`).
**Return Value**: `true` if the option is unregistered successfully, `false` otherwise.
**Usage Case**: `cli_unregister_option(parser, "--help", 'h');`

### Command and Option Handling

### `const CliOption* cli_find_option(const CliParser *parser, const char *longOpt, char shortOpt)`
**Purpose**: Searches for and returns a pointer to an option by its long name or short character.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `longOpt`: The long option name (e.g., `"--help"`).
  - `shortOpt`: The short option character (e.g., `'h'`).
**Return Value**: A pointer to the `CliOption` if found, or `NULL` if not found.
**Usage Case**: `const CliOption *opt = cli_find_option(parser, "--help", 'h');`

### `const CliCommand* cli_find_command(const CliParser *parser, const char *name)`
**Purpose**: Searches for and returns a pointer to a command by its name.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `name`: The name of the command to find (e.g., `"greet"`).
**Return Value**: A pointer to the `CliCommand` if found, or `NULL` if not found.
**Usage Case**: `const CliCommand *cmd = cli_find_command(parser, "greet");`

### `void cli_set_default_command_handler(CliParser *parser, CliCommandHandler handler)`
**Purpose**: Sets a default command handler to be used when no specific command is matched.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `handler`: A function pointer to the command handler.
**Return Value**: None.
**Usage Case**:
  ```c
  void defaultHandler(const CliCommand *cmd, int argc, char *argv[], void *userData) {
      printf("Default command executed.\n");
  }
  cli_set_default_command_handler(parser, defaultHandler);
  ```

### `void cli_add_option_group(CliParser *parser, const char *groupName, const CliOption *options, size_t numOptions)`
**Purpose**: Groups related options together under a named group for better organization in help messages.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `groupName`: The name of the option group.
  - `options`: An array of `CliOption` structures representing the options in the group.
  - `numOptions`: The number of options in the group.
**Return Value**: None.
**Usage Case**:
  ```c
  CliOption serverOptions[] = {
      {.longOpt = "--port", .shortOpt = 'p'},
      {.longOpt = "--verbose", .shortOpt = 'v'}
  };
  cli_add_option_group(parser, "Server Options", serverOptions, 2);
  ```

### `void cli_remove_option_group(CliParser *parser, const char *groupName)`
**Purpose**: Removes a previously defined option group from the parser.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `groupName`: The name of the option group to remove.
**Return Value**: None.
**Usage Case**: `cli_remove_option_group(parser, "Server Options");`

### Error Handling

### `void cli_set_error_handler(CliParser *parser, CliErrorHandler handler)`
**Purpose**: Sets a custom function to handle parsing errors.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `handler`: A function pointer to the error handler.
**Return Value**: None.
**Usage Case**:
  ```c
  void customErrorHandler(const CliParser *parser, const char *error, void *userData) {
      fprintf(stderr, "Error: %s\n", error);
  }
  cli_set_error_handler(parser, customErrorHandler);
  ```

### `void cli_display_error(const CliParser *parser, const char *error)`
**Purpose**: Displays an error message using the configured error handler. If no custom handler is set, the error is printed to `stderr`.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `error`: The error message to be displayed.
**Return Value**: None.
**Usage Case**: `cli_display_error(parser, "Invalid command.");`

### `CliError cli_get_last_error(const CliParser *parser)`
**Purpose**: Retrieves the last error that occurred during parsing.
**Parameters**:
  - `parser`: The `CliParser` instance.
**Return Value**: The last `CliError` encountered.
**Usage Case**: `CliError lastError = cli_get_last_error(parser);`

### Parsing Arguments

### `CliStatusCode cli_parse_args(CliParser *parser, int argc, char *argv[])`
**Purpose**: Parses command-line arguments according to the configured options and commands. This function is the core of the CLI parsing process.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `argc`: The argument count (number of command-line arguments).
  - `argv`: The argument vector (array of command-line arguments).
**Return Value**: A status code (`CliStatusCode`) indicating success or the type of error encountered.
**Usage Case**: `CliStatusCode status = cli_parse_args(parser, argc, argv);`

### `bool cli_parse_args_with_delimiter(CliParser *parser, int argc, char *argv[], const char *delimiter)`
**Purpose**: Parses command-line arguments using a specified delimiter, allowing for more complex argument structures.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `argc`: The argument count.
  - `argv`: The argument vector.
  - `delimiter`: The delimiter used to separate command arguments.
**Return Value**: `true` if parsing succeeds, `false` otherwise.
**Usage Case**: `cli_parse_args_with_delimiter(parser, argc, argv, ",");`

### `bool cli_process_option_group(CliParser *parser, const char *groupName, int argc, char *argv[])`
**Purpose**: Parses command-line arguments for a specific option group.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `groupName`: The name of the option group to process.
  - `argc`: The argument count.
  - `argv`: The argument vector.
**Return Value**: `true` if the options in the group are processed successfully, `false` otherwise.
**Usage Case**: `cli_process_option_group(parser, "Server Options", argc, argv);`

### Miscellaneous Functions

### `void cli_print_help(const CliParser *parser)`
**Purpose**: Prints a detailed help message, including descriptions of all options and commands. This function is typically used when the user requests help with a `--help` or `-h` option.
**Parameters**:
  - `parser`: The `CliParser` instance.
**Return Value**: None.
**Usage Case**: `cli_print_help(parser);`

### `void cli_print_version(const CliParser *parser, const char *version)`
**Purpose**: Prints the version information of the application.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `version`: The version string to be displayed.
**Return Value**: None.
**Usage Case**: `cli_print_version(parser, "1.0.0");`

### `void cli_set_custom_usage(CliParser *parser, const char *usage)`
**Purpose**: Allows setting a custom usage message, overriding the default usage message generated by the parser.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `usage`: The custom usage message.
**Return Value**: None.
**Usage Case**:
  ```c
  cli_set_custom_usage(parser, "Usage: MyApp [options]");
  ```

### `void cli_enable_strict_mode(CliParser *parser, bool enable)`
**Purpose**: Enables or disables strict mode, where unrecognized options trigger errors instead of being ignored. This mode is useful for ensuring that only valid options are passed to the program.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `enable`: A boolean value (`true` to enable, `false` to disable).
**Return Value**: None.
**Usage Case**: `cli_enable_strict_mode(parser, true);`

### `void cli_update_description(CliParser *parser, const char *name, const char *newDescription, bool isCommand)`
**Purpose**: Updates the description of an option or command. This can be useful for dynamic help messages or changing descriptions based on context.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `name`: The name of the option or command.
  - `newDescription`: The new description string.
  - `isCommand`: A boolean value indicating whether the name refers to a command (`true`) or an option (`false`).
**Return Value**: None.
**Usage Case**:
  ```c
  cli_update_description(parser, "greet", "Greets the user in a friendly manner", true);
  ```

### Interactive Mode and Pipelining

### `void cli_enter_interactive_mode(CliParser *parser, const char *prompt)`
**Purpose**: Enters an interactive mode where commands can be input repeatedly. This mode is useful for applications that require continuous user interaction.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `prompt`: The prompt string to display in the interactive mode.
**Return Value**: None.
**Usage Case**:
  ```c
  cli_enter_interactive_mode(parser, ">");
  ```

### Aliases and Dependencies

### `bool cli_register_command_alias(CliParser *parser, const char *commandName, const char *alias)`
**Purpose**: Registers an alias for a command, allowing users to use alternative names for commands.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `commandName`: The name of the command to alias.
  - `alias`: The alias name.
**Return Value**: `true` if the alias is registered successfully, `false` otherwise.
**Usage Case**: `cli_register_command_alias(parser, "greet", "hello");`

### `bool cli_add_option_alias(CliParser *parser, const char *optionName, const char *alias)`
**Purpose**: Registers an alias for an option, allowing users to use alternative names for options.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `optionName`: The name of the option to alias.
  - `alias`: The alias name.
**Return Value**: `true` if the alias is registered successfully, `false` otherwise.
**Usage Case**: `cli_add_option_alias(parser, "--verbose", "-v");`

### `bool cli_set_option_dependencies(CliParser *parser, const char *longOpt, char shortOpt, const char *dependsOnLongOpt, char dependsOnShortOpt)`
**Purpose**: Defines dependencies between options, where the presence of one option requires the presence of another.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `longOpt`: The long option name that has a dependency.
  - `shortOpt`: The short option character that has a dependency.
  - `dependsOnLongOpt`: The long option name that is required.
  - `dependsOnShortOpt`: The short option character that is required.
**Return Value**: `true` if the dependency is set successfully, `false` otherwise.
**Usage Case**:
  ```c
  cli_set_option_dependencies(parser, "--verbose", 'v', "--log-file", 'l');
  ```

### Validation and Confirmation

### `bool cli_validate_option_argument(const CliOption *option, const char *value)`
**Purpose**: Validates the argument passed to an option, typically used to check if the value meets certain criteria (e.g., a valid port number).
**Parameters**:
  - `option`: The `CliOption` to validate.
  - `value`: The argument value to validate.
**Return Value**: `true` if the argument is valid, `false` otherwise.
**Usage Case**: `cli_validate_option_argument(option, "8080");`

### `bool cli_prompt_confirmation(const char *promptMessage)`
**Purpose**: Prompts the user for confirmation with a customizable message and waits for a Y/N response.
**Parameters**:
  - `promptMessage`: The message to display in the confirmation prompt.
**Return Value**: `true` if the user confirms (Y), `false` otherwise (N).
**Usage Case**:
  ```c
  bool confirmed = cli_prompt_confirmation("Are you sure you want to continue? (Y/N)");
  ```

### Hooks for Execution

### `void cli_set_pre_execution_hook(CliParser *parser, CliPreExecutionHook hook)`
**Purpose**: Registers a function to be called before any command execution, allowing for preparation or validation tasks.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `hook`: A function pointer to the pre-execution hook.
**Return Value**: None.
**Usage Case**:
  ```c
  void preExecutionHook(const CliParser *parser) {
      printf("Preparing to execute a command...\n");
  }
  cli_set_pre_execution_hook(parser, preExecutionHook);
  ```

### `void cli_set_post_execution_hook(CliParser *parser, CliPostExecutionHook hook)`
**Purpose**: Registers a function to be called after any command execution, allowing for cleanup or logging tasks.
**Parameters**:
  - `parser`: The `CliParser` instance.
  - `hook`: A function pointer to the post-execution hook.
**Return Value**: None.
**Usage Case**:
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
    else {
        fmt_printf("Parser Create Successfully\n");
    }
    // Deallocate the CLI parser and all associated resources
    cli_parser_deallocate(parser);
    return 0;
}
```
**Result**
```
Parser Create Successfully
```

---

## Example 2 : setting custome usage message `cli_set_custom_usage`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"


void help_option_handler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;
    if (!userData) {
        fmt_fprintf(stderr, "Error: userData is NULL in helpOption handler.\n");
        return;
    }
    CliParser *parser = (CliParser *)userData;
    cli_print_help(parser);
}

void version_option_handler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;
    if (!userData) {
        fmt_fprintf(stderr, "Error: userData is NULL in versionOption handler.\n");
        return;
    }
    fmt_printf("Version: MyApp v1.0.0\n");
}

int main(int argc, char *argv[]) {
    fmt_printf("Creating CLI parser...\n");
    CliParser *parser = cli_parser_create("MyApp");

    if (!parser) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }
    fmt_printf("CLI parser created successfully.\n");

    const char *customUsage = "Usage: MyApp [options]\n"
                              "Options:\n"
                              "  -h, --help    Display this help message\n"
                              "  -v, --version Display version information\n";
    fmt_printf("Setting custom usage message...\n");
    cli_set_custom_usage(parser, customUsage);
    fmt_printf("Custom usage message set successfully.\n");

    // Help Option
    CliOption helpOption = {
        .longOpt = "--help",
        .shortOpt = 'h',
        .optionType = CLI_NO_ARG,
        .handler = help_option_handler,
        .validator = NULL,
        .description = "Display this help message",
        .userData = parser,
        .customErrorMessage = NULL,
        .optionValidator = NULL,
        .validationErrorMessage = NULL
    };

    // Version Option
    CliOption versionOption = {
        .longOpt = "--version",
        .shortOpt = 'v',
        .optionType = CLI_NO_ARG,
        .handler = version_option_handler,
        .validator = NULL,
        .description = "Display version information",
        .userData = parser,
        .customErrorMessage = NULL,
        .optionValidator = NULL,
        .validationErrorMessage = NULL
    };

    cli_register_option(parser, &helpOption);
    cli_register_option(parser, &versionOption);
    fmt_printf("Options registered successfully.\n");

    fmt_printf("Testing argument parsing...\n");
    CliStatusCode status = cli_parse_args(parser, argc, argv);

    if (status != CLI_SUCCESS) {
        fmt_fprintf(stderr, "Failed to parse arguments. Error: %d\n", status);
    } 
    else {
        fmt_printf("Arguments parsed successfully.\n");
    }

    cli_parser_deallocate(parser);
    return 0;
}
```
**Result**
```
./main --help

Creating CLI parser...
CLI parser created successfully.
Setting custom usage message...
Custom usage message set successfully.
Options registered successfully.
Testing argument parsing...
Options:
  --help, h	Display this help message
  --version, v	Display version information
Arguments parsed successfully.

./main --version

Creating CLI parser...
CLI parser created successfully.
Setting custom usage message...
Custom usage message set successfully.
Options registered successfully.
Testing argument parsing...
Version: MyApp v1.0.0
Arguments parsed successfully.

./main

Creating CLI parser...
CLI parser created successfully.
Setting custom usage message...
Custom usage message set successfully.
Options registered successfully.
Testing argument parsing...
Arguments parsed successfully.
```

---

## Example 3: Enabling Strict Mode in CLI Parser with 1 `cli_enable_strict_mode`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"


void greet_option_handler(const CliOption *option, const char *value, void *userData) {
    if (value == NULL) {
        fmt_fprintf(stderr, "Error: Missing value for --greet\n");
        return;
    }
    const char *appName = (const char *)userData;
    fmt_printf("Hello, %s! Welcome to %s\n", value, appName ? appName : "Unknown App");
}

void version_option_handler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;
    const char *appName = (const char *)userData;
    fmt_printf("%s version 1.0.0\n", appName ? appName : "Unknown App");
}

void help_option_handler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;
    CliParser *parser = (CliParser *)userData;
    cli_print_help(parser);
}

void unrecognized_option_handler(const CliParser *parser, const char *arg, void *userData) {
    fmt_fprintf(stderr, "Error: Unrecognized option '%s'.\n", arg);
    fmt_printf("Use '--help' to see available options.\n");
    cli_print_help(parser);
}

int main(int argc, char *argv[]) {
    fmt_printf("Creating CLI parser...\n");

    CliParser *parser = cli_parser_create("MyStrictApp");
    if (parser == NULL) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    cli_enable_strict_mode(parser, true);
    const char *customUsage = "Usage: MyStrictApp [options] [commands]\n"
                          "\nOptions:\n"
                          "  --greet <name>  Greet a user by name\n"
                          "  --version       Show version information\n"
                          "  -h, --help      Display this help message\n"
                          "\nCommands:\n"
                          "  (No commands are currently supported)\n";
    cli_set_custom_usage(parser, customUsage);


    // Register the --greet option
    CliOption greetOption = {
        .longOpt = "--greet",
        .shortOpt = '\0',
        .optionType = CLI_REQUIRED_ARG,
        .handler = greet_option_handler,
        .description = "Greet a user by name",
        .userData = (void *)"MyStrictApp", 
    };

    // Register the --version option
    CliOption versionOption = {
        .longOpt = "--version",
        .shortOpt = '\0',
        .optionType = CLI_NO_ARG,
        .handler = version_option_handler,
        .description = "Show version information",
        .userData = (void *)"MyStrictApp", 
    };

    // Register the --help option
    CliOption helpOption = {
        .longOpt = "--help",
        .shortOpt = 'h',
        .optionType = CLI_NO_ARG,
        .handler = help_option_handler,
        .description = "Display this help message",
        .userData = parser,
    };

    cli_register_option(parser, &greetOption);
    cli_register_option(parser, &versionOption);
    cli_register_option(parser, &helpOption);

    // Set the error handler for unrecognized options
    cli_set_error_handler(parser, (CliErrorHandler)unrecognized_option_handler);

    fmt_printf("Testing CLI argument parsing...\n");
    CliStatusCode status = cli_parse_args(parser, argc, argv);

    if (status == CLI_SUCCESS) {
        fmt_printf("Arguments parsed successfully.\n");
    } 
    else {
        fmt_fprintf(stderr, "Failed to parse arguments. Error code: %d\n", status);
    }

    cli_parser_deallocate(parser);
    fmt_printf("MyStrictApp execution completed.\n");

    return 0;
}
```
**Result**
```
./main --test

Creating CLI parser...
Error: Unrecognized option '--test'.
Use '--help' to see available options.
Options:
  --greet,      Greet a user by name
  --help, h     Display this help message
Failed to parse arguments. Error code: 1

./main --greet "best programmer"

Creating CLI parser...
Testing CLI argument parsing...
Hello, best programmer! Welcome to MyStrictApp
Arguments parsed successfully.
Creating CLI parser...
Testing CLI argument parsing...
MyStrictApp version 1.0.0
Arguments parsed successfully.

./main --help 

Creating CLI parser...
Testing CLI argument parsing...
Options:
  --greet,      Greet a user by name
  --version,    Show version information
  --help, h     Display this help message
Arguments parsed successfully.

./main -h  

Creating CLI parser...
Testing CLI argument parsing...
Options:
  --greet,      Greet a user by name
  --version,    Show version information
  --help, h     Display this help message
Arguments parsed successfully.

./main --version

Creating CLI parser...
Testing CLI argument parsing...
MyStrictApp version 1.0.0
Arguments parsed successfully.
```

---

## Example 4 : set Custom error handler with `cli_set_error_handler`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

void customErrorHandler(const CliParser *parser, const char *error, void *userData) {
    (void)parser;
    const char *appName = (const char *)userData;

    fmt_fprintf(stderr, "[%s Error] %s\n", appName ? appName : "Application", error);
    cli_print_help(parser);
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("ExampleApp");
    if (parser == NULL) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    const char *customUsage = "Usage: ExampleApp [options]\n"
                              "Options:\n"
                              "  --example  An example option\n"
                              "  --help     Show this help message\n";
    cli_set_custom_usage(parser, customUsage);

    // Set the custom error handler
    cli_set_error_handler(parser, customErrorHandler);

    // Add the --help option
    CliOption helpOption = {
        .longOpt = "--help",
        .shortOpt = 'h',
        .optionType = CLI_NO_ARG,
        .handler = NULL,
        .description = "Show this help message",
        .userData = parser,
    };

    cli_register_option(parser, &helpOption);

    fmt_printf("Testing CLI parser with invalid arguments...\n");
    const char *testArgs[] = {"ExampleApp", "--invalid"};
    CliStatusCode status = cli_parse_args(parser, argc, (char**)testArgs);

    if (status != CLI_SUCCESS) {
        fmt_fprintf(stderr, "Parser returned an error. Error code: %d\n", status);
    }

    cli_parser_deallocate(parser);
    fmt_printf("CLI parser test completed.\n");

    return 0;
}
```
**Result**
```
./main --help

[Application Error] --invalid
Options:
  --help, h     Show this help message
Parser returned an error. Error code: 1

./main --invalid

Testing CLI parser with invalid arguments...
[Application Error] --invalid
Options:
  --help, h     Show this help message
Parser returned an error. Error code: 1

./main h

Testing CLI parser with invalid arguments...
[Application Error] --invalid
Options:
  --help, h     Show this help message
Parser returned an error. Error code: 1
```

---

## Example 5: Registering and Using a Command with `cli_register_command`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <string.h>

// Greet Command Handler
void greetCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    (void)userData;
    if (argc > 0) {
        fmt_printf("Hello, %s!\n", argv[0]);
    } 
    else {
        fmt_printf("Hello, world!\n");
    }
}

// Farewell Command Handler
void farewellCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    (void)userData;
    if (argc > 0) {
        fmt_printf("Goodbye, %s!\n", argv[0]);
    } 
    else {
        fmt_printf("Goodbye, world!\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fmt_fprintf(stderr, "Usage: %s <command> [arguments]\n", argv[0]);
        return -1;
    }

    // Create the parser
    CliParser *parser = cli_parser_create("DynamicCommandApp");
    if (!parser) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
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
    }

    // Register the "farewell" command
    CliCommand farewellCommand = {
        .name = "farewell",
        .handler = farewellCommandHandler,
        .description = "Bids farewell to the user. Usage: farewell [name]",
        .userData = NULL
    };
    if (!cli_register_command(parser, &farewellCommand)) {
        fmt_fprintf(stderr, "Failed to register 'farewell' command\n");
    }

    // Parse real command-line arguments
    CliStatusCode status = cli_parse_args(parser, argc, argv);
    if (status != CLI_SUCCESS) {
        fmt_fprintf(stderr, "Error: Command not recognized or failed to execute.\n");
    }

    // Deallocate the parser
    cli_parser_deallocate(parser);
    return 0;
}

```
**Result**
```
./main greet Alice
Hello, Alice!

./main greet
Hello, world!

./main greet Amin 
Hello, Amin!

./main farewell Programmer
Goodbye, Programmer!

./main farewell unknown   
Goodbye, unknown!

./main unknown
Error: Command not recognized or failed to execute.
```

---

### Example 6: Demonstrating `cli_print_help` with Commands and Options `cli_register_option`

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
    CliParser *parser = (CliParser *)userData;
    cli_print_help(parser);
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
**Result**
```
./main

Options:
Commands:
  greet Greets the user. Usage: greet [name]

./main --help 

Options:
  --help, h     Display this help message
Commands:
  greet Greets the user. Usage: greet [name]

./main greet Amin

Hello, Amin!

./main greet Amin

Hello, Amin!

./main greet

Hello, world!

./main invalid   

Options:
  --help, h     Display this help message
Commands:
  greet Greets the user. Usage: greet [name]

```

---

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
**Result**
```
./main
Custom Error: Unrecognized command: 'none'

./main greet 
Hello, world!

./main greet Programmer
Hello, Programmer!

./main invalid
Custom Error: Unrecognized command: 'invalid'

```

---

## Example 8 : Using `cli_find_option` and `cli_print_version`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <string.h>

void versionOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option; 
    (void)value;  

    const CliParser *parser = (const CliParser *)userData;
    cli_print_version(parser, "1.0.0");
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
        .userData = parser 
    };

    if (!cli_register_option(parser, &versionOption)) {
        fmt_fprintf(stderr, "Failed to register '--version' option\n");
        cli_parser_deallocate(parser);
        return -1;
    }

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
**Result**
```
./main --version
VersionApp version 1.0.0

./main --unknown
Unknown option: --unknown

./main
No options provided. Use --version to print the version.
```

---

## Example 9 : Using `cli_find_command`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <string.h>

void helpCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command; 
    (void)argc;    
    (void)argv;    
    (void)userData; 

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
**Result**
```
./main help
Available commands:
  help    - Displays this help message

./main invalid
Unknown command: invalid

./main
No command provided. Use 'help' for more information.
```

---

## Example 10 : update cli description with `cli_update_description`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include "string/std_string.h"
#include <stdlib.h>

void greetCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command; 
    (void)userData; 

    if (argc > 0) {
        fmt_fprintf(stdout, "Greetings, %s! How are you today?\n", argv[0]);
    } 
    else {
        fmt_fprintf(stdout, "Hello, world! How are you today?\n");
    }
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("DemoApp");
    if (!parser) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }

    CliCommand greetCommand = {
        .name = "greet",
        .handler = greetCommandHandler,
        .description = string_strdup("Greets the user. Usage: greet [name]"),
        .userData = NULL
    };

    if (!cli_register_command(parser, &greetCommand)) {
        fmt_fprintf(stderr, "Failed to register 'greet' command\n");
        cli_parser_deallocate(parser);
        return -1;
    }

    const char *newDescription = "Say hello with a personalized message. Usage: greet [name]";
    cli_update_description(parser, "greet", newDescription, true);

    if (argc > 1) {
        CliStatusCode status = cli_parse_args(parser, argc, argv);
        if (status != CLI_SUCCESS) {
            fmt_fprintf(stderr, "Failed to parse arguments\n");
        }
    } 
    else {
        fmt_fprintf(stdout, "No command provided. Use 'help' for more information.\n");
    }

    // Display the updated command descriptions
    fmt_printf("\nUpdated Commands:\n");
    for (size_t i = 0; i < parser->numCommands; i++) {
        fmt_printf("  %s: %s\n", parser->commands[i].name, parser->commands[i].description);
    }

    free((void *)greetCommand.description); // free the string_strdup'd description owned by main
    cli_parser_deallocate(parser);
    return 0;
}
```
**Result**
```
./main 
No command provided. Use 'help' for more information.
Updated Commands:
  greet: Say hello with a personalized message. Usage: greet [name]


./main greet Amin
Greetings, Amin! How are you today?

Updated Commands:
  greet: Say hello with a personalized message. Usage: greet [name]

./main greet
Hello, world! How are you today?

Updated Commands:
  greet: Say hello with a personalized message. Usage: greet [name]

./main invalid
Failed to parse arguments

Updated Commands:
  greet: Say hello with a personalized message. Usage: greet [name]

```

---


## Example 11 : how to send parameters 

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <string.h>
#include <stdlib.h>

void addCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    const char *appName = (const char *)userData; 
    if (argc != 2) {
        fmt_printf("Usage: %s add <num1> <num2>\n", appName ? appName : "calc");
        return;
    }
    int num1 = atoi(argv[0]);
    int num2 = atoi(argv[1]);
    fmt_printf("Addition is %d + %d = %d\n", num1, num2, num1 + num2);
}

void subtractCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    const char *appName = (const char *)userData; 
    if (argc != 2) {
        fmt_printf("Usage: %s subtract <num1> <num2>\n", appName ? appName : "calc");
        return;
    }
    int num1 = atoi(argv[0]);
    int num2 = atoi(argv[1]);
    fmt_printf("Subtraction is %d - %d = %d\n", num1, num2, num1 - num2);
}

void helpOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;
    CliParser *parser = (CliParser *)userData;
    cli_print_help(parser);
}

void versionOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;
    (void)userData;
    fmt_printf("Version 1.0.0\n");
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("calc");

    cli_set_custom_usage(parser, "calc [command] [options]\n"
                                  "Commands:\n"
                                  "  add <num1> <num2>     Add two numbers\n"
                                  "  subtract <num1> <num2> Subtract two numbers\n"
                                  "Options:\n"
                                  "  --help, -h             Show this help message\n"
                                  "  --version, -v          Show version information");

    CliCommand addCommand = {
        .name = "add",
        .handler = addCommandHandler,
        .description = "Add two numbers",
        .userData = (void *)"calc"
    };

    CliCommand subtractCommand = {
        .name = "subtract",
        .handler = subtractCommandHandler,
        .description = "Subtract two numbers",
        .userData = (void *)"calc"
    };

    cli_register_command(parser, &addCommand);
    cli_register_command(parser, &subtractCommand);

    CliOption helpOption = {
        .longOpt = "--help",
        .shortOpt = 'h',
        .handler = helpOptionHandler,
        .description = "Show help message",
        .userData = parser
    };
    CliOption versionOption = {
        .longOpt = "--version",
        .shortOpt = 'v',
        .handler = versionOptionHandler,
        .description = "Show version information"
    };

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
**Result**
```
/main add 10 20
Addition is 10 + 20 = 30

./main subtract 30 50
Subtraction is 30 - 50 = -20

./main --version
Version 1.0.0

./main --help
Options:
  --help, h     Show help message
  --version, v  Show version information
Commands:
  add   Add two numbers
  subtract      Subtract two numbers

./main invalid
Unknown command or option. Use '--help' for usage information.

./main
No command provided. Use '--help' for usage information.
```

---


## Example 12 : interactive mode 

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <stdlib.h>

void calculateCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    (void)userData;

    if (argc != 2 && argc != 3) { 
        fmt_printf("Usage: calculate <number1> <number2>\n");
        return;
    }

    int num1, num2;
    if (argc == 2) {
        // Single mode
        num1 = atoi(argv[0]);
        num2 = atoi(argv[1]);
    } 
    else {
        // Interactive mode
        num1 = atoi(argv[1]);
        num2 = atoi(argv[2]);
    }

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

void helpOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;
    CliParser *parser = (CliParser *)userData;

    cli_print_help(parser);
}

void versionOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;
    fmt_printf("Version 1.0.0\n");
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


    CliOption helpOption = {
        .longOpt = "--help",
        .shortOpt = 'h',
        .optionType = CLI_NO_ARG,
        .handler = helpOptionHandler,
        .description = "Show help message",
        .userData = parser
    };

    CliOption versionOption = {
        .longOpt = "--version",
        .shortOpt = 'v',
        .optionType = CLI_NO_ARG,
        .handler = versionOptionHandler,
        .description = "Show version information"
    };

    cli_register_option(parser, &helpOption);
    cli_register_option(parser, &versionOption);
    cli_register_option(parser, &interactiveOption);

    cli_parse_args(parser, argc, argv);

    cli_parser_deallocate(parser);
    return 0;
}
```
**Result**
```
./main calculate 10 20  
Sum: 30

./main --help        
Options:
  --help, h     Show help message
  --version, v  Show version information
  --interactive,        Enters interactive mode
Commands:
  calculate     Performs a simple addition

./main --version     
Version 1.0.0

./main --interactive  
Entering interactive mode. Type 'exit' to quit.
> invalid
Error: Command 'invalid' failed with status 8
> test 1 0
Error: Command 'test' failed with status 8
> calculate 20 20
Sum: 40
> exit
Exiting interactive mode.

```

---

## Example 13: Interactive Mode and Command Aliases

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <stdlib.h>

void exitCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    (void)argc;
    (void)argv;
    (void)userData;
    fmt_printf("Exiting via command: %s\n", command->name);
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
    cli_register_command_alias(parser, "exit", "quit2");
    cli_register_command_alias(parser, "exit", "quit");

    enterInteractiveMode(parser);

    cli_parser_deallocate(parser);
    return 0;
}
```
**Result**
```
./main
Interactive mode. Type 'exit' or 'quit' to exit.
>exit
Exiting interactive mode.

./main
Interactive mode. Type 'exit' or 'quit' to exit.
>invalid
Error: Command 'invalid' failed with status 8
>quit
Exiting via command: quit

./main
Interactive mode. Type 'exit' or 'quit' to exit.
>quit2
Exiting via command: quit2
```

---

## Example 14 : Pre/Post Execution Hooks with `cli_set_pre_execution_hook` and `cli_set_post_execution_hook`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

void preHook(const CliParser *parser, void *userData) {
    (void)parser;
    (void)userData;
    fmt_printf("[pre-hook] About to execute command.\n");
}

void postHook(const CliParser *parser, void *userData) {
    (void)parser;
    (void)userData;
    fmt_printf("[post-hook] Command execution finished.\n");
}

void greetHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    (void)userData;
    fmt_printf("Hello, %s!\n", argc > 0 ? argv[0] : "world");
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("HookApp");
    cli_set_pre_execution_hook(parser, preHook);
    cli_set_post_execution_hook(parser, postHook);

    CliCommand greetCommand = {
        .name = "greet",
        .handler = greetHandler,
        .description = "Greet a user",
        .userData = NULL
    };
    cli_register_command(parser, &greetCommand);

    cli_parse_args(parser, argc, argv);
    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**
```
./main greet Alice
[pre-hook] About to execute command.
Hello, Alice!
[post-hook] Command execution finished.

./main greet
[pre-hook] About to execute command.
Hello, world!
[post-hook] Command execution finished.
```

---

## Example 15 : Option Alias and Argument Validation with `cli_add_option_alias`, `cli_validate_option_argument`, and `cli_get_last_error`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

bool validateNumber(const char *value, void *userData) {
    (void)userData;
    if (!value || !*value) return false;
    for (int i = 0; value[i]; i++) {
        if (value[i] < '0' || value[i] > '9') return false;
    }
    return true;
}

void countHandler(const CliOption *option, const char *value, void *userData) {
    (void)userData;
    if (!cli_validate_option_argument(option, value)) {
        fmt_fprintf(stderr, "Invalid value for --count: '%s' (must be a non-negative integer)\n", value);
        return;
    }
    fmt_printf("Count set to: %s\n", value);
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("AliasApp");

    CliOption countOption = {
        .longOpt = "--count",
        .shortOpt = 'c',
        .optionType = CLI_REQUIRED_ARG,
        .handler = countHandler,
        .validator = validateNumber,
        .description = "Set a numeric count"
    };
    cli_register_option(parser, &countOption);
    cli_add_option_alias(parser, "--count", "--num");  /* --num is now a synonym */

    CliStatusCode status = cli_parse_args(parser, argc, argv);
    if (status != CLI_SUCCESS) {
        CliError err = cli_get_last_error(parser);
        fmt_fprintf(stderr, "Parse error (code %d): %s\n", err.code, err.message);
    }

    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**
```
./main --count 5
Count set to: 5

./main --num 10
Count set to: 10

./main -c 3
Count set to: 3

./main --count abc
Invalid value for --count: 'abc' (must be a non-negative integer)
```

---

## Example 16 : running server with different mode 

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

int globalPort = 0;
bool verboseMode = false;

bool validatePort(const char *value, void *userData) {
    (void)userData;
    long port = strtol(value, NULL, 10);
    if (port <= 0 || port > 65535) {
        fmt_printf("Port must be between 1 and 65535\n");
        return false;
    }
    return true;
}

void portOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)userData;
    if (!cli_validate_option_argument(option, value)) {
        fmt_fprintf(stderr, "Invalid port: %s (must be 1-65535)\n", value);
        return;
    }
    long port = strtol(value, NULL, 10);
    globalPort = (int)port;
    fmt_printf("Port set to %d\n", globalPort);
}

void verboseOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)userData;
    (void)option;
    (void)value;
    verboseMode = true;
    fmt_printf("Verbose mode enabled\n");
}

void startServerCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    (void)userData;
    (void)argc;
    (void)argv;

    if (globalPort <= 0) {
        fmt_printf("Error: Port not specified or invalid. Use --port to set a valid port.\n");
        return;
    }
    fmt_printf("Server started on port %d...\n", globalPort);

    if (verboseMode) {
        fmt_printf("[VERBOSE] Verbose mode is ON. Logging detailed server operations.\n");
    }
}

void helpOptionHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)value;

    CliParser *parser = (CliParser *)userData;
    if (parser == NULL) {
        fmt_printf("Error: Parser is NULL.\n");
        return;
    }

    cli_print_help(parser);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("ServerApp");
    CliCommand startServerCommand = {
        .name = "start",
        .handler = startServerCommandHandler,
        .description = "Starts the server",
    };

    CliOption serverOptions[] = {
        {.longOpt = "--port", .shortOpt = 'p', .optionType = CLI_REQUIRED_ARG, .handler = portOptionHandler, .validator = validatePort, .description = "Set server port"},
        {.longOpt = "--verbose", .shortOpt = 'v', .optionType = CLI_NO_ARG, .handler = verboseOptionHandler, .description = "Enable verbose output"},
    };

    CliOption helpOption = {
        .longOpt = "--help",
        .shortOpt = 'h',
        .optionType = CLI_NO_ARG,
        .handler = helpOptionHandler,
        .description = "Show help message",
        .userData = parser
    };

    cli_register_command(parser, &startServerCommand);
    cli_add_option_group(parser, "Server Options", serverOptions, sizeof(serverOptions) / sizeof(CliOption));
    cli_register_option(parser, &helpOption);

    if (cli_parse_args(parser, argc, argv) != CLI_SUCCESS) {
        fmt_printf("Error: Failed to parse arguments\n");
        cli_print_help(parser);
        cli_parser_deallocate(parser);
        exit(EXIT_FAILURE);
    }

    cli_parser_deallocate(parser);
    return 0;
}
```
**Result**
```
Note: global options (--port, --verbose) must appear BEFORE the command name.

./main --port 8080 start
Port set to 8080
Server started on port 8080...

./main --port 8080 --verbose start
Port set to 8080
Verbose mode enabled
Server started on port 8080...
[VERBOSE] Verbose mode is ON. Logging detailed server operations.

./main start
Error: Port not specified or invalid. Use --port to set a valid port.

./main --port bad_value start
Invalid port: bad_value (must be 1-65535)
Error: Port not specified or invalid. Use --port to set a valid port.

./main --help
Options:
  --port, p	Set server port
  --verbose, v	Enable verbose output
  --help, h	Show help message
Commands:
  start	Starts the server
```

---

## Example 17 : Grouping related options with `cli_add_option_group` and `cli_process_option_group`

Option groups let you register and process a set of related options together. `cli_process_option_group` scans the arguments and invokes each matched option's handler.

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

void verboseHandler(const CliOption *option, const char *value, void *userData) {
    (void)option; (void)value; (void)userData;
    fmt_printf("  [logging] verbose enabled\n");
}

void levelHandler(const CliOption *option, const char *value, void *userData) {
    (void)option; (void)userData;
    fmt_printf("  [logging] level = %s\n", value ? value : "(none)");
}

int main(void) {
    CliParser *parser = cli_parser_create("app");

    CliOption logging[] = {
        { .longOpt = "--verbose", .shortOpt = 'V', .optionType = CLI_NO_ARG,
          .handler = verboseHandler, .description = "Enable verbose logging" },
        { .longOpt = "--level", .shortOpt = 'l', .optionType = CLI_REQUIRED_ARG,
          .handler = levelHandler, .description = "Set the log level" },
    };
    cli_add_option_group(parser, "Logging", logging, 2);

    /* Simulated command line: app --verbose --level debug */
    char *argv[] = { "app", "--verbose", "--level", "debug" };
    int argc = 4;

    fmt_printf("Processing 'Logging' group:\n");
    bool ok = cli_process_option_group(parser, "Logging", argc, argv);
    fmt_printf("group processed: %s\n", ok ? "yes" : "no");

    cli_remove_option_group(parser, "Logging");
    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**

```
Processing 'Logging' group:
  [logging] verbose enabled
  [logging] level = debug
group processed: yes
```

---

## Example 18 : `--key=value` syntax with `cli_parse_args_with_delimiter`

`cli_parse_args_with_delimiter` splits each argument on a delimiter before parsing, so a single `--name=Alice` token is expanded to `--name` `Alice`.

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

void nameHandler(const CliOption *option, const char *value, void *userData) {
    (void)option; (void)userData;
    fmt_printf("Hello, %s!\n", value ? value : "(anonymous)");
}

int main(void) {
    CliParser *parser = cli_parser_create("greet");

    CliOption nameOpt = {
        .longOpt = "--name", .shortOpt = 'n', .optionType = CLI_REQUIRED_ARG,
        .handler = nameHandler, .description = "Who to greet"
    };
    cli_register_option(parser, &nameOpt);

    /* The user typed:  greet --name=Alice
       The "=" delimiter splits "--name=Alice" into "--name" "Alice". */
    char *argv[] = { "greet", "--name=Alice" };
    int argc = 2;

    bool ok = cli_parse_args_with_delimiter(parser, argc, argv, "=");
    fmt_printf("parse ok: %s\n", ok ? "yes" : "no");

    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**

```
Hello, Alice!
parse ok: yes
```

---

## Example 19 : Validating an option argument with a custom validator

Attach a `CliArgumentValidator` and a `validationErrorMessage` to an option, then check candidate values with `cli_validate_option_argument`; on failure the message is available via `cli_get_last_error`.

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <ctype.h>

/* CliArgumentValidator: bool (const char* value, void* userData) */
bool isPositiveInt(const char *value, void *userData) {
    (void)userData;
    if (!value || !*value) {
        return false;
    }
    for (const char *p = value; *p; ++p) {
        if (!isdigit((unsigned char)*p)) {
            return false;
        }
    }
    return atoi(value) > 0;
}

int main(void) {
    CliParser *parser = cli_parser_create("server");

    CliOption portOpt = {
        .longOpt = "--port", .shortOpt = 'p', .optionType = CLI_REQUIRED_ARG,
        .description = "TCP port (positive integer)",
        .validator = isPositiveInt,
        .validationErrorMessage = "port must be a positive integer"
    };
    cli_register_option(parser, &portOpt);

    const char *inputs[] = { "8080", "0", "abc" };
    for (int i = 0; i < 3; ++i) {
        if (cli_validate_option_argument(&portOpt, inputs[i])) {
            fmt_printf("'%s' -> valid\n", inputs[i]);
        } 
        else {
            CliError err = cli_get_last_error(parser);
            fmt_printf("'%s' -> rejected: %s\n", inputs[i], err.message);
        }
    }

    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**

```
'8080' -> valid
'0' -> rejected: port must be a positive integer
'abc' -> rejected: port must be a positive integer
```

---

## Example 20 : Removing registrations with `cli_unregister_option` and `cli_unregister_command`

Options and commands can be removed at runtime; afterwards `cli_find_option` / `cli_find_command` no longer find them.

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

void optHandler(const CliOption *o, const char *v, void *u) { (void)o; (void)v; (void)u; }
void cmdHandler(const CliCommand *c, int argc, char *argv[], void *u) {
    (void)c; (void)argc; (void)argv; (void)u;
}

int main(void) {
    CliParser *parser = cli_parser_create("tool");

    CliOption debugOpt = {
        .longOpt = "--debug", .shortOpt = 'd', .optionType = CLI_NO_ARG,
        .handler = optHandler, .description = "Enable debug output"
    };
    cli_register_option(parser, &debugOpt);

    CliCommand buildCmd = {
        .name = "build", .handler = cmdHandler, .description = "Build the project"
    };
    cli_register_command(parser, &buildCmd);

    fmt_printf("before: option=%s, command=%s\n",
               cli_find_option(parser, "--debug", 'd') ? "found" : "missing",
               cli_find_command(parser, "build")       ? "found" : "missing");

    cli_unregister_option(parser, "--debug", 'd');
    cli_unregister_command(parser, "build");

    fmt_printf("after:  option=%s, command=%s\n",
               cli_find_option(parser, "--debug", 'd') ? "found" : "missing",
               cli_find_command(parser, "build")       ? "found" : "missing");

    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**

```
before: option=found, command=found
after:  option=missing, command=missing
```

---

## Example 21 : Declaring option dependencies with `cli_set_option_dependencies`

Declare that one option is only meaningful alongside another. The call validates that both options are registered, so a dependency on an unknown option is reported via `cli_get_last_error`.

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

void noop(const CliOption *o, const char *v, void *u) { (void)o; (void)v; (void)u; }

int main(void) {
    CliParser *parser = cli_parser_create("convert");

    CliOption outOpt = {
        .longOpt = "--output", .shortOpt = 'o', .optionType = CLI_REQUIRED_ARG,
        .handler = noop, .description = "Output file"
    };
    CliOption fmtOpt = {
        .longOpt = "--format", .shortOpt = 'f', .optionType = CLI_REQUIRED_ARG,
        .handler = noop, .description = "Output format"
    };
    cli_register_option(parser, &outOpt);
    cli_register_option(parser, &fmtOpt);

    /* --output is only meaningful together with --format. */
    bool ok = cli_set_option_dependencies(parser, "--output", 'o', "--format", 'f');
    fmt_printf("declare (--output needs --format): %s\n", ok ? "ok" : "failed");

    /* Declaring a dependency on an option that doesn't exist fails. */
    bool bad = cli_set_option_dependencies(parser, "--output", 'o', "--missing", 0);
    if (!bad) {
        CliError err = cli_get_last_error(parser);
        fmt_printf("declare (--output needs --missing): failed -> %s\n", err.message);
    }

    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**

```
declare (--output needs --format): ok
declare (--output needs --missing): failed -> Error: Dependency option '--missing' not found.
```

---

## Example 22 : Collecting repeated options into a `Vector` (integrates `vector`)

A `CLI_REQUIRED_ARG` option may appear many times. Here the `--tag` handler pushes each value into a `Vector` passed through `userData`, so the parser becomes a collector of repeated flags.

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include "vector/vector.h"
#include "string/std_string.h"
#include <stdlib.h>

/* The --tag option may be repeated; each value is collected into a Vector
   passed through userData. We store a strdup'd copy (char*) per tag. */
void tagHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    Vector *tags = (Vector *)userData;
    if (value && tags) {
        char *copy = string_strdup(value);
        vector_push_back(tags, &copy);
    }
}

int main(void) {
    CliParser *parser = cli_parser_create("tagger");
    Vector *tags = vector_create(sizeof(char *));

    CliOption tagOpt = {
        .longOpt = "--tag", .shortOpt = 't', .optionType = CLI_REQUIRED_ARG,
        .handler = tagHandler, .description = "Add a tag (repeatable)",
        .userData = tags
    };
    cli_register_option(parser, &tagOpt);

    /* Simulated command line: tagger --tag red --tag green --tag blue */
    char *argv[] = { "tagger", "--tag", "red", "--tag", "green", "--tag", "blue" };
    int argc = 7;

    cli_parse_args(parser, argc, argv);

    fmt_printf("Collected %zu tag(s):\n", vector_size(tags));
    for (size_t i = 0; i < vector_size(tags); ++i) {
        char *t = *(char **)vector_at(tags, i);
        fmt_printf("  [%zu] %s\n", i, t);
        free(t);
    }

    vector_deallocate(tags);
    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**

```
Collected 3 tag(s):
  [0] red
  [1] green
  [2] blue
```

---

## Example 23 : Transforming an option value with the `String` library (integrates `string`)

An option value is just a `const char*`. Wrapping it in a `String` lets you reuse the rich text helpers — here `string_to_upper` and `string_to_lower` echo the `--name` value in both cases.

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include "string/std_string.h"
#include <stdlib.h>

/* --name <value> : echo the value upper-cased and lower-cased using the
   String library's case helpers. */
void nameHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)userData;
    if (!value) {
        return;
    }
    String *s = string_create(value);
    char *upper = string_to_upper(s);
    char *lower = string_to_lower(s);

    fmt_printf("original: %s\n", value);
    fmt_printf("upper:    %s\n", upper);
    fmt_printf("lower:    %s\n", lower);

    free(upper);
    free(lower);
    string_deallocate(s);
}

int main(void) {
    CliParser *parser = cli_parser_create("namecase");

    CliOption nameOpt = {
        .longOpt = "--name", .shortOpt = 'n', .optionType = CLI_REQUIRED_ARG,
        .handler = nameHandler, .description = "Print the name in upper and lower case"
    };
    cli_register_option(parser, &nameOpt);

    /* Simulated command line: namecase --name MixedCase */
    char *argv[] = { "namecase", "--name", "MixedCase" };
    int argc = 3;

    cli_parse_args(parser, argc, argv);

    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**

```
original: MixedCase
upper:    MIXEDCASE
lower:    mixedcase
```

---

## Example 24 : A command that emits JSON output (integrates `json`)

A command handler can produce structured output. Here the `info` command builds a `JSON_OBJECT` with the `json` library and prints its compact serialization. Note that `json_serialize` emits object keys in sorted order.

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include "json/json.h"
#include "string/std_string.h"
#include <stdlib.h>

/* The "info" command builds a JSON object describing the app and prints the
   compact serialization produced by the JSON library. */
void infoCommandHandler(const CliCommand *command, int argc, char *argv[], void *userData) {
    (void)command;
    (void)argc;
    (void)argv;
    (void)userData;

    JsonElement *root = json_create(JSON_OBJECT);

    JsonElement *name = json_create(JSON_STRING);
    name->value.string_val = string_strdup("namecase");
    json_add_to_object(root, "app", name);

    JsonElement *version = json_create(JSON_STRING);
    version->value.string_val = string_strdup("1.0.0");
    json_add_to_object(root, "version", version);

    JsonElement *ok = json_create(JSON_BOOL);
    ok->value.bool_val = true;
    json_add_to_object(root, "ready", ok);

    char *text = json_serialize(root);
    fmt_printf("%s\n", text);

    free(text);
    json_deallocate(root);
}

int main(void) {
    CliParser *parser = cli_parser_create("infoapp");

    CliCommand infoCmd = {
        .name = "info", .handler = infoCommandHandler,
        .description = "Emit application info as JSON"
    };
    cli_register_command(parser, &infoCmd);

    /* Simulated command line: infoapp info */
    char *argv[] = { "infoapp", "info" };
    int argc = 2;

    cli_parse_args(parser, argc, argv);

    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**

```
{"app": "namecase", "ready": true, "version": "1.0.0"}
```

---

## Example 25 : Splitting a delimited option value (integrates `string`)

Reuse `string_split` to turn one comma-separated `--fields` argument into individual values. Each returned `String*` (and the array itself) is freed once consumed.

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include "string/std_string.h"
#include <stdlib.h>

/* --fields a,b,c : split the comma-separated value into individual fields
   using the String library's split helper. */
void fieldsHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    (void)userData;
    if (!value) {
        return;
    }
    String *s = string_create(value);
    int count = 0;
    String **parts = string_split(s, ",", &count);

    fmt_printf("split into %d field(s):\n", count);
    for (int i = 0; i < count; ++i) {
        fmt_printf("  %d: %s\n", i + 1, string_c_str(parts[i]));
        string_deallocate(parts[i]);
    }
    free(parts);
    string_deallocate(s);
}

int main(void) {
    CliParser *parser = cli_parser_create("splitter");

    CliOption fieldsOpt = {
        .longOpt = "--fields", .shortOpt = 'f', .optionType = CLI_REQUIRED_ARG,
        .handler = fieldsHandler, .description = "Comma-separated fields to split"
    };
    cli_register_option(parser, &fieldsOpt);

    /* Simulated command line: splitter --fields name,age,email */
    char *argv[] = { "splitter", "--fields", "name,age,email" };
    int argc = 3;

    cli_parse_args(parser, argc, argv);

    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**

```
split into 3 field(s):
  1: name
  2: age
  3: email
```

---

## Example 26 : Accumulating numeric options and reducing them (integrates `vector`)

Each `--add <n>` pushes an integer into a `Vector<int>`. After parsing, the program walks the vector to compute a running total — a small map/reduce over repeated flags.

```c
#include "cli/cli.h"
#include "fmt/fmt.h"
#include "vector/vector.h"
#include <stdlib.h>

/* Each --add <n> pushes an integer into a Vector<int>; after parsing we sum
   the collected numbers. */
void addHandler(const CliOption *option, const char *value, void *userData) {
    (void)option;
    Vector *nums = (Vector *)userData;
    if (value && nums) {
        int n = atoi(value);
        vector_push_back(nums, &n);
    }
}

int main(void) {
    CliParser *parser = cli_parser_create("sum");
    Vector *nums = vector_create(sizeof(int));

    CliOption addOpt = {
        .longOpt = "--add", .shortOpt = 'a', .optionType = CLI_REQUIRED_ARG,
        .handler = addHandler, .description = "Add a number to the running set (repeatable)",
        .userData = nums
    };
    cli_register_option(parser, &addOpt);

    /* Simulated command line: sum --add 4 --add 8 --add 15 --add 16 */
    char *argv[] = { "sum", "--add", "4", "--add", "8", "--add", "15", "--add", "16" };
    int argc = 9;

    cli_parse_args(parser, argc, argv);

    long total = 0;
    for (size_t i = 0; i < vector_size(nums); ++i) {
        total += *(int *)vector_at(nums, i);
    }
    fmt_printf("count = %zu\n", vector_size(nums));
    fmt_printf("sum   = %ld\n", total);

    vector_deallocate(nums);
    cli_parser_deallocate(parser);
    return 0;
}
```

**Result**

```
count = 4
sum   = 43
```

---

## License

This project is open-source and available under [ISC License].