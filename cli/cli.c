#include "cli.h"
#include "../fmt/fmt.h"
#include "../string/string.h"
#include <stdlib.h>
#include <string.h>

static CliError cli_last_error = {0, ""};

CliParser* cli_parser_create(const char *progName) {
    if (progName == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Program name is NULL in cli_parser_create.\n");
        cli_last_error.code = CLI_ERROR_NONE;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif 
        return NULL;
    }

    // Allocate memory for the CliParser struct
    CliParser *parser = (CliParser *)malloc(sizeof(CliParser));
    if (parser == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed for CliParser in cli_parser_create.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif 
        return NULL;
    }

    // Initialize the CliParser struct
    parser->options = NULL;
    parser->commands = NULL;
    parser->numOptions = 0;
    parser->numCommands = 0;
    parser->progName = string_strdup(progName); 
    if (parser->progName == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed for program name in cli_parser_create.\n");
        #ifdef CLI_LOGGING_ENABLE
            fmt_fprintf(stderr, cli_last_error.message);
        #endif 
        free(parser); 
        return NULL;
    }
    parser->usage = NULL;
    parser->errorHandler = NULL;
    parser->lastError = CLI_SUCCESS;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Parser Created Successfully.\n");

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
                free((void*)parser->options[i].longOpt); // Assuming longOpt is dynamically allocated
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
            // Assume we need to free dynamically allocated command names or other properties
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

    // Finally, free the parser itself
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
            cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND; // Or introduce a new error code for duplicate commands
            
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

    // Indicate successful execution
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
            cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND; // Consider introducing a new error code for duplicate options
            
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
        parser->errorHandler(parser, error, NULL); // Pass NULL or appropriate userData for each handler if needed
    } 
    else {
        fmt_fprintf(stderr, "Error: %s\n", error);
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error displayed: %s", error);
    cli_last_error.code = CLI_ERROR_NONE; // Or a suitable error code indicating the error was handled

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
