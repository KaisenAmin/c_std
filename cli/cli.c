/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Cli
*/

#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "../fmt/fmt.h"
#include "../string/std_string.h"


extern char **environ;
static CliError cli_last_error = {0, ""};

static char** split_string(const char* str, const char* delimiter, int* count) {
    char* token;
    char** tokenArray = NULL;
    char* strCopy = string_strdup(str); // Create a modifiable copy of str
    int tokens = 0;

    // Get the first token
    token = strtok(strCopy, delimiter);
    while (token != NULL) {
        tokenArray = realloc(tokenArray, sizeof(char*) * (tokens + 1));
        tokenArray[tokens++] = string_strdup(token);
        token = strtok(NULL, delimiter);
    }
    *count = tokens;

    free(strCopy); // Clean up the strdup
    return tokenArray;
}
/** 
 * @brief Creates a new CLI parser instance.
 *
 * This function allocates and initializes a new `CliParser` structure, which can be used
 * to manage command-line options, commands, and their associated handlers.
 *
 * @param progName A string representing the name of the program. This name will be used in
 *                 usage messages. Must not be NULL.
 *
 * @return A pointer to the newly created `CliParser` instance, or NULL if an error occurred.
 *
 * @note NULL if `progName` is NULL or if memory allocation fails.
 * @note CliParser* a valid pointer to the `CliParser` instance if successful.
 * @note The caller is responsible for deallocating the returned `CliParser` instance
 *          using `cli_parser_deallocate` to prevent memory leaks.
 */
CliParser* cli_parser_create(const char *progName) {
    if (progName == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Program name is NULL in cli_parser_create.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif 
        return NULL;
    }

    CliParser *parser = (CliParser *)malloc(sizeof(CliParser));
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed for CliParser in cli_parser_create.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif 
        return NULL;
    }

    parser->options = NULL;
    parser->commands = NULL;
    parser->numOptions = 0;
    parser->numCommands = 0;
    parser->progName = string_strdup(progName);
    if (parser->progName == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed for program name in cli_parser_create.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        free(parser); // Free the allocated parser before returning NULL
        return NULL;
    }
    parser->usage = NULL;
    parser->errorHandler = NULL;
    memset(&parser->lastError, 0, sizeof(parser->lastError)); // Initializing lastError
    parser->strictMode = false;
    parser->defaultCommandHandler = NULL;
    parser->optionGroups = NULL; // Initialize to NULL
    parser->numOptionGroups = 0; // Initialize to 0
    parser->preExecutionHook = NULL;
    parser->postExecutionHook = NULL;
    parser->preExecutionHookUserData = NULL;
    parser->pipeliningEnabled = false;
    parser->userData = NULL;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Parser Created Successfully.\n");
    cli_last_error.code = CLI_SUCCESS;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, cli_last_error.message);
    #endif

    return parser;
}

/**
 * @brief Deallocates a CLI parser and all associated resources.
 *
 * This function frees all memory associated with the given `CliParser` instance, including
 * the options, commands, and any other dynamically allocated data. After calling this
 * function, the `parser` pointer is no longer valid and should not be used.
 *
 * @param parser A pointer to the `CliParser` instance to be deallocated. Must not be NULL.
 *
 * @return void
 * @note If `parser` is NULL, the function logs an error and returns without performing
 *          any deallocation.
 */
void cli_parser_deallocate(CliParser *parser) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Attempted to deallocate a NULL CliParser.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;

        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    // Free the program name
    if (parser->progName != NULL) {
        free(parser->progName);
        parser->progName = NULL;
    }

    // Deallocate options if they exist
    if (parser->options != NULL) {
        for (size_t i = 0; i < parser->numOptions; i++) {
            if (parser->options[i].longOpt != NULL) {
                free((void*)parser->options[i].longOpt); 
            }
            if (parser->options[i].description != NULL) {
                free((void*)parser->options[i].description);
            }
        }
        free(parser->options);
        parser->options = NULL;
    }

    // Deallocate commands if they exist
    if (parser->commands != NULL) {
        for (size_t i = 0; i < parser->numCommands; i++) {
            if (parser->commands[i].name != NULL) {
                free((void*)parser->commands[i].name);
            }
            if (parser->commands[i].description != NULL) {
                free((void*)parser->commands[i].description);
            }
        }
        free(parser->commands);
        parser->commands = NULL;
    }

    free(parser);

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: CliParser deallocated successfully.\n");
    cli_last_error.code = CLI_SUCCESS;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, cli_last_error.message);
    #endif
}

/**
 * @brief Sets a custom usage message for the CLI parser.
 *
 * This function allows the user to set a custom usage message that will be displayed
 * when help information is printed. The custom usage message overrides the default
 * generated message.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param usage A string containing the custom usage message. Must not be NULL.
 *
 * @return void
 * @note If the `parser` or `usage` is NULL, an error is logged and the function returns without setting the usage message.
 */
void cli_set_custom_usage(CliParser *parser, const char *usage) {
    if (parser == NULL || usage == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: NULL argument provided to cli_set_custom_usage.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    // Free existing usage message if present
    if (parser->usage != NULL) {
        free(parser->usage);
        parser->usage = NULL;
    }

    // Duplicate the usage message and assign it to the parser
    parser->usage = string_strdup(usage);
    if (parser->usage == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed for custom usage message in cli_set_custom_usage.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
    } 
    else {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Custom usage message set successfully.\n");
        cli_last_error.code = CLI_SUCCESS;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stdout, cli_last_error.message);
        #endif
    }
}

/**
 * @brief Enables or disables strict mode for the CLI parser.
 *
 * Strict mode, when enabled, causes the parser to treat unrecognized options as errors.
 * This can be useful for ensuring that all provided options are valid and recognized by the parser.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param enable A boolean flag indicating whether to enable (`true`) or disable (`false`) strict mode.
 *
 * @return void
 * @note If the `parser` is NULL, an error is logged and the function returns without modifying the strict mode setting.
 */
void cli_enable_strict_mode(CliParser *parser, bool enable) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: NULL parser provided to cli_enable_strict_mode.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    // Set the strictMode field based on the enable parameter
    parser->strictMode = enable;

    if (enable) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Strict mode enabled for CLI parser.\n");
        cli_last_error.code = CLI_SUCCESS;
    } 
    else {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Strict mode disabled for CLI parser.\n");
        cli_last_error.code = CLI_DISABLE_STRICT_MODE;
    }

    #ifdef CLI_LOGGING_ENABLE 
        if (parser->strictMode) {
            fmt_fprintf(stdout, cli_last_error.message);
        }
        else {
            fmt_fprintf(stderr, cli_last_error.message);
        }
    #endif
}
/** 
 * @brief Sets a custom error handler for the CLI parser.
 *
 * This function allows the user to specify a custom error handler function that will be called whenever
 * an error occurs during parsing. The custom handler can be used to provide custom error messages or
 * perform specific error handling actions.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param handler A pointer to the error handler function. Must not be NULL.
 *
 * @return void
 * @note If the `parser` or `handler` is NULL, an error is logged and the function returns without setting the error handler.
 */
void cli_set_error_handler(CliParser *parser, CliErrorHandler handler) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Cannot set error handler on a NULL CliParser.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    if (handler == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Cannot set error handler on a NULL errorHandler function.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    // Assign the provided handler to the parser
    parser->errorHandler = handler;
    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Custome Error handler set successfully.\n");
    cli_last_error.code = CLI_SUCCESS;

    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, cli_last_error.message);
    #endif
}

/**
 * @brief Registers a new command with the CLI parser.
 *
 * This function adds a new command to the CLI parser, allowing it to be recognized and executed during
 * command-line parsing. The command must have a unique name.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param command A pointer to the `CliCommand` structure defining the command. Must not be NULL.
 *
 * @return `true` if the command was successfully registered, `false` otherwise.
 *
 * @note false if the `parser` or `command` is NULL, or if a duplicate command name is detected.
 * @note true if the command was successfully registered.
 */
bool cli_register_command(CliParser *parser, const CliCommand *command) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Parser is NULL in cli_register_command.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return false;
    }
    if (command == NULL || command->name == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Command or command name is NULL in cli_register_command.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return false;
    }

    // Check for duplicate command names
    for (size_t i = 0; i < parser->numCommands; i++) {
        if (strcmp(parser->commands[i].name, command->name) == 0) {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Command '%s' already exists in cli_register_command.\n", command->name);
            cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND; 
            #ifdef CLI_LOGGING_ENABLE
                fmt_fprintf(stderr, cli_last_error.message);
            #endif
            return false;
        }
    }

    // Allocate or resize the commands array
    void *temp = realloc(parser->commands, (parser->numCommands + 1) * sizeof(CliCommand));
    if (temp == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed in cli_register_command.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return false;
    }
    parser->commands = temp;

    // Add the new command
    parser->commands[parser->numCommands] = *command;
    parser->numCommands++;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Command '%s' registered successfully.\n", command->name);
    cli_last_error.code = CLI_SUCCESS;

    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, cli_last_error.message);
    #endif
    return true;
}

/**
 * @brief Prints the help message for the CLI parser.
 *
 * This function outputs the usage message, followed by the list of available options and commands.
 * If a custom usage message has been set, it will be displayed; otherwise, a default usage message
 * will be generated based on the program name.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 *
 * @return void
 * @note If the parser is NULL, an error is logged, and the function returns without printing anything.
 */
void cli_print_help(const CliParser *parser) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: NULL parser provided to cli_print_usage.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    if (parser->usage) {
        fmt_fprintf(stdout, "%s\n", parser->usage);
    } 
    else {
        fmt_fprintf(stdout, "Usage: %s [options] [commands]\n", parser->progName ? parser->progName : "application");
    }

    // Print options
    if (parser->options && parser->numOptions > 0) {
        fmt_fprintf(stdout, "Options:\n");
        for (size_t i = 0; i < parser->numOptions; i++) {
            fmt_fprintf(stdout, "  %s, %c\t%s\n",
                        parser->options[i].longOpt ? parser->options[i].longOpt : "",
                        parser->options[i].shortOpt ? parser->options[i].shortOpt : ' ',
                        parser->options[i].description ? parser->options[i].description : "");
        }
    }

    // Print commands
    if (parser->commands && parser->numCommands > 0) {
        fmt_fprintf(stdout, "Commands:\n");
        for (size_t i = 0; i < parser->numCommands; i++) {
            fmt_fprintf(stdout, "  %s\t%s\n",
                        parser->commands[i].name ? parser->commands[i].name : "Unnamed",
                        parser->commands[i].description ? parser->commands[i].description : "No description available");
        }
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Usage information printed successfully.\n");
    cli_last_error.code = CLI_SUCCESS;

    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, cli_last_error.message);
    #endif
}

/**
 * @brief Registers a new option with the CLI parser.
 *
 * This function adds a new option to the CLI parser, allowing it to be recognized and processed
 * during command-line parsing. The option must have a unique long or short identifier.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param option A pointer to the `CliOption` structure defining the option. Must not be NULL.
 *
 * @return `true` if the option was successfully registered, `false` otherwise.
 *
 * @note false if the `parser` or `option` is NULL, or if a duplicate option is detected.
 * @note true if the option was successfully registered.
 */
bool cli_register_option(CliParser *parser, const CliOption *option) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Parser is NULL in cli_register_option.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return false;
    }
    if (option == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Option is NULL in cli_register_option.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return false;
    }

    // Check for duplicate options by long option or short option
    for (size_t i = 0; i < parser->numOptions; i++) {
        if ((option->longOpt && parser->options[i].longOpt && strcmp(parser->options[i].longOpt, option->longOpt) == 0) ||
            (option->shortOpt && parser->options[i].shortOpt && option->shortOpt == parser->options[i].shortOpt)) {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Duplicate option '%s' in cli_register_option.\n", option->longOpt ? option->longOpt : "");
            cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND; 
            
            #ifdef CLI_LOGGING_ENABLE
                fmt_fprintf(stderr, cli_last_error.message);
            #endif
            return false;
        }
    }

    // Allocate or resize the options array
    void *temp = realloc(parser->options, (parser->numOptions + 1) * sizeof(CliOption));
    if (temp == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed in cli_register_option.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return false;
    }
    parser->options = temp;

    // Add the new option
    parser->options[parser->numOptions] = *option;
    parser->numOptions++;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Option '%s' registered successfully.\n", option->longOpt ? option->longOpt : "");
    cli_last_error.code = CLI_SUCCESS;
    
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, cli_last_error.message);
    #endif
    return true;
}

/**
 * @brief Displays an error message using the CLI parser's error handler.
 *
 * This function outputs an error message using the parser's configured error handler, if one is set.
 * If no custom error handler is provided, the error message is printed to `stderr`.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param error The error message to display. Must not be NULL or empty.
 *
 * @return void
 *
 * @note If the `parser` or `error` is NULL, an error is logged and the function returns without displaying anything.
 */
void cli_display_error(const CliParser *parser, const char *error) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: NULL parser provided to cli_display_error.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    if (error == NULL || error[0] == '\0') {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: NULL or empty error message provided to cli_display_error.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    if (parser->errorHandler) {
        // Assuming error handlers are designed to work without parser's userData
        parser->errorHandler(parser, error, NULL); 
    } 
    else {
        fmt_fprintf(stderr, "Error: %s\n", error);
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error displayed: %s", error);
    cli_last_error.code = CLI_ERROR_NONE; 
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, cli_last_error.message);
    #endif
}

/**
 * @brief Prints the version information for the application.
 *
 * This function outputs the version information of the application, using the program name
 * specified when the parser was created, followed by the provided version string.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param version A string containing the version information. Must not be NULL.
 *
 * @return void
 *
 * @note If the `parser` or `version` is NULL, an error is logged and the function returns without printing anything.
 */
void cli_print_version(const CliParser *parser, const char *version) {
    if (parser == NULL || version == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Invalid arguments provided to cli_print_version.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    fmt_fprintf(stdout, "%s version %s\n", parser->progName ? parser->progName : "Application", version);

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Version information printed successfully.\n");
    cli_last_error.code = CLI_SUCCESS;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, cli_last_error.message);
    #endif
}

/**
 * @brief Finds and returns a pointer to a command by its name.
 *
 * This function searches for a command in the CLI parser by its name. If found, a pointer to the command is returned.
 * If the command is not found, NULL is returned.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param name The name of the command to search for. Must not be NULL.
 *
 * @return A pointer to the `CliCommand` structure if the command is found, or NULL if not found.
 * @note If the `parser` or `name` is NULL, an error is logged and NULL is returned.
 */
const CliCommand* cli_find_command(const CliParser *parser, const char *name) {
    if (parser == NULL || name == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Invalid arguments provided to cli_find_command.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return NULL;
    }

    for (size_t i = 0; i < parser->numCommands; i++) {
        if (strcmp(parser->commands[i].name, name) == 0) {
            return &parser->commands[i];
        }
    }

    return NULL; // Command not found
}

/**
 * @brief Finds and returns a pointer to an option by its long or short name.
 *
 * This function searches for an option in the CLI parser by its long option name or short option character.
 * If found, a pointer to the option is returned. If the option is not found, NULL is returned.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param longOpt The long option name to search for. Can be NULL if searching by short option.
 * @param shortOpt The short option character to search for. Can be '\0' if searching by long option.
 *
 * @return A pointer to the `CliOption` structure if the option is found, or NULL if not found.
 * @note If the `parser` is NULL or both `longOpt` and `shortOpt` are not provided, an error is logged and NULL is returned.
 */
const CliOption* cli_find_option(const CliParser *parser, const char *longOpt, char shortOpt) {
    if (parser == NULL || (longOpt == NULL && shortOpt == '\0')) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Invalid arguments provided to cli_find_option.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return NULL;
    }

    for (size_t i = 0; i < parser->numOptions; i++) {
        if ((longOpt && parser->options[i].longOpt && strcmp(parser->options[i].longOpt, longOpt) == 0) ||
            (shortOpt && parser->options[i].shortOpt == shortOpt)) {
            return &parser->options[i];
        }
    }

    return NULL; // Option not found
}

/**
 * @brief Updates the description of a command or option.
 *
 * This function updates the description of a command or option in the CLI parser. The description is updated only
 * if the specified command or option is found.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param name The name of the command or option to update. Must not be NULL.
 * @param newDescription The new description to set. Must not be NULL.
 * @param isCommand A boolean flag indicating whether the name refers to a command (true) or an option (false).
 *
 * @return void
 * @note If the `parser`, `name`, or `newDescription` is NULL, an error is logged and the function returns without updating anything.
 */
void cli_update_description(CliParser *parser, const char *name, const char *newDescription, bool isCommand) {
    if (parser == NULL || name == NULL || newDescription == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Invalid arguments provided to cli_update_description.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    bool found = false;

    if (isCommand) {
        for (size_t i = 0; i < parser->numCommands; i++) {
            if (strcmp(parser->commands[i].name, name) == 0) {
                // Check if the current description is dynamically allocated
                if (parser->commands[i].description != NULL) {
                    free((void*)parser->commands[i].description);
                }
                parser->commands[i].description = string_strdup(newDescription);
                found = true;
                break;
            }
        }
    } else {
        for (size_t i = 0; i < parser->numOptions; i++) {
            if ((parser->options[i].longOpt && strcmp(parser->options[i].longOpt, name) == 0) ||
                (parser->options[i].shortOpt == name[0])) {
                // Check if the current description is dynamically allocated
                if (parser->options[i].description != NULL) {
                    free((void*)parser->options[i].description);
                }
                parser->options[i].description = string_strdup(newDescription); 
                found = true;
                break;
            }
        }
    }

    if (!found) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: '%s' not found in cli_update_description.\n", name);
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Description updated successfully for '%s'.\n", name);
    cli_last_error.code = CLI_SUCCESS;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, cli_last_error.message);
    #endif
}

/**
 * @brief Parses command-line arguments based on the registered options and commands.
 *
 * This function processes the command-line arguments, first handling options
 * (those that start with '-' or '--') and then looking for and executing commands.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param argc The argument count, typically from the main function.
 * @param argv The argument vector, typically from the main function.
 *
 * @return `CliStatusCode` indicating the result of the parsing operation.
 *
 * @note CLI_SUCCESS on successful parsing and execution.
 * @note CLI_ERROR_INVALID_ARGUMENT if the parser or arguments are invalid.
 * @note CLI_ERROR_COMMAND_NOT_FOUND if no valid command was found in the arguments.
 */
CliStatusCode cli_parse_args(CliParser *parser, int argc, char *argv[]) {
    if (!parser || argc < 1 || !argv) {
        fmt_printf("Debug: Invalid CLI parser setup or arguments.\n");
        return CLI_ERROR_INVALID_ARGUMENT;
    }

    fmt_printf("Debug: Starting argument parsing.\n");
    bool commandFound = false;

    // First phase: Process all options
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-' || argv[i][1] == '-') { // Assuming options start with '-' or '--'
            bool isOptionProcessed = false;

            // Check each option group
            for (size_t j = 0; j < parser->numOptionGroups; ++j) {
                CliOptionGroup *group = &parser->optionGroups[j];
                for (size_t k = 0; k < group->numOptions; ++k) {
                    CliOption *option = &group->options[k];
                    // Match option by long option or short option
                    if ((option->longOpt && strcmp(argv[i], option->longOpt) == 0) ||
                        (option->shortOpt && argv[i][1] == option->shortOpt)) {
                        fmt_printf("Debug: Matched option %s.\n", argv[i]);
                        const char *value = NULL;
                        if (option->optionType != CLI_NO_ARG && i + 1 < argc) {
                            value = argv[++i]; // Increment i to skip option's argument
                            fmt_printf("Debug: Option argument %s.\n", value);
                        }
                        // Call the option handler with the value
                        if (option->handler) {
                            fmt_printf("Debug: Calling handler for option %s with value %s.\n", argv[i], value ? value : "NULL");
                            option->handler(option, value, option->userData);
                            isOptionProcessed = true;
                            break; // Break if the option is processed
                        }
                    }
                }
                if (isOptionProcessed) break; // Break the outer loop if the option is processed
            }
            if (!isOptionProcessed) {
                fmt_printf("Debug: Unrecognized option %s.\n", argv[i]);
                // You might want to handle unrecognized options here
            }
        }
    }

    // Second phase: Look for command
    for (int i = 1; i < argc; ++i) {
        if (!(argv[i][0] == '-' || argv[i][1] == '-')) { // This is not an option, treat it as a command
            const CliCommand *command = cli_find_command(parser, argv[i]);
            if (command) {
                fmt_printf("Debug: Found command %s. Calling handler.\n", argv[i]);
                command->handler(command, argc - i - 1, &argv[i + 1], command->userData);
                commandFound = true;
                break; // Stop processing after command is found
            }
        }
    }

    if (!commandFound) {
        fmt_printf("Debug: No valid command found. Consider printing help here.\n");
        // Optionally print help or handle the absence of a valid command
        return CLI_ERROR_COMMAND_NOT_FOUND;
    }

    fmt_printf("Debug: Command processed successfully.\n");
    return CLI_SUCCESS;
}

/**
 * @brief Retrieves the last error that occurred in the CLI parser.
 *
 * This function returns the last error information that occurred in the parser,
 * including the error code and message.
 *
 * @param parser A pointer to the `CliParser` instance. Can be NULL.
 *
 * @return A `CliError` structure containing the last error details.
 * @note If the `parser` is NULL, the function logs an error and returns the global last error.
 */
CliError cli_get_last_error(const CliParser *parser) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: cli_get_last_status called with NULL parser.\n");
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: cli_get_last_status called with NULL parser.\n");
        #endif
        return cli_last_error;
    }

    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, "Success: Parser arg is not null in cli_get_last_error.\n");
    #endif 
    return cli_last_error;
}

/**
 * @brief Sets the default command handler for the parser.
 *
 * This function allows setting a default command handler that will be invoked
 * when no specific command is found in the arguments. The handler can be cleared
 * by passing NULL.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param handler The function to be set as the default command handler. Can be NULL.
 *
 * @return void
 * @note If the `parser` is NULL, the function logs an error and does not set the handler.
 * If the handler is NULL, the default handler is cleared.
 */
void cli_set_default_command_handler(CliParser *parser, CliCommandHandler handler) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: NULL parser provided to cli_set_default_command_handler.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return;
    }

    if (handler == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Warning: NULL handler provided to cli_set_default_command_handler. Default handler cleared.");
        cli_last_error.code = CLI_SUCCESS; 
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stdout, "%s\n", cli_last_error.message);
        #endif
    } 
    else {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Default command handler set successfully.");
        cli_last_error.code = CLI_SUCCESS;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stdout, "%s\n", cli_last_error.message);
        #endif
    }

    parser->defaultCommandHandler = handler;
}

/**
 * @brief Unregisters a previously registered option from the parser.
 *
 * This function removes an option from the parser by either its long or short
 * option identifier. It also handles the memory deallocation for the option's
 * name and description.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param longOpt The long option name to unregister. Can be NULL if using shortOpt.
 * @param shortOpt The short option character to unregister. Can be '\0' if using longOpt.
 *
 * @return `true` if the option was successfully unregistered, `false` otherwise.
 *
 * @note false if the `parser` is NULL, or if both `longOpt` and `shortOpt` are NULL.
 * @note true if the option was successfully found and unregistered.
 */
bool cli_unregister_option(CliParser *parser, const char *longOpt, char shortOpt) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Parser is NULL in cli_unregister_option.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    if (longOpt == NULL && shortOpt == '\0') {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Both long and short option identifiers are NULL or empty in cli_unregister_option.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    for (size_t i = 0; i < parser->numOptions; i++) {
        bool match = (longOpt && parser->options[i].longOpt && strcmp(parser->options[i].longOpt, longOpt) == 0) ||
                     (shortOpt && parser->options[i].shortOpt == shortOpt);

        if (match) {
            if (parser->options[i].longOpt) {
                free((void*)parser->options[i].longOpt);
            }
            if (parser->options[i].description) {
                free((void*)parser->options[i].description);
            }

            // Option found, shift the rest of the options up by one to overwrite the unregistered option
            size_t remainingOptions = parser->numOptions - i - 1;
            if (remainingOptions > 0) {
                memmove(&parser->options[i], &parser->options[i + 1], remainingOptions * sizeof(CliOption));
            }

            // Decrement the total number of options
            parser->numOptions--;

            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Option '%s' unregistered successfully.", longOpt ? longOpt : shortOpt ? &shortOpt : "N/A");
            cli_last_error.code = CLI_SUCCESS;
            #ifdef CLI_LOGGING_ENABLE
                fmt_fprintf(stdout, "%s\n", cli_last_error.message);
            #endif
            return true;
        }
    }

    // Option not found
    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Option '%s' not found in cli_unregister_option.", longOpt ? longOpt : shortOpt ? &shortOpt : "N/A");
    cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, "%s\n", cli_last_error.message);
    #endif

    return false;
}

/**
 * @brief Unregisters a previously registered command from the parser.
 *
 * This function removes a command from the parser by its name. It also handles
 * the memory deallocation for the command's name and description.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param name The name of the command to unregister. Must not be NULL.
 *
 * @return `true` if the command was successfully unregistered, `false` otherwise.
 *
 * @note false if the `parser` or `name` is NULL, or if the command was not found.
 * @note true if the command was successfully found and unregistered.
 */
bool cli_unregister_command(CliParser *parser, const char *name) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Parser is NULL in cli_unregister_command.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    if (name == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Command name is NULL in cli_unregister_command.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    for (size_t i = 0; i < parser->numCommands; i++) {
        if (strcmp(parser->commands[i].name, name) == 0) {
            free((void*)parser->commands[i].name);
            if (parser->commands[i].description) {
                free((void*)parser->commands[i].description);
            }

            // Command found, shift the rest of the commands up by one to overwrite the unregistered command
            size_t remainingCommands = parser->numCommands - i - 1;
            if (remainingCommands > 0) {
                memmove(&parser->commands[i], &parser->commands[i + 1], remainingCommands * sizeof(CliCommand));
            }

            // Decrement the total number of commands
            parser->numCommands--;

            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Command '%s' unregistered successfully.", name);
            cli_last_error.code = CLI_SUCCESS;
            #ifdef CLI_LOGGING_ENABLE
                fmt_fprintf(stdout, "%s\n", cli_last_error.message);
            #endif
            return true;
        }
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Command '%s' not found in cli_unregister_command.", name);
    cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, "%s\n", cli_last_error.message);
    #endif

    return false;
}

/**
 * @brief Parses command-line arguments using a specified delimiter for argument splitting.
 *
 * This function processes the command-line arguments, splitting any arguments
 * containing the specified delimiter before parsing them. This allows complex
 * command-line inputs to be handled more flexibly.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param argc The argument count, typically from the main function.
 * @param argv The argument vector, typically from the main function.
 * @param delimiter The delimiter used to split arguments. Must not be NULL.
 *
 * @return `true` if the arguments were successfully parsed, `false` otherwise.
 *
 * @note false if the `parser`, `argv`, or `delimiter` is NULL, or if parsing fails.
 * @note true if the arguments were successfully parsed.
 */
bool cli_parse_args_with_delimiter(CliParser *parser, int argc, char *argv[], const char *delimiter) {
    if (parser == NULL || argv == NULL || delimiter == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Invalid arguments provided to cli_parse_args_with_delimiter.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    int totalArgs = 0;
    char** newArgv = NULL; 

    for (int i = 0; i < argc; ++i) {
        int splitCount = 0;
        char** splitArgs = split_string(argv[i], delimiter, &splitCount);

        if (splitCount > 1) {
            // Argument contains the delimiter and was split
            newArgv = realloc(newArgv, sizeof(char*) * (totalArgs + splitCount));
            for (int j = 0; j < splitCount; ++j) {
                newArgv[totalArgs++] = splitArgs[j];
            }
            free(splitArgs); // only free the array, not the strings,
        }
         else {
            // Argument does not contain the delimiter, or splitting failed
            newArgv = realloc(newArgv, sizeof(char*) * (totalArgs + 1));
            newArgv[totalArgs++] = argv[i];
            if (splitCount == 1) {
                free(splitArgs[0]);
            }
            free(splitArgs);
        }
    }

    // Use the existing cli_parse_args function to parse the newly constructed argv
    CliStatusCode status = cli_parse_args(parser, totalArgs, newArgv);

    for (int i = 0; i < totalArgs; ++i) {
        if (i >= argc) { // Only free new arguments
            free(newArgv[i]);
        }
    }
    free(newArgv);
    return status == CLI_SUCCESS;
}

/**
 * @brief Adds a new group of options to the CLI parser.
 *
 * This function registers a group of related options under a specified group name
 * in the parser. This allows for better organization and handling of options.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param groupName The name of the option group. Must not be NULL.
 * @param options An array of `CliOption` structures to be added to the group. Must not be NULL.
 * @param numOptions The number of options in the `options` array. Must be greater than 0.
 *
 * @return void
 *
 * @note If any of the parameters are invalid, the function logs an error and does not add the group.
 */
void cli_add_option_group(CliParser *parser, const char *groupName, const CliOption *options, size_t numOptions) {
    if (parser == NULL || groupName == NULL || options == NULL || numOptions == 0) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Invalid arguments provided to cli_add_option_group.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    // Allocate memory for the new option group
    CliOptionGroup *newGroup = realloc(parser->optionGroups, sizeof(CliOptionGroup) * (parser->numOptionGroups + 1));
    if (newGroup == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Memory allocation failed for new option group in cli_add_option_group.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }
    parser->optionGroups = newGroup;

    // Initialize the new option group
    CliOptionGroup *group = &parser->optionGroups[parser->numOptionGroups];
    group->groupName = string_strdup(groupName);
    group->numOptions = numOptions;
    group->options = malloc(sizeof(CliOption) * numOptions);
    if (group->options == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Memory allocation failed for options in new option group in cli_add_option_group.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;

        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    // Copy the options into the new group
    for (size_t i = 0; i < numOptions; i++) {
        group->options[i] = options[i];
    }

    parser->numOptionGroups++;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Option group '%s' added successfully.\n", groupName);
    cli_last_error.code = CLI_SUCCESS;

    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, "Option group '%s' added successfully.\n", groupName);
    #endif
}

/**
 * @brief Removes an option group from the CLI parser by its name.
 *
 * This function searches for an option group by name and removes it from the CLI parser.
 * It also handles memory deallocation for the group's name and its options.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param groupName The name of the option group to remove. Must not be NULL.
 *
 * @return void
 * @note If the `parser` or `groupName` is NULL, or if the group is not found, the function logs an error.
 */
void cli_remove_option_group(CliParser *parser, const char *groupName) {
    if (parser == NULL || groupName == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Invalid arguments provided to cli_remove_option_group: parser or groupName is NULL.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    for (size_t i = 0; i < parser->numOptionGroups; ++i) {
        if (strcmp(parser->optionGroups[i].groupName, groupName) == 0) {
            free((void *)parser->optionGroups[i].groupName);
            free(parser->optionGroups[i].options);

            // Shift the rest of the option groups down by one
            for (size_t j = i; j < parser->numOptionGroups - 1; ++j) {
                parser->optionGroups[j] = parser->optionGroups[j + 1];
            }

            // Decrease the number of option groups and reallocate memory
            parser->numOptionGroups--;
            if (parser->numOptionGroups == 0) {
                free(parser->optionGroups);
                parser->optionGroups = NULL;
            } 
            else {
                CliOptionGroup *temp = realloc(parser->optionGroups, sizeof(CliOptionGroup) * parser->numOptionGroups);
                if (temp == NULL) {
                    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Memory reallocation failed in cli_remove_option_group.\n");
                    cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
                    #ifdef CLI_LOGGING_ENABLE
                        fmt_fprintf(stderr, cli_last_error.message);
                    #endif
                    return;
                }
                parser->optionGroups = temp;
            }

            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Option group '%s' removed successfully.\n", groupName);
            cli_last_error.code = CLI_SUCCESS;
            #ifdef CLI_LOGGING_ENABLE
                fmt_fprintf(stdout, cli_last_error.message);
            #endif
            return;
        }
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Option group '%s' not found in cli_remove_option_group.\n", groupName);
    cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, cli_last_error.message);
    #endif
}

/**
 * @brief Enters an interactive mode where the user can input and execute commands repeatedly.
 *
 * This function prompts the user for commands in a loop, parses them, and executes the corresponding
 * actions until the user types "exit" or "quit". It provides a simple command-line interface (CLI)
 * experience within a running application.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param prompt The prompt string to display before each user input. Must not be NULL.
 *
 * @return void
 * @note If the `parser` is NULL, the function logs an error and exits without entering interactive mode.
 */
void cli_enter_interactive_mode(CliParser *parser, const char *prompt) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Invalid parser provided to cli_enter_interactive_mode.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif
        return;
    }

    char inputBuffer[1024]; 
    fmt_printf("%s ", prompt);
    while (fgets(inputBuffer, sizeof(inputBuffer), stdin)) {
        // Remove trailing newline, if present
        inputBuffer[strcspn(inputBuffer, "\n")] = 0;

        if (strcmp(inputBuffer, "exit") == 0 || strcmp(inputBuffer, "quit") == 0) {
            break;
        }

        // Tokenize the input to simulate argc/argv
        char *argv[64]; // Adjust size as necessary
        int argc = 0;
        char *token = strtok(inputBuffer, " ");
        while (token != NULL && argc < (int)(sizeof(argv) / sizeof(argv[0]) - 1)) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL; // NULL-terminate the argument list

        if (argc > 0) {
            // Parse and execute the command
            cli_parse_args(parser, argc, argv);
        }
        fmt_printf("%s ", prompt); // Display the prompt again for the next input
    }

    // Print a goodbye message or perform any cleanup before exiting interactive mode
    fmt_printf("Exiting interactive mode.\n");
}

/**
 * @brief Sets a custom error message for a specific option when its validation fails.
 *
 * This function assigns a custom error message to a specific option identified by
 * its long or short option name. The custom error message is displayed when the
 * option fails validation during argument parsing.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param longOpt The long option name. Can be NULL if using shortOpt.
 * @param shortOpt The short option character. Can be '\0' if using longOpt.
 * @param errorMessage The custom error message to set. Must not be NULL.
 *
 * @return void
 * @note If the `parser`, `longOpt` and `shortOpt` are both NULL, or if `errorMessage` is NULL,
 * the function logs an error and does not set the custom error message.
 */
void cli_set_option_error_message(CliParser *parser, const char *longOpt, char shortOpt, const char *errorMessage) {
    if (!parser) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Parser cannot be NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return;
    }

    if (!longOpt && shortOpt == '\0') {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Both longOpt and shortOpt cannot be empty.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return;
    }

    if (!errorMessage) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error message cannot be NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return;
    }

    bool found = false;
    for (size_t i = 0; i < parser->numOptions; ++i) {
        if ((longOpt && parser->options[i].longOpt && strcmp(parser->options[i].longOpt, longOpt) == 0) ||
            (shortOpt && parser->options[i].shortOpt == shortOpt)) {
            // Free existing error message if present
            if (parser->options[i].customErrorMessage) {
                free(parser->options[i].customErrorMessage);
            }

            // Allocate memory and copy the new error message
            parser->options[i].customErrorMessage = strdup(errorMessage);
            if (!parser->options[i].customErrorMessage) {
                snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Failed to allocate memory for custom error message.");
                cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
                #ifdef CLI_LOGGING_ENABLE
                    fmt_fprintf(stderr, "%s\n", cli_last_error.message);
                #endif
                return;
            }

            found = true;
            break;
        }
    }

    if (!found) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Option not found.");
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
    } 
    else {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Custom error message set successfully.");
        cli_last_error.code = CLI_SUCCESS;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stdout, "%s\n", cli_last_error.message);
        #endif
    }
}

/**
 * @brief Validates an argument provided for a specific CLI option using its custom validator function.
 *
 * This function checks if the argument provided for a CLI option is valid according to the
 * custom validator function associated with that option. If the option has no validator, the
 * function assumes the argument is valid.
 *
 * @param option A pointer to the `CliOption` instance. Must not be NULL.
 * @param value The argument value to validate. Must not be NULL.
 *
 * @return `true` if the argument is valid or if there is no validator, `false` otherwise.
 *
 * @note false if `option` or `value` is NULL, or if validation fails.
 * @note true if the argument passes validation or if no validator is present.
 */
bool cli_validate_option_argument(const CliOption *option, const char *value) {
    // Validate input parameters
    if (option == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), 
                 "Error: Option is NULL in cli_validate_option_argument.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    if (value == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), 
                 "Error: Value is NULL in cli_validate_option_argument.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    // Check if the option has a validator
    if (option->validator) {
        bool isValid = option->validator(value, option->userData);
        if (!isValid) {
            // Validator function returned false
            if (option->validationErrorMessage) {
                snprintf(cli_last_error.message, sizeof(cli_last_error.message), 
                         "%s", option->validationErrorMessage);
            } 
            else {
                snprintf(cli_last_error.message, sizeof(cli_last_error.message), 
                         "Error: Validation failed for the given option argument.");
            }
            cli_last_error.code = CLI_ERROR_VALIDATION_FAILED;
            #ifdef CLI_LOGGING_ENABLE
                fmt_fprintf(stderr, "%s\n", cli_last_error.message);
            #endif
            return false;
        }
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Validation Passed in cli_validate_option_argument.\n");
        cli_last_error.code = CLI_SUCCESS;
        return true; // Validation passed
    } 
    else {
        // No validator provided, assume the value is valid
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Validation Passed in cli_validate_option_argument.\n");
        cli_last_error.code = CLI_SUCCESS;
        return true;
    }
}

/**
 * @brief Sets a hook function to be executed before any command execution.
 *
 * This function allows the user to specify a hook that will be called automatically
 * before any command is executed. This can be used for setup, logging, or any
 * other pre-processing tasks. The hook can be cleared by passing `NULL`.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param hook The function to be set as the pre-execution hook. Can be NULL to clear the hook.
 *
 * @return void
 * @note If the `parser` is NULL, the function will log an error and return without setting the hook.
 */
void cli_set_pre_execution_hook(CliParser *parser, CliPreExecutionHook hook) {
    // Validate the input parameters
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), 
                 "Error: Parser is NULL in cli_set_pre_execution_hook.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return;
    }

    // It's valid for hook to be NULL, as the user might want to clear a previously set hook.
    parser->preExecutionHook = hook;

    // Optionally, you might want to allow setting user data for the hook function.
    // If you add a parameter for user data, you would set it here.

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), 
             hook ? "Pre-execution hook set successfully." : "Pre-execution hook cleared.");
    cli_last_error.code = CLI_SUCCESS;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, "%s\n", cli_last_error.message);
    #endif
}

/**
 * @brief Sets a hook function to be executed after any command execution.
 *
 * This function allows the user to specify a hook that will be called automatically
 * after any command is executed. This can be used for cleanup, logging, or any
 * other post-processing tasks. The hook can be cleared by passing `NULL`.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param hook The function to be set as the post-execution hook. Can be NULL to clear the hook.
 *
 * @return void

 * @note If the `parser` is NULL, the function will log an error and return without setting the hook.
 */
void cli_set_post_execution_hook(CliParser *parser, CliPostExecutionHook hook) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Parser is NULL in cli_set_post_execution_hook.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return;
    }

    // It's valid for hook to be NULL to clear a previously set hook
    parser->postExecutionHook = hook;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message),
             hook ? "Post-execution hook set successfully." : "Post-execution hook cleared.");
    cli_last_error.code = CLI_SUCCESS;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, "%s\n", cli_last_error.message);
    #endif
}

/**
 * @brief Enables or disables the pipelining feature in the CLI parser.
 *
 * Pipelining allows the output of one command to be used as the input for another command.
 * This function toggles the feature based on the `enable` parameter.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param enable A boolean value indicating whether to enable (`true`) or disable (`false`) pipelining.
 *
 * @return void
 *
 * @note If the `parser` is NULL, the function will log an error and return without enabling/disabling pipelining.
 */
void cli_enable_pipelining(CliParser *parser, bool enable) {
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Parser is NULL in cli_enable_pipelining.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return;
    }

    // Set or unset the pipelining feature based on the 'enable' parameter
    parser->pipeliningEnabled = enable;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message),
             "Pipelining %s successfully.", enable ? "enabled" : "disabled");
    cli_last_error.code = CLI_SUCCESS;
    #ifdef CLI_LOGGING_ENABLE
    fmt_fprintf(stdout, "%s\n", cli_last_error.message);
    #endif
}

/**
 * @brief Registers an alias for an existing command in the CLI parser.
 *
 * This function allows you to define an alternative name (alias) for an existing
 * command in the CLI parser. The alias will behave exactly like the original command.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param commandName The name of the existing command to alias. Must not be NULL.
 * @param alias The alias name for the command. Must not be NULL.
 * 
 * @return `true` if the alias was successfully added, `false` otherwise.
 *         If false, the function will also set an appropriate error code and message
 *         in the `cli_last_error` structure.
 *
 * @note false if `parser`, `commandName`, or `alias` is NULL.
 * @note false if the specified original command is not found in the parser.
 * @note false if the alias name already exists as a command.
 * @note true if the alias was successfully added.
 */
bool cli_register_command_alias(CliParser *parser, const char *commandName, const char *alias) {
    if (!parser) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Parser is NULL in cli_register_command_alias.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;

        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    if (!commandName || !alias) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Command name or alias is NULL in cli_register_command_alias.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;

        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    // Ensure the command exists
    const CliCommand* originalCommand = cli_find_command(parser, commandName);
    if (!originalCommand) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Command '%s' does not exist.", commandName);
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    // Ensure the alias does not already exist as a command
    if (cli_find_command(parser, alias)) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Alias '%s' already exists as a command.", alias);
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    // Create a new command with the same handler and user data as the original
    CliCommand aliasCommand = {
        .name = string_strdup(alias), // Ensure you handle memory allocation and deallocation properly
        .handler = originalCommand->handler,
        .description = originalCommand->description, // You may choose to share the description or provide a custom one
        .userData = originalCommand->userData
    };

    // Register the alias command
    return cli_register_command(parser, &aliasCommand);
}

/**
 * @brief Prompts the user for confirmation with a customizable message.
 *
 * This function displays a prompt message asking the user for a yes/no confirmation.
 * It waits for the user to input 'y' or 'n' and returns `true` for 'y' and `false` for 'n'.
 * If the input is invalid, the function re-prompts the user.
 *
 * @param promptMessage The message to display when prompting for confirmation. Must not be NULL.
 * 
 * @return `true` if the user confirms with 'y', `false` if the user declines with 'n'.
 *         If the input is invalid, the function re-prompts the user.
 *
 * @note false if `promptMessage` is NULL or if there is an error reading input.
 * @note true if the user inputs 'y'.
 * @note false if the user inputs 'n'.
 */
bool cli_prompt_confirmation(const char *promptMessage) {
    if (!promptMessage) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: promptMessage is NULL in cli_prompt_confirmation.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;

        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Prompt message is NULL.\n");
        #endif
        return false;
    }

    fmt_printf("%s (y/n): ", promptMessage);
    char response[4]; // Buffer for user input; large enough for "y\n" or "n\n" and null terminator
    if (fgets(response, sizeof(response), stdin) == NULL) {
        fmt_fprintf(stderr, "Error reading input.\n");
        return false;
    }

    // Trim newline character if present
    response[strcspn(response, "\n")] = 0;

    if (strcasecmp(response, "y") == 0) {
        return true;
    } 
    else if (strcasecmp(response, "n") == 0) {
        return false;
    } 
    else {
        fmt_fprintf(stderr, "Invalid input. Please enter 'y' or 'n'.\n");
        return cli_prompt_confirmation(promptMessage); // Recursive call to re-prompt the user
    }
}

/**
 * @brief Processes a specific group of options within the CLI parser.
 *
 * This function processes the command line arguments for a specific option group,
 * ensuring that each option within the group is handled according to its type
 * (e.g., required argument, optional argument).
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param groupName The name of the option group to process. Must not be NULL.
 * @param argc The argument count, typically from the main function.
 * @param argv The argument vector, typically from the main function.
 * 
 * @return `true` if the option group was successfully processed, `false` otherwise.
 *         If false, the function will also set an appropriate error code and message
 *         in the `cli_last_error` structure.
 *
 * @note false if `parser`, `groupName`, or `argv` is NULL, or if `argc` is less than 1.
 * @note false if the specified option group is not found in the parser.
 * @note false if a required option within the group is not found.
 * @note true if the option group was processed successfully.
 */
bool cli_process_option_group(CliParser *parser, const char *groupName, int argc, char *argv[]) {
    if (!parser || !groupName || argc < 1 || !argv) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Invalid arguments provided to cli_process_option_group.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    // Find the option group by name
    bool groupFound = false;
    CliOptionGroup *group = NULL;
    for (size_t i = 0; i < parser->numOptionGroups; ++i) {
        if (strcmp(parser->optionGroups[i].groupName, groupName) == 0) {
            group = &parser->optionGroups[i];
            groupFound = true;
            break;
        }
    }

    if (!groupFound) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Option group '%s' not found.", groupName);
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    // Process each option in the group
    for (size_t i = 0; i < group->numOptions; ++i) {
        CliOption *option = &group->options[i];
        bool optionFound = false;
        for (int j = 1; j < argc; ++j) { // Skip the program name
            if (strcmp(argv[j], option->longOpt) == 0 || (option->shortOpt && argv[j][0] == option->shortOpt)) {
                optionFound = true;
                const char *value = NULL;
                if (option->optionType != CLI_NO_ARG && j + 1 < argc) {
                    value = argv[++j]; // Move to the next argument for the value
                }
                if (option->handler) {
                    option->handler(option, value, parser->userData);
                }
                break;
            }
        }
        if (!optionFound && option->optionType == CLI_REQUIRED_ARG) {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                     "Required option '%s' not found in group '%s'.", option->longOpt, groupName);
            cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
            #ifdef CLI_LOGGING_ENABLE
                fmt_fprintf(stderr, "%s\n", cli_last_error.message);
            #endif
            return false;
        }
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message),
             "Option group '%s' processed successfully.", groupName);
    cli_last_error.code = CLI_SUCCESS;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, "%s\n", cli_last_error.message);
    #endif
    return true;
}

/**
 * @brief Adds an alias for an existing option in the CLI parser.
 *
 * This function allows you to define an alternative name (alias) for an existing
 * option in the CLI parser. The alias will behave exactly like the original option.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param optionName The name of the existing option to alias. Must not be NULL.
 * @param alias The alias name for the option. Must not be NULL.
 * 
 * @return `true` if the alias was successfully added, `false` otherwise.
 *         If false, the function will also set an appropriate error code and message
 *         in the `cli_last_error` structure.
 *
 * @note false if `parser`, `optionName`, or `alias` is NULL.
 * @note false if the specified original option is not found in the parser.
 * @note false if the alias name already exists as an option.
 * @note false if memory allocation fails during the process.
 * @note true if the alias was successfully added.
 */
bool cli_add_option_alias(CliParser *parser, const char *optionName, const char *alias) {
    if (!parser) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Parser is NULL in cli_add_option_alias.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    if (!optionName || !alias) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: optionName or alias is NULL in cli_add_option_alias.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    // Search for the original option to alias
    const CliOption *originalOption = NULL;
    for (size_t i = 0; i < parser->numOptions; i++) {
        if (parser->options[i].longOpt && strcmp(parser->options[i].longOpt, optionName) == 0) {
            originalOption = &parser->options[i];
            break;
        }
    }

    if (!originalOption) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Original option '%s' not found in cli_add_option_alias.", optionName);
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    // Ensure the alias does not already exist
    for (size_t i = 0; i < parser->numOptions; i++) {
        if (parser->options[i].longOpt && strcmp(parser->options[i].longOpt, alias) == 0) {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                     "Error: Alias '%s' already exists.", alias);
            cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
            #ifdef CLI_LOGGING_ENABLE
                fmt_fprintf(stderr, "%s\n", cli_last_error.message);
            #endif
            return false;
        }
    }

    // Expand the options array to accommodate the new alias option
    CliOption *newOptions = realloc(parser->options, sizeof(CliOption) * (parser->numOptions + 1));
    if (!newOptions) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Memory allocation failed in cli_add_option_alias.");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }
    parser->options = newOptions;

    // Create and add the alias option
    CliOption aliasOption = *originalOption; // Copy the original option
    aliasOption.longOpt = string_strdup(alias); // Duplicate the alias name
    if (!aliasOption.longOpt) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: strdup failed for alias name in cli_add_option_alias.");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    parser->options[parser->numOptions] = aliasOption;
    parser->numOptions++;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message),
             "Success: Option alias '%s' added for '%s'.", alias, optionName);
    cli_last_error.code = CLI_SUCCESS;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, "%s\n", cli_last_error.message);
    #endif

    return true;
}

/**
 * @brief Sets a dependency between two options in the CLI parser.
 *
 * This function establishes a dependency where the presence of one option
 * (identified by `longOpt` or `shortOpt`) requires the presence of another
 * option (identified by `dependsOnLongOpt` or `dependsOnShortOpt`).
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param longOpt The long option name that has a dependency (e.g., "--verbose").
 *                Can be NULL if `shortOpt` is provided.
 * @param shortOpt The short option character that has a dependency (e.g., 'v').
 * Can be '\0' if `longOpt` is provided.
 * @param dependsOnLongOpt The long option name that is required as a dependency. Can be NULL if `dependsOnShortOpt` is provided.
 * @param dependsOnShortOpt The short option character that is required as a dependency.
 * Can be '\0' if `dependsOnLongOpt` is provided.
 * 
 * @return `true` if the dependency was successfully established, `false` otherwise.
 *         If false, the function will also set an appropriate error code and message
 *         in the `cli_last_error` structure.
 *
 * @note This function currently does not store the dependency relationship for 
 *       validation during parsing. Future implementations may include this feature.
 *
 * @note false if the `parser` is NULL.
 * @note false if both `longOpt` and `shortOpt` are NULL or empty, or if both
 *         `dependsOnLongOpt` and `dependsOnShortOpt` are NULL or empty.
 * @note false if the specified option or its dependency is not found in the parser.
 * @note true if the option dependency was successfully set.
 */
bool cli_set_option_dependencies(CliParser *parser, const char *longOpt, char shortOpt, const char *dependsOnLongOpt, char dependsOnShortOpt) {
    // Validate parser and option names
    if (!parser) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Parser is NULL in cli_set_option_dependencies.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    if ((!longOpt && shortOpt == '\0') || (!dependsOnLongOpt && dependsOnShortOpt == '\0')) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Both source and dependency options cannot be NULL or empty in cli_set_option_dependencies.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    // Find the option that has the dependency
    bool foundOption = false;
    for (size_t i = 0; i < parser->numOptions; ++i) {
        if ((longOpt && parser->options[i].longOpt && strcmp(parser->options[i].longOpt, longOpt) == 0) || 
            (shortOpt && parser->options[i].shortOpt == shortOpt)) {
            foundOption = true;

            // Now, verify the dependency exists
            bool foundDependency = false;
            for (size_t j = 0; j < parser->numOptions; ++j) {
                if ((dependsOnLongOpt && parser->options[j].longOpt && strcmp(parser->options[j].longOpt, dependsOnLongOpt) == 0) ||
                    (dependsOnShortOpt && parser->options[j].shortOpt == dependsOnShortOpt)) {
                    foundDependency = true;
                    break;
                }
            }

            if (!foundDependency) {
                snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                         "Error: Dependency option '%s' not found in cli_set_option_dependencies.", dependsOnLongOpt ? dependsOnLongOpt : "N/A");
                cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
                #ifdef CLI_LOGGING_ENABLE
                fmt_fprintf(stderr, "%s\n", cli_last_error.message);
                #endif
                return false;
            }

            // Dependency found, no further action required for now
            // Future implementations may store this relationship for validation during parsing
            break;
        }
    }

    if (!foundOption) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Source option '%s' not found in cli_set_option_dependencies.", longOpt ? longOpt : "N/A");
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
        #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, "%s\n", cli_last_error.message);
        #endif
        return false;
    }

    // At this point, both the option and its dependency exist
    snprintf(cli_last_error.message, sizeof(cli_last_error.message),
             "Success: Option dependency between '%s' and '%s' set successfully.", longOpt ? longOpt : "N/A", dependsOnLongOpt ? dependsOnLongOpt : "N/A");
    cli_last_error.code = CLI_SUCCESS;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stdout, "%s\n", cli_last_error.message);
    #endif
    return true;
}
