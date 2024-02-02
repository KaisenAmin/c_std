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

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "CliParser deallocated successfully.\n");
    cli_last_error.code = CLI_SUCCESS;
    #ifdef CLI_LOGGING_ENABLE
        fmt_fprintf(stderr, cli_last_error.message);
    #endif
}
