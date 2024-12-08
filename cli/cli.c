/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Cli
*/

#include <stdlib.h>
#include <string.h>
#include "cli.h"
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
        tokenArray = (char**)realloc(tokenArray, sizeof(char*) * (tokens + 1));
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
        CLI_LOG("[cli_parser_create] %s", cli_last_error.message);
        return NULL;
    }

    CliParser *parser = (CliParser *)malloc(sizeof(CliParser));
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed for CliParser in cli_parser_create.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        CLI_LOG("[cli_parser_create] %s", cli_last_error.message);
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
        CLI_LOG("[cli_parser_create] %s", cli_last_error.message);
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
    parser->userData = NULL;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Parser Created Successfully.\n");
    cli_last_error.code = CLI_SUCCESS;
    CLI_LOG("[cli_parser_create] %s", cli_last_error.message);

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

        CLI_LOG("[cli_parser_deallocate] %s", cli_last_error.message);
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

    // Deallocate commands and their aliases if they exist
    if (parser->commands != NULL) {
        for (size_t i = 0; i < parser->numCommands; i++) {
            if (parser->commands[i].name != NULL) {
                free((void*)parser->commands[i].name);
            }
            if (parser->commands[i].description != NULL) {
                free((void*)parser->commands[i].description);
            }

            // Free each alias in the command's aliases array
            if (parser->commands[i].aliases != NULL) {
                for (size_t j = 0; j < parser->commands[i].numAliases; j++) {
                    if (parser->commands[i].aliases[j] != NULL) {
                        free((void*)parser->commands[i].aliases[j]);
                    }
                }
                free(parser->commands[i].aliases); // Free the aliases array itself
                parser->commands[i].aliases = NULL;
            }
        }
        free(parser->commands);
        parser->commands = NULL;
    }

    // Finally, free the parser structure itself
    free(parser);

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: CliParser deallocated successfully.\n");
    cli_last_error.code = CLI_SUCCESS;
    CLI_LOG("[cli_parser_deallocate] %s", cli_last_error.message);
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
        CLI_LOG("[cli_set_custom_usage] %s", cli_last_error.message);
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
        CLI_LOG("[cli_set_custom_usage] %s", cli_last_error.message);
    } 
    else {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Custom usage message set successfully.\n");
        cli_last_error.code = CLI_SUCCESS;
        CLI_LOG("[cli_set_custom_usage] %s", cli_last_error.message);
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
        CLI_LOG("[cli_enable_strict_mode] %s", cli_last_error.message);
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

    CLI_LOG("[cli_enable_strict_mode] %s", cli_last_error.message);
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
        CLI_LOG("[cli_set_error_handler] %s", cli_last_error.message);
        return;
    }

    if (handler == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Cannot set error handler on a NULL errorHandler function.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_set_error_handler] %s", cli_last_error.message);
        return;
    }

    // Assign the provided handler to the parser
    parser->errorHandler = handler;
    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Custome Error handler set successfully.\n");
    cli_last_error.code = CLI_SUCCESS;

    CLI_LOG("[cli_set_error_handler] %s", cli_last_error.message);
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Function to register a command with the CLI parser
bool cli_register_command(CliParser *parser, const CliCommand *command) {
    if (!parser) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: CLI parser is NULL in cli_register_command.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_register_command] %s\n", cli_last_error.message);
        return false;
    }

    if (!command || !command->name) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Command or command name is NULL in cli_register_command.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_register_command] %s\n", cli_last_error.message);
        return false;
    }

    // Check for duplicate command names
    for (size_t i = 0; i < parser->numCommands; ++i) {
        if (strcmp(parser->commands[i].name, command->name) == 0) {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                     "Error: Duplicate command '%s' in cli_register_command.", command->name);
            cli_last_error.code = CLI_ERROR_COMMAND_NOT_FOUND;
            CLI_LOG("[cli_register_command] %s\n", cli_last_error.message);
            return false;
        }
    }

    // Allocate memory for the new command array
    CliCommand *newCommands = (CliCommand *)realloc(parser->commands,
                                                    (parser->numCommands + 1) * sizeof(CliCommand));
    if (!newCommands) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Memory allocation failed in cli_register_command.");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        CLI_LOG("[cli_register_command] %s\n", cli_last_error.message);
        return false;
    }

    // Add the new command to the array
    parser->commands = newCommands;
    parser->commands[parser->numCommands] = *command;
    parser->numCommands++;

    CLI_LOG("[cli_register_command] Debug: Command '%s' registered successfully.\n", command->name);
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
        
        CLI_LOG("[cli_print_help] %s", cli_last_error.message);
        return;
    }

    if (parser->usage) {
        CLI_LOG("[cli_print_help] %s\n", parser->usage);
    } 
    else {
        CLI_LOG("[cli_print_help] Usage: %s [options] [commands]\n", parser->progName ? parser->progName : "application");
    }

    // Print options
    if (parser->options && parser->numOptions > 0) {
        fprintf(stdout, "Options:\n");
        for (size_t i = 0; i < parser->numOptions; i++) {
            fprintf(stdout, "  %s, %c\t%s\n",
                        parser->options[i].longOpt ? parser->options[i].longOpt : "",
                        parser->options[i].shortOpt ? parser->options[i].shortOpt : ' ',
                        parser->options[i].description ? parser->options[i].description : "");
        }
    }

    // Print commands
    if (parser->commands && parser->numCommands > 0) {
        fprintf(stdout, "Commands:\n");
        for (size_t i = 0; i < parser->numCommands; i++) {
            fprintf(stdout, "  %s\t%s\n",
                        parser->commands[i].name ? parser->commands[i].name : "Unnamed",
                        parser->commands[i].description ? parser->commands[i].description : "No description available");
        }
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Usage information printed successfully.\n");
    cli_last_error.code = CLI_SUCCESS;

    CLI_LOG("[cli_print_help] %s", cli_last_error.message);
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
    if (!parser) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Parser is NULL in cli_register_option.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_register_option] %s", cli_last_error.message);
        return false;
    }
    if (!option) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Option is NULL in cli_register_option.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_register_option] %s", cli_last_error.message);
        return false;
    }

    // Check for duplicate options
    for (size_t i = 0; i < parser->numOptions; i++) {
        if ((option->longOpt && parser->options[i].longOpt && strcmp(parser->options[i].longOpt, option->longOpt) == 0) ||
            (option->shortOpt && parser->options[i].shortOpt == option->shortOpt)) {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Duplicate option '%s'.\n", option->longOpt ? option->longOpt : "");
            cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
            CLI_LOG("[cli_register_option] %s", cli_last_error.message);
            return false;
        }
    }

    // Resize the options array
    CliOption *temp = (CliOption *)realloc(parser->options, (parser->numOptions + 1) * sizeof(CliOption));
    if (!temp) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed in cli_register_option.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        CLI_LOG("[cli_register_option] %s", cli_last_error.message);
        return false;
    }
    parser->options = temp;

    // Copy the option and its userData
    parser->options[parser->numOptions] = *option;

    CLI_LOG("[cli_register_option] Debug: Option '%s' registered with userData=%p.\n",
            option->longOpt ? option->longOpt : "(unknown)", option->userData);

    parser->numOptions++;
    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Option '%s' registered successfully.\n", option->longOpt ? option->longOpt : "");
    cli_last_error.code = CLI_SUCCESS;
    CLI_LOG("[cli_register_option] %s", cli_last_error.message);
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
        
        CLI_LOG("[cli_display_error] %s", cli_last_error.message);
        return;
    }

    if (error == NULL || error[0] == '\0') {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: NULL or empty error message provided to cli_display_error.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        
        CLI_LOG("[cli_display_error] %s", cli_last_error.message);
        return;
    }

    if (parser->errorHandler) {
        // Assuming error handlers are designed to work without parser's userData
        parser->errorHandler(parser, error, NULL); 
    } 
    else {
        CLI_LOG("[cli_display_error] Error: %s\n", error);
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error displayed: %s", error);
    cli_last_error.code = CLI_ERROR_NONE; 
    CLI_LOG("[cli_display_error] %s", cli_last_error.message);
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
        CLI_LOG("[cli_display_error] %s", cli_last_error.message);
        return;
    }

    fprintf(stdout, "%s version %s\n", parser->progName ? parser->progName : "Application", version);

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Version information printed successfully.\n");
    cli_last_error.code = CLI_SUCCESS;
    CLI_LOG("[cli_display_error] %s", cli_last_error.message);
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
    if (!parser || !name) {
        CLI_LOG("[cli_find_command] Error: Invalid parser or command name.\n");
        return NULL;
    }

    for (size_t i = 0; i < parser->numCommands; i++) {
        if (strcmp(parser->commands[i].name, name) == 0) {
            return &parser->commands[i];
        }

        // Check aliases
        for (size_t j = 0; j < parser->commands[i].numAliases; j++) {
            if (strcmp(parser->commands[i].aliases[j], name) == 0) {
                return &parser->commands[i];
            }
        }
    }

    CLI_LOG("[cli_find_command] Warning: Command '%s' not found.\n", name);
    return NULL;
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
        CLI_LOG("[cli_display_error] %s", cli_last_error.message);
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
        CLI_LOG("[cli_update_description] %s", cli_last_error.message);
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
        CLI_LOG("[cli_update_description] %s", cli_last_error.message);
        return;
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Description updated successfully for '%s'.\n", name);
    cli_last_error.code = CLI_SUCCESS;
    CLI_LOG("[cli_update_description] %s", cli_last_error.message);
}

/**
 * @brief Parses command-line arguments and executes the corresponding command or options.
 *
 * @param parser A pointer to the `CliParser` instance. Must not be NULL.
 * @param argc The argument count from `main`.
 * @param argv The argument vector from `main`.
 *
 * @return `CliStatusCode` indicating the result of parsing:
 *         - `CLI_SUCCESS` on success.
 *         - `CLI_ERROR_INVALID_ARGUMENT` if parser or args are invalid.
 *         - `CLI_ERROR_PARSE` for unrecognized options.
 *         - `CLI_ERROR_COMMAND_NOT_FOUND` for unrecognized commands.
 */
CliStatusCode cli_parse_args(CliParser *parser, int argc, char *argv[]) {
    if (!parser || argc < 1 || !argv) {
        CLI_LOG("[cli_parse_args] Error: Invalid parser or arguments.\n");
        return CLI_ERROR_INVALID_ARGUMENT;
    }

    CLI_LOG("[cli_parse_args] Debug: Starting argument parsing.\n");

    if(argc == 1) {
        CLI_LOG("[cli_parse_args] Debug: in argc\n");
        const CliCommand *command = cli_find_command(parser, argv[0]);
        if (command) {
            command->handler(command, argc, argv, command->userData);
            return CLI_SUCCESS;
        }
    }

    // First, process all options (arguments starting with '-' or '--')
    for (int i = 1; i < argc; ++i) {
        
     
        if (argv[i][0] == '-') {
            bool optionProcessed = false;

            // Loop through registered options
            for (size_t j = 0; j < parser->numOptions; ++j) {
                CliOption *option = &parser->options[j];

                // Match long option
                if (option->longOpt && strcmp(argv[i], option->longOpt) == 0) {
                    CLI_LOG("[cli_parse_args] Debug: Matched long option '%s'.\n", argv[i]);
                    const char *value = NULL;
                    if (option->optionType != CLI_NO_ARG && i + 1 < argc) {
                        value = argv[++i];
                    }
                    if (option->handler) {
                        option->handler(option, value, option->userData);
                    }
                    optionProcessed = true;
                    break;
                }

                // Match short option
                if (option->shortOpt && argv[i][1] == option->shortOpt && argv[i][2] == '\0') {
                    CLI_LOG("[cli_parse_args] Debug: Matched short option '-%c'.\n", option->shortOpt);
                    const char *value = NULL;
                    if (option->optionType != CLI_NO_ARG && i + 1 < argc) {
                        value = argv[++i];
                    }
                    if (option->handler) {
                        option->handler(option, value, option->userData);
                    }
                    optionProcessed = true;
                    break;
                }
            }

            if (!optionProcessed) {
                CLI_LOG("[cli_parse_args] Warning: Unrecognized option '%s'.\n", argv[i]);
                if (parser->errorHandler) {
                    parser->errorHandler(parser, argv[i], parser->userData);
                }
                return CLI_ERROR_PARSE;
            }
        }
    }

    // Then, process commands (non-option arguments)
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') { // Skip options
            const char *commandName = argv[i];
            const CliCommand *command = cli_find_command(parser, commandName);
            if (command) {
                CLI_LOG("[cli_parse_args] Debug: Matched command '%s'. Calling handler.\n", commandName);
                command->handler(command, argc - i - 1, &argv[i + 1], command->userData);
                return CLI_SUCCESS;
            } else {
                CLI_LOG("[cli_parse_args] Warning: Unrecognized command '%s'.\n", commandName);
                if (parser->errorHandler) {
                    parser->errorHandler(parser, commandName, parser->userData);
                }
                return CLI_ERROR_COMMAND_NOT_FOUND;
            }
        }
    }

    CLI_LOG("[cli_parse_args] Debug: No commands executed. Argument parsing completed successfully.\n");
    return CLI_ERROR_COMMAND_NOT_FOUND;
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
        CLI_LOG("[cli_get_last_error] %s", cli_last_error.message);
        return cli_last_error;
    }
    CLI_LOG("[cli_get_last_error] Success: Parser arg is not null in cli_get_last_error.\n");
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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: NULL parser provided to ");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_set_default_command_handler] %s", cli_last_error.message);
      
        return;
    }

    if (handler == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Warning: NULL handler provided to Default handler cleared.");
        cli_last_error.code = CLI_SUCCESS; 
        CLI_LOG("[cli_set_default_command_handler] %s", cli_last_error.message);
    } 
    else {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Default command handler set successfully.");
        cli_last_error.code = CLI_SUCCESS;
        CLI_LOG("[cli_set_default_command_handler] %s", cli_last_error.message);
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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Parser is NULL in .");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_unregister_option] %s", cli_last_error.message);
        return false;
    }

    if (longOpt == NULL && shortOpt == '\0') {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Both long and short option identifiers are NULL or empty in .");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_unregister_option] %s", cli_last_error.message);
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
            CLI_LOG("[cli_unregister_option] %s", cli_last_error.message);
            return true;
        }
    }

    // Option not found
    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Option '%s' not found in.", longOpt ? longOpt : shortOpt ? &shortOpt : "N/A");
    cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
    CLI_LOG("[cli_unregister_option] %s", cli_last_error.message);

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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Parser is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_unregister_command] %s", cli_last_error.message);
        return false;
    }

    if (name == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Command name is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_unregister_command] %s", cli_last_error.message);
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
            CLI_LOG("[cli_unregister_command] %s", cli_last_error.message);
            return true;
        }
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Command '%s' not found.", name);
    cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
    CLI_LOG("[cli_unregister_command] %s", cli_last_error.message);

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
        CLI_LOG("[cli_parse_args_with_delimiter] %s", cli_last_error.message);
        return false;
    }

    int totalArgs = 0;
    char** newArgv = NULL; 

    for (int i = 0; i < argc; ++i) {
        int splitCount = 0;
        char** splitArgs = split_string(argv[i], delimiter, &splitCount);

        if (splitCount > 1) {
            // Argument contains the delimiter and was split
            newArgv = (char**)realloc(newArgv, sizeof(char*) * (totalArgs + splitCount));
            for (int j = 0; j < splitCount; ++j) {
                newArgv[totalArgs++] = splitArgs[j];
            }
            free(splitArgs); // only free the array, not the strings,
        }
         else {
            // Argument does not contain the delimiter, or splitting failed
            newArgv = (char**)realloc(newArgv, sizeof(char*) * (totalArgs + 1));
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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Invalid arguments provided.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_add_option_group] %s", cli_last_error.message);
        return;
    }

    // Register each option directly to the parser
    for (size_t i = 0; i < numOptions; i++) {
        if (!cli_register_option(parser, &options[i])) {
            CLI_LOG("[cli_add_option_group] Error: Failed to register option '%s'.\n",
                    options[i].longOpt ? options[i].longOpt : "(unknown)");
        }
    }

    CLI_LOG("[cli_add_option_group] Group '%s' with %zu options registered successfully.\n", groupName, numOptions);
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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Invalid arguments provided: parser or groupName is NULL.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_remove_option_group] %s", cli_last_error.message);
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
                CliOptionGroup *temp = (CliOptionGroup*)realloc(parser->optionGroups, sizeof(CliOptionGroup) * parser->numOptionGroups);
                if (temp == NULL) {
                    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Memory reallocation failed.\n");
                    cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
                    CLI_LOG("[cli_remove_option_group] %s", cli_last_error.message);
                    return;
                }
                parser->optionGroups = temp;
            }

            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Option group '%s' removed successfully.\n", groupName);
            cli_last_error.code = CLI_SUCCESS;
            CLI_LOG("[cli_remove_option_group] %s", cli_last_error.message);
            return;
        }
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Option group '%s' not found in cli_remove_option_group.\n", groupName);
    cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
    CLI_LOG("[cli_remove_option_group] %s", cli_last_error.message);
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
    if (!parser || !prompt) {
        CLI_LOG("[cli_enter_interactive_mode] Error: Invalid parser or prompt.\n");
        return;
    }

    char inputBuffer[1024];
    printf("%s", prompt);

    while (fgets(inputBuffer, sizeof(inputBuffer), stdin)) {
        inputBuffer[strcspn(inputBuffer, "\n")] = '\0'; // Remove newline character

        if (strcmp(inputBuffer, "exit") == 0) {
            printf("Exiting interactive mode.\n");
            break;
        }

        char *argv[64];
        int argc = 0;
        char *token = strtok(inputBuffer, " ");
        while (token && argc < 64) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }

        if (argc > 0) {
            CLI_LOG("[cli_enter_interactive_mode] Parsed command: '%s' with %d args.\n", argv[0], argc - 1);
            CliStatusCode status = cli_parse_args(parser, argc, argv);
            if (status != CLI_SUCCESS) {
                fprintf(stderr, "Error: Command '%s' failed with status %d\n", argv[0], status);
            }
        }

        printf("%s", prompt);
    }
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
        CLI_LOG("[cli_set_option_error_message] %s", cli_last_error.message);
        return;
    }

    if (!longOpt && shortOpt == '\0') {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Both longOpt and shortOpt cannot be empty.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_set_option_error_message] %s", cli_last_error.message);
        return;
    }

    if (!errorMessage) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error message cannot be NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_set_option_error_message] %s", cli_last_error.message);
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
                CLI_LOG("[cli_set_option_error_message] %s", cli_last_error.message);
                return;
            }

            found = true;
            break;
        }
    }

    if (!found) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Option not found.");
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
        CLI_LOG("[cli_set_option_error_message] %s", cli_last_error.message);;
    } 
    else {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Custom error message set successfully.");
        cli_last_error.code = CLI_SUCCESS;
        CLI_LOG("[cli_set_option_error_message] %s", cli_last_error.message);
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
                 "Error: Option is NULL in.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_validate_option_argument] %s", cli_last_error.message);
        return false;
    }

    if (value == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), 
                 "Error: Value is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_validate_option_argument] %s", cli_last_error.message);
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
            CLI_LOG("[cli_validate_option_argument] %s", cli_last_error.message);
            return false;
        }
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Validation Passed.\n");
        cli_last_error.code = CLI_SUCCESS;
        return true; // Validation passed
    } 
    else {
        // No validator provided, assume the value is valid
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Validation Passed.\n");
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
                 "Error: Parser is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_set_pre_execution_hook] %s", cli_last_error.message);
        return;
    }

    // It's valid for hook to be NULL, as the user might want to clear a previously set hook.
    parser->preExecutionHook = hook;

    // Optionally, you might want to allow setting user data for the hook function.
    // If you add a parameter for user data, you would set it here.

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), 
             hook ? "Pre-execution hook set successfully." : "Pre-execution hook cleared.");
    cli_last_error.code = CLI_SUCCESS;
    CLI_LOG("[cli_set_pre_execution_hook] %s", cli_last_error.message);
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
                 "Error: Parser is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_set_post_execution_hook] %s", cli_last_error.message);
        return;
    }

    // It's valid for hook to be NULL to clear a previously set hook
    parser->postExecutionHook = hook;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message),
             hook ? "Post-execution hook set successfully." : "Post-execution hook cleared.");
    cli_last_error.code = CLI_SUCCESS;
    CLI_LOG("[cli_set_post_execution_hook] %s", cli_last_error.message);
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
    if (!parser || !commandName || !alias) {
        CLI_LOG("[cli_register_command_alias] Error: Invalid arguments.\n");
        return false;
    }

    // Find the original command
    for (size_t i = 0; i < parser->numCommands; i++) {
        if (strcmp(parser->commands[i].name, commandName) == 0) {
            // Reallocate memory for the new alias
            const char **newAliases = (const char**)realloc(parser->commands[i].aliases, 
                sizeof(char *) * (parser->commands[i].numAliases + 1));
            if (!newAliases) {
                CLI_LOG("[cli_register_command_alias] Error: Memory allocation failed.\n");
                return false;
            }

            parser->commands[i].aliases = newAliases;
            parser->commands[i].aliases[parser->commands[i].numAliases] = strdup(alias);
            parser->commands[i].numAliases++;
            CLI_LOG("[cli_register_command_alias] Alias '%s' registered for command '%s'.\n", alias, commandName);
            return true;
        }
    }

    CLI_LOG("[cli_register_command_alias] Error: Command '%s' not found.\n", commandName);
    return false;
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
                 "Error: promptMessage is NULL .");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_prompt_confirmation] %s", cli_last_error.message);

        return false;
    }

    CLI_LOG("%s (y/n): ", promptMessage);
    char response[4]; // Buffer for user input; large enough for "y\n" or "n\n" and null terminator
    if (fgets(response, sizeof(response), stdin) == NULL) {
        CLI_LOG("Error reading input.\n");
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
        CLI_LOG("Invalid input. Please enter 'y' or 'n'.\n");
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
                 "Invalid arguments provided .");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_process_option_group] %s", cli_last_error.message);
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
        CLI_LOG("[cli_process_option_group] %s", cli_last_error.message);
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
            CLI_LOG("[cli_process_option_group] %s", cli_last_error.message);
            return false;
        }
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message),
             "Option group '%s' processed successfully.", groupName);
    cli_last_error.code = CLI_SUCCESS;
    CLI_LOG("[cli_process_option_group] %s", cli_last_error.message);
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
                 "Error: Parser is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
        return false;
    }

    if (!optionName || !alias) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: optionName or alias is NULL in cli_add_option_alias.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
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
                 "Error: Original option '%s' not found.", optionName);
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
        CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
        return false;
    }

    // Ensure the alias does not already exist
    for (size_t i = 0; i < parser->numOptions; i++) {
        if (parser->options[i].longOpt && strcmp(parser->options[i].longOpt, alias) == 0) {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                     "Error: Alias '%s' already exists.", alias);
            cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
            CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
            return false;
        }
    }

    // Expand the options array to accommodate the new alias option
    CliOption *newOptions = (CliOption*)realloc(parser->options, sizeof(CliOption) * (parser->numOptions + 1));
    if (!newOptions) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Memory allocation failed.");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
        return false;
    }
    parser->options = newOptions;

    // Create and add the alias option
    CliOption aliasOption = *originalOption; // Copy the original option
    aliasOption.longOpt = string_strdup(alias); // Duplicate the alias name
    if (!aliasOption.longOpt) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: strdup failed for alias name.");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
        return false;
    }

    parser->options[parser->numOptions] = aliasOption;
    parser->numOptions++;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message),
             "Success: Option alias '%s' added for '%s'.", alias, optionName);
    cli_last_error.code = CLI_SUCCESS;
    CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);

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
                 "Error: Parser is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_set_option_dependencies] %s", cli_last_error.message);
        return false;
    }

    if ((!longOpt && shortOpt == '\0') || (!dependsOnLongOpt && dependsOnShortOpt == '\0')) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Both source and dependency options cannot be NULL or empty.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_set_option_dependencies] %s", cli_last_error.message);
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
                         "Error: Dependency option '%s' not found.", dependsOnLongOpt ? dependsOnLongOpt : "N/A");
                cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
                CLI_LOG("[cli_set_option_dependencies] %s", cli_last_error.message);
                return false;
            }

            // Dependency found, no further action required for now
            // Future implementations may store this relationship for validation during parsing
            break;
        }
    }

    if (!foundOption) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),
                 "Error: Source option '%s' not found .", longOpt ? longOpt : "N/A");
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
        CLI_LOG("[cli_set_option_dependencies] %s", cli_last_error.message);
        return false;
    }

    // At this point, both the option and its dependency exist
    snprintf(cli_last_error.message, sizeof(cli_last_error.message),
             "Success: Option dependency between '%s' and '%s' set successfully.", longOpt ? longOpt : "N/A", dependsOnLongOpt ? dependsOnLongOpt : "N/A");
    cli_last_error.code = CLI_SUCCESS;
    CLI_LOG("[cli_set_option_dependencies] %s", cli_last_error.message);
    return true;
}
