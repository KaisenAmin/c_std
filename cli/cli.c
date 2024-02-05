#include "cli.h"
#include "../fmt/fmt.h"
#include "../string/string.h"
#include <stdlib.h>
#include <string.h>

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
