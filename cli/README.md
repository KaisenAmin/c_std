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
    else {
        fmt_println("Parser Create Successfully");
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
    if (!userData) {
        fmt_fprintf(stderr, "Error: userData is NULL in helpOption handler.\n");
        return;
    }
    CliParser *parser = (CliParser *)userData;
    cli_print_help(parser);
}

void version_option_handler(const CliOption *option, const char *value, void *userData) {
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
Registering options...
Options registered successfully.
Testing argument parsing...
Options:
  --help, h     Display this help message
  --version, v  Display version information
Arguments parsed successfully.
Deallocating CLI parser...

 ./main --version

Creating CLI parser...
CLI parser created successfully.
Setting custom usage message...
Custom usage message set successfully.
Registering options...
Options registered successfully.
Testing argument parsing...
Version: MyApp v1.0.0
Arguments parsed successfully.
Deallocating CLI parser...
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
    (void)userData;
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
    cli_register_command_alias(parser, "exit", "fuck");
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
>invalid
Error: Command 'invalid' failed with status 8
>quit
Exiting via command: quit

./main
Interactive mode. Type 'exit' or 'quit' to exit.
>fuck
Exiting via command: fuck

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
    (void)option;
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
./main start --port 8080
Port set to 8080
Server started on port 8080...


./main start --port 8080 --verbose
Port set to 8080
Verbose mode enabled
Server started on port 8080...
Verbose mode is ON. Logging detailed server operations.
Verbose mode flag is set to true.

./main start
Error: Port not specified or invalid. Use --port to set a valid port.

./main --help
Error: Failed to parse arguments
Options:
  --port, p     Set server port
  --verbose, v  Enable verbose output
  --help, h     Show help message
Commands:
  start Starts the server
```

---

## License

This project is open-source and available under [ISC License].