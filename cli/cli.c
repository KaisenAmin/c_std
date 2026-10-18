/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Cli
*/

/* POSIX / GNU declarations (strdup, clock_gettime, localtime_r, ...) must be
   requested before the first #include, so that strict -std=c17 builds
   (CMAKE_C_EXTENSIONS=OFF) see them as well. */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "cli.h"
#include "../string/std_string.h"

#if defined(_MSC_VER)
/* MSVC has no <strings.h>; its CRT spells this _stricmp. */
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif


/* Last error of the calling thread (one shared global raced between threads).
   cli_parse_args also stores its result in parser->lastError. */
#if defined(_MSC_VER) && !defined(__clang__)
static __declspec(thread) CliError cli_last_error = {0, ""};
#else
static _Thread_local CliError cli_last_error = {0, ""};
#endif

/* Reentrant strtok: the same splitting, with the position kept in *save
   instead of strtok's hidden global state (which broke a caller's own strtok
   loop and raced between threads on POSIX). */
static char* cli_strtok_r(char* s, const char* delim, char** save) {
    char* p = s ? s : *save;
    if (p == NULL) {
        return NULL;
    }
    p += strspn(p, delim);
    if (*p == '\0') {
        *save = p;
        return NULL;
    }
    char* end = p + strcspn(p, delim);
    if (*end != '\0') {
        *end++ = '\0';
    }
    *save = end;
    return p;
}


/* Does option o answer to longOpt or shortOpt? */
static bool cli_option_is(const CliOption* o, const char* longOpt, char shortOpt) {
    return (longOpt && o->longOpt && strcmp(o->longOpt, longOpt) == 0) ||
           (shortOpt && o->shortOpt == shortOpt);
}

/* Is name already used by a command, either as its name or as an alias? */
static bool cli_command_name_taken(const CliParser* parser, const char* name) {
    for (size_t i = 0; i < parser->numCommands; i++) {
        if (parser->commands[i].name && strcmp(parser->commands[i].name, name) == 0) {
            return true;
        }
        for (size_t j = 0; j < parser->commands[i].numAliases; j++) {
            if (parser->commands[i].aliases[j] && strcmp(parser->commands[i].aliases[j], name) == 0) {
                return true;
            }
        }
    }
    return false;
}

static void cli_free_option_strings(CliOption* o) {
    free((void*)o->longOpt);
    free((void*)o->description);
    free(o->customErrorMessage);
    free(o->validationErrorMessage);
    o->longOpt = NULL;
    o->description = NULL;
    o->customErrorMessage = NULL;
    o->validationErrorMessage = NULL;
}

static void cli_free_command_strings(CliCommand* c) {
    free((void*)c->name);
    free((void*)c->description);
    if (c->aliases) {
        for (size_t j = 0; j < c->numAliases; j++) {
            free((void*)c->aliases[j]);
        }
        free((void*)c->aliases);
    }
    c->name = NULL;
    c->description = NULL;
    c->aliases = NULL;
    c->numAliases = 0;
}

/* Drops every dependency that mentions an option answering to longOpt/shortOpt. */
static void cli_forget_dependencies(CliParser* parser, const char* longOpt, char shortOpt) {
    size_t kept = 0;
    for (size_t i = 0; i < parser->numDependencies; i++) {
        CliOptionDependency* d = &parser->dependencies[i];
        bool hit = (longOpt && ((d->longOpt && strcmp(d->longOpt, longOpt) == 0) ||
                                (d->dependsOnLongOpt && strcmp(d->dependsOnLongOpt, longOpt) == 0))) ||
                   (shortOpt && (d->shortOpt == shortOpt || d->dependsOnShortOpt == shortOpt));
        if (hit) {
            free(d->longOpt);
            free(d->dependsOnLongOpt);
        }
        else {
            parser->dependencies[kept++] = *d;
        }
    }
    parser->numDependencies = kept;
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

    CliParser *parser = (CliParser *)calloc(1, sizeof(CliParser));
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
            // Custom error messages are strdup'd by cli_set_option_error_message /
            // cli_set_option_validation_error; free them too (were leaking).
            if (parser->options[i].customErrorMessage != NULL) {
                free(parser->options[i].customErrorMessage);
            }
            if (parser->options[i].validationErrorMessage != NULL) {
                free(parser->options[i].validationErrorMessage);
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

    // Free the custom usage message set by cli_set_custom_usage (was leaking).
    if (parser->usage != NULL) {
        free(parser->usage);
        parser->usage = NULL;
    }

    // Free option groups added by cli_add_option_group: each group's strdup'd name
    // and its shallow-copied options array (the CliOption strings inside are
    // caller-owned, so not freed here), then the group array itself.
    if (parser->optionGroups != NULL) {
        for (size_t i = 0; i < parser->numOptionGroups; i++) {
            free((void*)parser->optionGroups[i].groupName);
            free(parser->optionGroups[i].options);
            free(parser->optionGroups[i].registered);
        }
        free(parser->optionGroups);
        parser->optionGroups = NULL;
    }

    for (size_t i = 0; i < parser->numDependencies; i++) {
        free(parser->dependencies[i].longOpt);
        free(parser->dependencies[i].dependsOnLongOpt);
    }
    free(parser->dependencies);
    parser->dependencies = NULL;

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


    parser->errorHandler = handler;
    snprintf(cli_last_error.message, sizeof(cli_last_error.message),
             handler ? "Success: Custom error handler set successfully.\n"
                     : "Success: Error handler cleared.\n");
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

// Function to register a command with the CLI parser
bool cli_register_command(CliParser *parser, const CliCommand *command) {
    if (!parser) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: CLI parser is NULL in cli_register_command.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;

        CLI_LOG("[cli_register_command] %s\n", cli_last_error.message);
        return false;
    }

    if (!command || !command->name) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Command or command name is NULL in cli_register_command.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;

        CLI_LOG("[cli_register_command] %s\n", cli_last_error.message);
        return false;
    }

    if (command->handler == NULL) {
        /* the parser calls the handler when the command is given */
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Command '%s' has no handler in cli_register_command.", command->name);
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_register_command] %s\n", cli_last_error.message);
        return false;
    }

    // Check for duplicate command names (an existing name or alias)
    if (cli_command_name_taken(parser, command->name)) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Duplicate command '%s' in cli_register_command.", command->name);
        cli_last_error.code = CLI_ERROR_COMMAND_NOT_FOUND;

        CLI_LOG("[cli_register_command] %s\n", cli_last_error.message);
        return false;
    }
    for (size_t a = 0; command->aliases && a < command->numAliases; a++) {
        const char* al = command->aliases[a];
        bool repeated = false;
        for (size_t b = 0; al && b < a; b++) {
            repeated = repeated || (command->aliases[b] && strcmp(command->aliases[b], al) == 0);
        }
        if (al && (repeated || strcmp(al, command->name) == 0 || cli_command_name_taken(parser, al))) {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Duplicate command alias '%s' in cli_register_command.", al);
            cli_last_error.code = CLI_ERROR_COMMAND_NOT_FOUND;
            CLI_LOG("[cli_register_command] %s\n", cli_last_error.message);
            return false;
        }
    }

    /* Deep copies first, so a failed allocation leaves the parser unchanged. */
    CliCommand copy = *command;
    copy.name = string_strdup(command->name);
    copy.description = command->description ? string_strdup(command->description) : NULL;
    copy.aliases = NULL;
    copy.numAliases = 0;
    bool copied = copy.name != NULL && (command->description == NULL || copy.description != NULL);
    if (copied && command->aliases && command->numAliases > 0) {
        const char **aliases_copy = (const char**)malloc(sizeof(char*) * command->numAliases);
        copied = aliases_copy != NULL;
        if (aliases_copy) {
            copy.aliases = aliases_copy;
            for (size_t a = 0; copied && a < command->numAliases; a++) {
                if (command->aliases[a] == NULL) {
                    continue;   /* NULL entries are skipped (strcmp(NULL) crashed lookups) */
                }
                aliases_copy[copy.numAliases] = string_strdup(command->aliases[a]);
                copied = aliases_copy[copy.numAliases] != NULL;
                if (copied) {
                    copy.numAliases++;
                }
            }
        }
    }
    if (!copied) {
        cli_free_command_strings(&copy);
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed in cli_register_command.");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        CLI_LOG("[cli_register_command] %s\n", cli_last_error.message);
        return false;
    }

    // Allocate memory for the new command array
    CliCommand *newCommands = (CliCommand *)realloc(parser->commands, (parser->numCommands + 1) * sizeof(CliCommand));
    if (!newCommands) {
        cli_free_command_strings(&copy);
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed in cli_register_command.");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;

        CLI_LOG("[cli_register_command] %s\n", cli_last_error.message);
        return false;
    }

    // The deep copies are freed by the deallocator / cli_unregister_command.
    parser->commands = newCommands;
    parser->commands[parser->numCommands] = copy;
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
        size_t n = strlen(parser->usage);
        fprintf(stdout, "%s%s", parser->usage, (n > 0 && parser->usage[n - 1] == '\n') ? "" : "\n");
    }
    else {
        fprintf(stdout, "Usage: %s [options] [commands]\n", parser->progName ? parser->progName : "application");
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

    // Deep-copy the strings because the deallocator will free() them later.
    CliOption copy = *option;
    copy.longOpt = option->longOpt ? string_strdup(option->longOpt) : NULL;
    copy.description = option->description ? string_strdup(option->description) : NULL;
    copy.customErrorMessage = option->customErrorMessage ? string_strdup(option->customErrorMessage) : NULL;
    copy.validationErrorMessage = option->validationErrorMessage ? string_strdup(option->validationErrorMessage) : NULL;
    if ((option->longOpt && !copy.longOpt) || (option->description && !copy.description) ||
        (option->customErrorMessage && !copy.customErrorMessage) ||
        (option->validationErrorMessage && !copy.validationErrorMessage)) {
        cli_free_option_strings(&copy);
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed in cli_register_option.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        CLI_LOG("[cli_register_option] %s", cli_last_error.message);
        return false;
    }
    parser->options[parser->numOptions] = copy;

    CLI_LOG("[cli_register_option] Debug: Option '%s' registered with userData=%p.\n", option->longOpt ? option->longOpt : "(unknown)", option->userData);

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
        fprintf(stderr, "Error: %s\n", error);
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
        if (parser->commands[i].name && strcmp(parser->commands[i].name, name) == 0) {
            return &parser->commands[i];
        }

        // Check aliases
        for (size_t j = 0; j < parser->commands[i].numAliases; j++) {
            if (parser->commands[i].aliases[j] && strcmp(parser->commands[i].aliases[j], name) == 0) {
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

    return NULL; 
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
    } 
    else {
        bool nameIsSingleChar = (name[0] != '\0' && name[1] == '\0');

        for (size_t i = 0; i < parser->numOptions; i++) {
            bool longMatch  = parser->options[i].longOpt && strcmp(parser->options[i].longOpt, name) == 0;
            bool shortMatch = nameIsSingleChar && parser->options[i].shortOpt == name[0];
            if (longMatch || shortMatch) {
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


/* ------------------------------------------------------------------ */
/* Option matching shared by cli_parse_args and option groups         */
/* ------------------------------------------------------------------ */

/* "-5" or "-.5" is a (negative) value, not an option. */
static bool cli_is_negative_number(const char* s) {
    return s[0] == '-' && (isdigit((unsigned char)s[1]) || (s[1] == '.' && isdigit((unsigned char)s[2])));
}

/* An option token ("-x", "--name"); "-" alone and negative numbers are values. */
static bool cli_is_option_token(const char* s) {
    return s && s[0] == '-' && s[1] != '\0' && !cli_is_negative_number(s);
}

/* Option whose longOpt ("--name") equals the first `len` bytes of `token`. */
static CliOption* cli_find_long_option(CliOption* opts, size_t n, const char* token, size_t len) {
    for (size_t k = 0; k < n; ++k) {
        const char* lo = opts[k].longOpt;
        if (lo && strlen(lo) == len && strncmp(lo, token, len) == 0) {
            return &opts[k];
        }
    }
    return NULL;
}

static CliOption* cli_find_short_option(CliOption* opts, size_t n, char c) {
    for (size_t k = 0; k < n; ++k) {
        if (opts[k].shortOpt && opts[k].shortOpt == c) {
            return &opts[k];
        }
    }
    return NULL;
}

/* Value for an option that takes one: an attached value ("--o=v", "-ov"),
 * else, for CLI_REQUIRED_ARG only, the next token if it is not itself an
 * option. A CLI_OPTIONAL_ARG never takes the next token (it used to swallow
 * a following command or option). Returns false when a required value is
 * missing. */
static bool cli_option_value(const CliOption* o, const char* attached, int argc, char* argv[], int* i, const char** value) {
    *value = NULL;
    if (attached) {
        *value = attached;
        return true;
    }
    if (o->optionType == CLI_REQUIRED_ARG) {
        if (*i + 1 < argc && !cli_is_option_token(argv[*i + 1])) {
            *value = argv[++*i];
            return true;
        }
        return false;
    }
    return true;
}

/* Checks the dependencies declared with cli_set_option_dependencies against
   the options given so far (seen[k] for parser->options[k]). On a missing
   option, writes "Option 'a' requires 'b'." into msg and returns false. */
static bool cli_dependencies_met(const CliParser* parser, const bool* seen, char* msg, size_t msgSize) {
    if (seen == NULL) {
        return true;
    }
    for (size_t d = 0; d < parser->numDependencies; d++) {
        const CliOptionDependency* dep = &parser->dependencies[d];
        bool given = false;
        bool needed = false;
        for (size_t k = 0; k < parser->numOptions; k++) {
            if (!seen[k]) {
                continue;
            }
            given  = given  || cli_option_is(&parser->options[k], dep->longOpt, dep->shortOpt);
            needed = needed || cli_option_is(&parser->options[k], dep->dependsOnLongOpt, dep->dependsOnShortOpt);
        }
        if (given && !needed) {
            char a[3] = { '-', dep->shortOpt, '\0' };
            char b[3] = { '-', dep->dependsOnShortOpt, '\0' };
            snprintf(msg, msgSize, "Option '%s' requires '%s'.", dep->longOpt ? dep->longOpt : a,
                     dep->dependsOnLongOpt ? dep->dependsOnLongOpt : b);
            return false;
        }
    }
    return true;
}

/* Records the outcome of a parse for cli_get_last_error and parser->lastError. */
static CliStatusCode cli_parse_status(CliParser* parser, CliStatusCode rc, const char* what, const char* token) {
    cli_last_error.code = rc;
    if (rc == CLI_SUCCESS) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Arguments parsed.");
    }
    else if (what == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "%s", token ? token : "");
    }
    else {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: %s '%s'.", what, token ? token : "");
    }
    parser->lastError = cli_last_error;
    return rc;
}

/* Runs the option's validators on a value it received. */
static bool cli_value_valid(const CliOption* o, const char* value) {
    if (value == NULL) {
        return true;
    }
    if (o->validator && !o->validator(value, o->userData)) {
        return false;
    }
    return !(o->optionValidator && !o->optionValidator(value));
}

/* Handles one option token at argv[*i] (long, short, or bundled shorts) from
 * the given option table. Returns CLI_SUCCESS, CLI_ERROR_PARSE (missing or
 * unexpected value), CLI_ERROR_VALIDATION_FAILED (a validator rejected the
 * value; the handler is not called) or CLI_ERROR_OPTION_NOT_FOUND (unknown
 * option). *failed receives the option a parse / validation error is about;
 * seen (optional, one flag per table entry) marks every option handled. */
static CliStatusCode cli_handle_option(CliOption* opts, size_t n, int argc, char* argv[], int* i,
                                       const char** bad, CliOption** failed, bool* seen) {
    const char* a = argv[*i];
    *bad = a;
    *failed = NULL;

    if (a[1] == '-') {   /* --name or --name=value */
        const char* eq = strchr(a, '=');
        size_t nameLen = eq ? (size_t)(eq - a) : strlen(a);
        CliOption* o = cli_find_long_option(opts, n, a, nameLen);
        if (!o) {
            return CLI_ERROR_OPTION_NOT_FOUND;
        }
        const char* value = NULL;
        *failed = o;
        if (o->optionType == CLI_NO_ARG) {
            if (eq) {
                return CLI_ERROR_PARSE;   /* "--flag=x" for a flag */
            }
        }
        else if (!cli_option_value(o, eq ? eq + 1 : NULL, argc, argv, i, &value)) {
            return CLI_ERROR_PARSE;       /* required value missing */
        }
        if (!cli_value_valid(o, value)) {
            return CLI_ERROR_VALIDATION_FAILED;
        }
        *failed = NULL;
        if (seen) {
            seen[o - opts] = true;
        }
        if (o->handler) {
            o->handler(o, value, o->userData);
        }
        return CLI_SUCCESS;
    }

    /* -a, -abc (bundled flags), -ovalue, -o value */
    for (const char* c = a + 1; *c; ++c) {
        CliOption* o = cli_find_short_option(opts, n, *c);
        if (!o) {
            return CLI_ERROR_OPTION_NOT_FOUND;
        }
        if (o->optionType == CLI_NO_ARG) {
            if (seen) {
                seen[o - opts] = true;
            }
            if (o->handler) {
                o->handler(o, NULL, o->userData);
            }
            continue;
        }
        const char* value = NULL;
        if (!cli_option_value(o, c[1] ? c + 1 : NULL, argc, argv, i, &value)) {
            *failed = o;
            return CLI_ERROR_PARSE;
        }
        if (!cli_value_valid(o, value)) {
            *failed = o;
            return CLI_ERROR_VALIDATION_FAILED;
        }
        if (seen) {
            seen[o - opts] = true;
        }
        if (o->handler) {
            o->handler(o, value, o->userData);
        }
        break;   /* the rest of the token was this option's value */
    }
    return CLI_SUCCESS;
}


/**
 * @brief Parses the command-line arguments.
 *
 * argv[0] is the program name. Options may appear before the command:
 * "--name", "--name=value", "--name value" (CLI_REQUIRED_ARG only, and only
 * when the next token is not an option), "-x", bundled "-abc", "-ovalue" and
 * "-o value". "--" ends option processing. The first non-option token selects
 * a command, whose handler receives the remaining arguments; when no command
 * matches, the default command handler (if set) receives the unmatched token
 * and everything after it. Unknown options are ignored unless strict mode is
 * enabled. An option value is checked with the option's validator /
 * optionValidator before its handler runs; dependencies declared with
 * cli_set_option_dependencies are checked before a command runs (and at the
 * end). Errors go to the error handler, if one is set: the option's
 * validationErrorMessage / customErrorMessage when it has one, otherwise the
 * offending token. The outcome is available from cli_get_last_error.
 *
 * @return CLI_SUCCESS, CLI_ERROR_PARSE (unknown option in strict mode, a
 *         missing / unexpected option value, or a missing required option),
 *         CLI_ERROR_VALIDATION_FAILED, CLI_ERROR_COMMAND_NOT_FOUND,
 *         CLI_ERROR_ALLOCATION_FAILED or CLI_ERROR_INVALID_ARGUMENT.
 */
CliStatusCode cli_parse_args(CliParser *parser, int argc, char *argv[]) {
    if (!parser || argc < 1 || !argv) {
        CLI_LOG("[cli_parse_args] Error: Invalid parser or arguments.\n");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Invalid parser or arguments in cli_parse_args.");
        return CLI_ERROR_INVALID_ARGUMENT;
    }

    CLI_LOG("[cli_parse_args] Debug: Starting argument parsing.\n");

    if (parser->preExecutionHook) {
        parser->preExecutionHook(parser, parser->preExecutionHookUserData);
    }

    CliStatusCode rc = CLI_SUCCESS;
    const char* badToken = NULL;
    const char* what = "";
    char depMessage[200];
    /* which options were given, only needed to check dependencies */
    bool* seen = NULL;
    if (parser->numDependencies > 0 && parser->numOptions > 0) {
        seen = (bool*)calloc(parser->numOptions, sizeof(bool));
        if (seen == NULL) {
            rc = CLI_ERROR_ALLOCATION_FAILED;
            what = "Out of memory while parsing";
            goto done;
        }
    }

    if (argc == 1) {
        /* Kept for callers that pass a bare command as argv[0]. */
        const CliCommand *command = cli_find_command(parser, argv[0]);
        if (command) {
            if (command->handler) {
                command->handler(command, argc, argv, command->userData);
            }
            goto done;
        }
    }

    bool optionsEnded = false;
    for (int i = 1; i < argc; ++i) {
        const char* a = argv[i];
        if (!a) {
            continue;
        }
        if (!optionsEnded && strcmp(a, "--") == 0) {
            optionsEnded = true;
            continue;
        }

        if (!optionsEnded && cli_is_option_token(a)) {
            const char* bad = NULL;
            CliOption* failed = NULL;
            CliStatusCode orc = cli_handle_option(parser->options, parser->numOptions, argc, argv, &i, &bad, &failed, seen);
            if (orc == CLI_ERROR_OPTION_NOT_FOUND) {
                CLI_LOG("[cli_parse_args] Warning: Unrecognized option '%s'.\n", a);
                if (!parser->strictMode) {
                    continue;   /* ignored, as documented, unless strict */
                }
                if (parser->errorHandler) {
                    parser->errorHandler(parser, a, parser->userData);
                }
                rc = CLI_ERROR_PARSE;
                what = "Unrecognized option";
                badToken = a;
                goto done;
            }
            if (orc != CLI_SUCCESS) {
                /* the option's own message, when it has one */
                const char* custom = NULL;
                if (failed) {
                    custom = (orc == CLI_ERROR_VALIDATION_FAILED && failed->validationErrorMessage)
                             ? failed->validationErrorMessage : failed->customErrorMessage;
                }
                if (parser->errorHandler) {
                    parser->errorHandler(parser, custom ? custom : bad, parser->userData);
                }
                rc = orc;
                what = custom ? NULL : (orc == CLI_ERROR_VALIDATION_FAILED ? "Invalid value for option"
                                                                           : "Missing or unexpected value for option");
                badToken = custom ? custom : bad;
                goto done;
            }
            continue;
        }

        if (!cli_dependencies_met(parser, seen, depMessage, sizeof(depMessage))) {
            if (parser->errorHandler) {
                parser->errorHandler(parser, depMessage, parser->userData);
            }
            rc = CLI_ERROR_PARSE;
            what = NULL;
            badToken = depMessage;
            goto done;
        }

        /* First positional token: a command (it gets the rest of argv). */
        const CliCommand *command = cli_find_command(parser, a);
        if (command) {
            CLI_LOG("[cli_parse_args] Debug: Matched command '%s'. Calling handler.\n", a);
            if (command->handler) {
                command->handler(command, argc - i - 1, &argv[i + 1], command->userData);
            }
            goto done;
        }
        if (parser->defaultCommandHandler) {
            /* No command matched: the default handler gets this token and
               everything after it (it was stored but never called). */
            parser->defaultCommandHandler(NULL, argc - i, &argv[i], parser->userData);
            goto done;
        }
        CLI_LOG("[cli_parse_args] Warning: Unrecognized command '%s'.\n", a);
        if (parser->errorHandler) {
            parser->errorHandler(parser, a, parser->userData);
        }
        rc = CLI_ERROR_COMMAND_NOT_FOUND;
        what = "Unrecognized command";
        badToken = a;
        goto done;
    }

    if (!cli_dependencies_met(parser, seen, depMessage, sizeof(depMessage))) {
        if (parser->errorHandler) {
            parser->errorHandler(parser, depMessage, parser->userData);
        }
        rc = CLI_ERROR_PARSE;
        what = NULL;
        badToken = depMessage;
    }

done:
    free(seen);
    if (parser->postExecutionHook) {
        parser->postExecutionHook(parser, parser->preExecutionHookUserData);
    }
    CLI_LOG("[cli_parse_args] Debug: Argument parsing completed with rc=%d.\n", (int)rc);
    return cli_parse_status(parser, rc, what, badToken);
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
            cli_forget_dependencies(parser, parser->options[i].longOpt, parser->options[i].shortOpt);
            /* the custom / validation messages are owned copies too (were leaking) */
            cli_free_option_strings(&parser->options[i]);

            // Option found, shift the rest of the options up by one to overwrite the unregistered option
            size_t remainingOptions = parser->numOptions - i - 1;
            if (remainingOptions > 0) {
                memmove(&parser->options[i], &parser->options[i + 1], remainingOptions * sizeof(CliOption));
            }

            // Decrement the total number of options
            parser->numOptions--;

            /* Use %c for shortOpt: the previous code passed &shortOpt (a pointer to
               a single, non-NUL-terminated char) to %s, so strlen ran past it into
               uninitialised stack. */
            if (longOpt) {
                snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Option '%s' unregistered successfully.", longOpt);
            } else {
                snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Option '%c' unregistered successfully.", shortOpt ? shortOpt : '?');
            }
            cli_last_error.code = CLI_SUCCESS;
            CLI_LOG("[cli_unregister_option] %s", cli_last_error.message);
            return true;
        }
    }

    // Option not found
    if (longOpt) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Option '%s' not found.", longOpt);
    } else {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Option '%c' not found.", shortOpt ? shortOpt : '?');
    }
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
        if (parser->commands[i].name && strcmp(parser->commands[i].name, name) == 0) {
            /* name, description and the deep-copied aliases (were leaking) */
            cli_free_command_strings(&parser->commands[i]);

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
 * @brief Parses command-line arguments, accepting "--key<delimiter>value" option syntax.
 *
 * Every long-option token of the form "--key<delimiter>value" (before a "--"
 * end-of-options marker) is passed to cli_parse_args as "--key=value": the
 * token is split at the FIRST delimiter after the leading "--", so the value
 * may itself contain the delimiter or be empty. argv[0], option values given
 * as separate tokens, commands and their arguments are never split (the old
 * version split every argument, turning "--filter=name=bob" into three).
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
    if (parser == NULL || argv == NULL || delimiter == NULL || delimiter[0] == '\0' || argc < 1) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Invalid arguments provided to cli_parse_args_with_delimiter.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;

        CLI_LOG("[cli_parse_args_with_delimiter] %s", cli_last_error.message);
        return false;
    }

    char** newArgv = (char**)malloc(sizeof(char*) * (size_t)argc);
    bool*  owned   = (bool*)calloc((size_t)argc, sizeof(bool));   /* owned[i]: newArgv[i] was allocated here */
    bool   ok      = newArgv != NULL && owned != NULL;
    size_t dlen    = strlen(delimiter);
    bool   optionsEnded = false;

    for (int i = 0; ok && i < argc; ++i) {
        char* a = argv[i];
        newArgv[i] = a;
        if (i == 0 || a == NULL || optionsEnded) {
            continue;
        }
        if (strcmp(a, "--") == 0) {
            optionsEnded = true;
            continue;
        }
        if (a[0] != '-' || a[1] != '-' || a[2] == '\0') {
            continue;   /* not a long option */
        }
        const char* d  = strstr(a + 2, delimiter);
        const char* eq = strchr(a, '=');
        if (d == NULL || d == a + 2 || (eq && eq < d)) {
            continue;   /* no "--key<delim>", or already "--key=..." */
        }
        size_t keyLen = (size_t)(d - a);
        const char* value = d + dlen;
        char* joined = (char*)malloc(keyLen + 1 + strlen(value) + 1);
        if (joined == NULL) {
            ok = false;
            break;
        }
        memcpy(joined, a, keyLen);
        joined[keyLen] = '=';
        strcpy(joined + keyLen + 1, value);
        newArgv[i] = joined;
        owned[i] = true;
    }

    CliStatusCode status = CLI_ERROR_ALLOCATION_FAILED;
    if (ok) {
        status = cli_parse_args(parser, argc, newArgv);
    }
    else {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed in cli_parse_args_with_delimiter.");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
    }

    for (int i = 0; owned && newArgv && i < argc; ++i) {
        if (owned[i]) {
            free(newArgv[i]);
        }
    }
    free(newArgv);
    free(owned);
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

    // Track the group itself so cli_remove_option_group can find it later.
    CliOptionGroup *newGroups = (CliOptionGroup*)realloc(parser->optionGroups, sizeof(CliOptionGroup) * (parser->numOptionGroups + 1));
    if (!newGroups) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),"Memory allocation failed in cli_add_option_group.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;

        CLI_LOG("[cli_add_option_group] %s", cli_last_error.message);
        return;
    }
    parser->optionGroups = newGroups;

    CliOptionGroup g;
    g.groupName  = string_strdup(groupName);
    g.numOptions = numOptions;
    g.options    = (CliOption*)malloc(sizeof(CliOption) * numOptions);
    g.registered = (unsigned char*)calloc(numOptions, 1);
    if (!g.groupName || !g.options || !g.registered) {
        free((void*)g.groupName);
        free(g.options);
        free(g.registered);
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),"Memory allocation failed in cli_add_option_group.\n");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        CLI_LOG("[cli_add_option_group] %s", cli_last_error.message);
        return;
    }
    // Shallow copy: caller owns the option's strings
    memcpy(g.options, options, sizeof(CliOption) * numOptions);

    // Register each option directly to the parser (a duplicate is skipped and
    // stays owned by whoever registered it first)
    for (size_t i = 0; i < numOptions; i++) {
        if (cli_register_option(parser, &options[i])) {
            g.registered[i] = 1;
        }
        else {
            CLI_LOG("[cli_add_option_group] Error: Failed to register option '%s'.\n", options[i].longOpt ? options[i].longOpt : "(unknown)");
        }
    }
    parser->optionGroups[parser->numOptionGroups] = g;
    parser->numOptionGroups++;

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
            /* The group's options were registered with the parser as well: they
               used to stay matched by cli_parse_args and listed by help. */
            CliOptionGroup* grp = &parser->optionGroups[i];
            for (size_t k = 0; k < grp->numOptions; k++) {
                if (grp->registered && grp->registered[k]) {
                    cli_unregister_option(parser, grp->options[k].longOpt, grp->options[k].longOpt ? '\0' : grp->options[k].shortOpt);
                }
            }
            free((void *)grp->groupName);
            free(grp->options);
            free(grp->registered);

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

        inputBuffer[strcspn(inputBuffer, "\r")] = '\0';
        if (strcmp(inputBuffer, "exit") == 0 || strcmp(inputBuffer, "quit") == 0) {
            printf("Exiting interactive mode.\n");
            break;
        }

        /* argv[0] is the program name, as cli_parse_args expects: the typed
           words used to start at argv[0], so "calculate 20 20" dispatched
           "20" as the command. */
        char *argv[65];
        int argc = 0;
        argv[argc++] = parser->progName ? parser->progName : (char*)"cli";
        char *save = NULL;
        char *token = cli_strtok_r(inputBuffer, " \t", &save);

        while (token && argc < 65) {
            argv[argc++] = token;
            token = cli_strtok_r(NULL, " \t", &save);
        }

        if (argc > 1) {
            CLI_LOG("[cli_enter_interactive_mode] Parsed command: '%s' with %d args.\n", argv[1], argc - 2);
            CliStatusCode status = cli_parse_args(parser, argc, argv);
            if (status != CLI_SUCCESS) {
                fprintf(stderr, "Error: Command '%s' failed with status %d\n", argv[1], status);
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
            parser->options[i].customErrorMessage = string_strdup(errorMessage);
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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Option is NULL in.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_validate_option_argument] %s", cli_last_error.message);
        return false;
    }

    if (value == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message),  "Error: Value is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_validate_option_argument] %s", cli_last_error.message);
        return false;
    }

    if (option->validator && !option->validator(value, option->userData)) {
        if (option->validationErrorMessage) {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "%s", option->validationErrorMessage);
        } 
        else {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Validation failed for the given option argument.");
        }
        cli_last_error.code = CLI_ERROR_VALIDATION_FAILED;
        CLI_LOG("[cli_validate_option_argument] %s", cli_last_error.message);

        return false;
    }
    if (option->optionValidator && !option->optionValidator(value)) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: optionValidator rejected value.");
        cli_last_error.code = CLI_ERROR_VALIDATION_FAILED;
        CLI_LOG("[cli_validate_option_argument] %s", cli_last_error.message);

        return false;
    }
    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Validation Passed.\n");
    cli_last_error.code = CLI_SUCCESS;

    return true;
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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Parser is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        
        CLI_LOG("[cli_set_pre_execution_hook] %s", cli_last_error.message);
        return;
    }

    // It's valid for hook to be NULL, as the user might want to clear a previously set hook.
    parser->preExecutionHook = hook;


    snprintf(cli_last_error.message, sizeof(cli_last_error.message),  hook ? "Pre-execution hook set successfully." : "Pre-execution hook cleared.");
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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Parser is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;

        CLI_LOG("[cli_set_post_execution_hook] %s", cli_last_error.message);
        return;
    }

    // It's valid for hook to be NULL to clear a previously set hook
    parser->postExecutionHook = hook;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), hook ? "Post-execution hook set successfully." : "Post-execution hook cleared.");
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

    if (cli_command_name_taken(parser, alias)) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: '%s' is already a command name or alias.", alias);
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_register_command_alias] %s\n", cli_last_error.message);
        return false;
    }

    // Find the original command
    for (size_t i = 0; i < parser->numCommands; i++) {
        if (parser->commands[i].name && strcmp(parser->commands[i].name, commandName) == 0) {
            char* copy = string_strdup(alias);
            // Reallocate memory for the new alias
            const char **newAliases = copy ? (const char**)realloc((void*)parser->commands[i].aliases, sizeof(char *) * (parser->commands[i].numAliases + 1)) : NULL;
            if (!newAliases) {
                free(copy);
                snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed in cli_register_command_alias.");
                cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
                CLI_LOG("[cli_register_command_alias] Error: Memory allocation failed.\n");
                return false;
            }

            parser->commands[i].aliases = newAliases;
            parser->commands[i].aliases[parser->commands[i].numAliases] = copy;
            parser->commands[i].numAliases++;
            cli_last_error.code = CLI_SUCCESS;
            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Alias '%s' registered.", alias);

            CLI_LOG("[cli_register_command_alias] Alias '%s' registered for command '%s'.\n", alias, commandName);
            return true;
        }
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Command '%s' not found.", commandName);
    cli_last_error.code = CLI_ERROR_COMMAND_NOT_FOUND;
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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: promptMessage is NULL .");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;
        CLI_LOG("[cli_prompt_confirmation] %s", cli_last_error.message);

        return false;
    }

    /* The prompt is printed (it went only to CLI_LOG, which is compiled
       out), whole lines are read, and invalid answers loop instead of
       recursing once per 3 bytes of input (long input overflowed the stack).
       y/yes and n/no are accepted in any letter case; end of input is "no". */
    for (;;) {
        printf("%s (y/n): ", promptMessage);
        fflush(stdout);

        char response[64];
        if (fgets(response, sizeof(response), stdin) == NULL) {
            return false;
        }
        size_t len = strcspn(response, "\r\n");
        bool longLine = (response[len] == '\0' && len == sizeof(response) - 1);
        response[len] = '\0';
        if (longLine) {
            int ch;
            while ((ch = getchar()) != EOF && ch != '\n') {
                /* discard the rest of an over-long line */
            }
            if (ch == EOF) {
                return false;
            }
        }

        char* answer = response;
        while (*answer == ' ' || *answer == '\t') {
            answer++;
        }
        if (strcasecmp(answer, "y") == 0 || strcasecmp(answer, "yes") == 0) {
            return true;
        }
        if (strcasecmp(answer, "n") == 0 || strcasecmp(answer, "no") == 0) {
            return false;
        }
        printf("Invalid input. Please enter 'y' or 'n'.\n");
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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Invalid arguments provided .");
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
        /* Same token rules as cli_parse_args ("-l", "--level", "--level=x",
           "-lx"), matching only this option. The short form used to be
           compared with argv[j][0] (the '-'), so "-l" never matched while a
           value such as "logfile.txt" did; a NULL longOpt crashed strcmp;
           and handlers got parser->userData instead of option->userData. */
        for (int j = 1; j < argc; ++j) { // Skip the program name
            const char* a = argv[j];
            if (!cli_is_option_token(a)) {
                continue;
            }
            bool isLong = (a[1] == '-');
            if (isLong) {
                const char* eq = strchr(a, '=');
                size_t nameLen = eq ? (size_t)(eq - a) : strlen(a);
                if (!option->longOpt || strlen(option->longOpt) != nameLen || strncmp(option->longOpt, a, nameLen) != 0) {
                    continue;
                }
            }
            else if (!option->shortOpt || a[1] != option->shortOpt) {
                continue;
            }
            const char* bad = NULL;
            CliOption* failed = NULL;
            int k = j;
            CliStatusCode orc = cli_handle_option(option, 1, argc, argv, &k, &bad, &failed, NULL);
            if (orc == CLI_ERROR_VALIDATION_FAILED) {
                snprintf(cli_last_error.message, sizeof(cli_last_error.message), "%s",
                         option->validationErrorMessage ? option->validationErrorMessage :
                         option->customErrorMessage ? option->customErrorMessage : "Error: Invalid value for option in group.");
                cli_last_error.code = CLI_ERROR_VALIDATION_FAILED;
                return false;
            }
            if (orc != CLI_SUCCESS) {
                if (option->customErrorMessage) {
                    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "%s", option->customErrorMessage);
                }
                else {
                    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Missing or unexpected value for option '%s' in group '%s'.", a, groupName);
                }
                cli_last_error.code = CLI_ERROR_PARSE;
                return false;
            }
            optionFound = true;
            break;
        }
        if (!optionFound && option->optionType == CLI_REQUIRED_ARG) {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Required option '%s' not found in group '%s'.", option->longOpt ? option->longOpt : "(short option)", groupName);
            cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
            CLI_LOG("[cli_process_option_group] %s", cli_last_error.message);
            return false;
        }
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Option group '%s' processed successfully.", groupName);
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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Parser is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;

        CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
        return false;
    }

    if (!optionName || !alias) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: optionName or alias is NULL in cli_add_option_alias.");
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
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Original option '%s' not found.", optionName);
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;

        CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
        return false;
    }

    // Ensure the alias does not already exist
    for (size_t i = 0; i < parser->numOptions; i++) {
        if (parser->options[i].longOpt && strcmp(parser->options[i].longOpt, alias) == 0) {
            snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Alias '%s' already exists.", alias);
            cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;

            CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
            return false;
        }
    }

    // Snapshot the original option's value BEFORE the realloc below: realloc()
    // may move parser->options, leaving originalOption (a pointer into the old,
    // freed block) dangling. Copy it by value while it is still valid.
    CliOption originalCopy = *originalOption;

    // Expand the options array to accommodate the new alias option
    CliOption *newOptions = (CliOption*)realloc(parser->options, sizeof(CliOption) * (parser->numOptions + 1));
    if (!newOptions) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed.");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;

        CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
        return false;
    }
    parser->options = newOptions;

    CliOption aliasOption = originalCopy;
    aliasOption.longOpt = string_strdup(alias);
    if (aliasOption.longOpt == NULL) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: strdup failed for alias name.");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;

        CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
        return false;
    }
    /* Own copies of the strings: sharing the original's pointers would
       double-free them in cli_parser_deallocate. The alias keeps the same
       description and error messages (and validators) as the original. */
    aliasOption.description = originalCopy.description ? string_strdup(originalCopy.description) : NULL;
    aliasOption.customErrorMessage = originalCopy.customErrorMessage ? string_strdup(originalCopy.customErrorMessage) : NULL;
    aliasOption.validationErrorMessage = originalCopy.validationErrorMessage ? string_strdup(originalCopy.validationErrorMessage) : NULL;
    if ((originalCopy.description && !aliasOption.description) ||
        (originalCopy.customErrorMessage && !aliasOption.customErrorMessage) ||
        (originalCopy.validationErrorMessage && !aliasOption.validationErrorMessage)) {
        cli_free_option_strings(&aliasOption);
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed.");
        cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
        CLI_LOG("[cli_add_option_alias] %s", cli_last_error.message);
        return false;
    }

    parser->options[parser->numOptions] = aliasOption;
    parser->numOptions++;

    snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Success: Option alias '%s' added for '%s'.", alias, optionName);
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
 * @note The dependency is stored and enforced by cli_parse_args: when the first
 *       option is given without the second, parsing fails with CLI_ERROR_PARSE
 *       ("Option 'a' requires 'b'.") before any command runs. Declaring the same
 *       pair twice is a no-op; unregistering either option drops it.
 *
 * @note false if the `parser` is NULL.
 * @note false if both `longOpt` and `shortOpt` are NULL or empty, or if both
 *         `dependsOnLongOpt` and `dependsOnShortOpt` are NULL or empty.
 * @note false if the specified option or its dependency is not found in the parser.
 * @note true if the option dependency was successfully set.
 */
bool cli_set_option_dependencies(CliParser *parser, const char *longOpt, char shortOpt, const char *dependsOnLongOpt, char dependsOnShortOpt) {
    if (!parser) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Parser is NULL.");
        cli_last_error.code = CLI_ERROR_INVALID_ARGUMENT;

        CLI_LOG("[cli_set_option_dependencies] %s", cli_last_error.message);
        return false;
    }

    if ((!longOpt && shortOpt == '\0') || (!dependsOnLongOpt && dependsOnShortOpt == '\0')) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Both source and dependency options cannot be NULL or empty.");
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
                snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Dependency option '%s' not found.", dependsOnLongOpt ? dependsOnLongOpt : "N/A");
                cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;
                CLI_LOG("[cli_set_option_dependencies] %s", cli_last_error.message);

                return false;
            }

            /* Store the pair under the registered options' own names. */
            const CliOption* src = &parser->options[i];
            const CliOption* dst = NULL;
            for (size_t j = 0; j < parser->numOptions && !dst; ++j) {
                if (cli_option_is(&parser->options[j], dependsOnLongOpt, dependsOnShortOpt)) {
                    dst = &parser->options[j];
                }
            }
            for (size_t d = 0; d < parser->numDependencies; ++d) {
                const CliOptionDependency* old = &parser->dependencies[d];
                if (cli_option_is(src, old->longOpt, old->shortOpt) && cli_option_is(dst, old->dependsOnLongOpt, old->dependsOnShortOpt)) {
                    cli_last_error.code = CLI_SUCCESS;
                    return true;   /* already declared */
                }
            }
            CliOptionDependency dep;
            dep.longOpt = src->longOpt ? string_strdup(src->longOpt) : NULL;
            dep.shortOpt = src->shortOpt;
            dep.dependsOnLongOpt = dst->longOpt ? string_strdup(dst->longOpt) : NULL;
            dep.dependsOnShortOpt = dst->shortOpt;
            CliOptionDependency* grown = NULL;
            if ((!src->longOpt || dep.longOpt) && (!dst->longOpt || dep.dependsOnLongOpt)) {
                grown = (CliOptionDependency*)realloc(parser->dependencies, sizeof(CliOptionDependency) * (parser->numDependencies + 1));
            }
            if (grown == NULL) {
                free(dep.longOpt);
                free(dep.dependsOnLongOpt);
                snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Memory allocation failed in cli_set_option_dependencies.");
                cli_last_error.code = CLI_ERROR_ALLOCATION_FAILED;
                return false;
            }
            parser->dependencies = grown;
            parser->dependencies[parser->numDependencies++] = dep;
            break;
        }
    }

    if (!foundOption) {
        snprintf(cli_last_error.message, sizeof(cli_last_error.message), "Error: Source option '%s' not found .", longOpt ? longOpt : "N/A");
        cli_last_error.code = CLI_ERROR_OPTION_NOT_FOUND;

        CLI_LOG("[cli_set_option_dependencies] %s", cli_last_error.message);
        return false;
    }

    snprintf(cli_last_error.message, sizeof(cli_last_error.message),
             "Success: Option dependency between '%s' and '%s' set successfully.", longOpt ? longOpt : "N/A", dependsOnLongOpt ? dependsOnLongOpt : "N/A");
             
    cli_last_error.code = CLI_SUCCESS;
    CLI_LOG("[cli_set_option_dependencies] %s", cli_last_error.message);

    return true;
}


/* =================================================================== */
/* Typed argument conversion                                           */
/*                                                                     */
/* Option handlers and validators receive the raw `const char* value`  */
/* from the command line and must convert it themselves. These         */
/* stateless helpers do that conversion once, correctly — rejecting    */
/* empty strings, trailing garbage, and out-of-range values instead of */
/* silently truncating the way bare atoi() does.                       */
/* =================================================================== */

/* Case-insensitive whole-string equality. */
static bool clip_ieq(const char* a, const char* b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
        ++a;
        ++b;
    }
    return *a == *b;
}

/**
 * @brief Parse a string into an int, with full validation.
 *
 * Accepts an optional leading sign and decimal digits (leading whitespace is
 * tolerated). Rejects an empty string, any trailing non-numeric characters
 * (e.g. "42abc", "1.5"), and values outside the `int` range. On success the
 * value is written to @p out; on failure @p out is left untouched.
 *
 * @param value Null-terminated string to parse. May be NULL (returns false).
 * @param out   Receives the parsed value on success. Must not be NULL.
 * @return true if @p value is a valid int (written to @p out); false otherwise.
 */
bool cli_parse_int(const char* value, int* out) {
    if (!value || !out || value[0] == '\0') {
        return false;
    }
    errno = 0;
    char* end = NULL;
    long v = strtol(value, &end, 10);
    if (end == value || *end != '\0') {
        CLI_LOG("[cli_parse_int] '%s' is not a valid integer.", value);
        return false;
    }
    if (errno == ERANGE || v < INT_MIN || v > INT_MAX) {
        CLI_LOG("[cli_parse_int] '%s' is out of int range.", value);
        return false;
    }
    *out = (int)v;
    return true;
}

/**
 * @brief Parse a string into a long, with full validation.
 *
 * Same rules as `cli_parse_int`, but for the wider `long` range — useful for
 * sizes, counts, or ids that may exceed `int`.
 *
 * @param value Null-terminated string to parse. May be NULL (returns false).
 * @param out   Receives the parsed value on success. Must not be NULL.
 * @return true if @p value is a valid long (written to @p out); false otherwise.
 */
bool cli_parse_long(const char* value, long* out) {
    if (!value || !out || value[0] == '\0') {
        return false;
    }
    errno = 0;
    char* end = NULL;
    long v = strtol(value, &end, 10);
    if (end == value || *end != '\0') {
        CLI_LOG("[cli_parse_long] '%s' is not a valid integer.", value);
        return false;
    }
    if (errno == ERANGE) {
        CLI_LOG("[cli_parse_long] '%s' is out of long range.", value);
        return false;
    }
    *out = v;
    return true;
}

/**
 * @brief Parse a string into a double, with full validation.
 *
 * Accepts decimal and scientific notation (e.g. "3.14", "-2.5", "1e9").
 * Rejects an empty string, trailing garbage (e.g. "1.2.3"), and values outside
 * the representable `double` range. On success the value is written to @p out.
 *
 * @param value Null-terminated string to parse. May be NULL (returns false).
 * @param out   Receives the parsed value on success. Must not be NULL.
 * @return true if @p value is a valid double (written to @p out); false otherwise.
 */
bool cli_parse_double(const char* value, double* out) {
    if (!value || !out || value[0] == '\0') {
        return false;
    }
    errno = 0;
    char* end = NULL;
    double v = strtod(value, &end);
    if (end == value || *end != '\0') {
        CLI_LOG("[cli_parse_double] '%s' is not a valid number.", value);
        return false;
    }
    if (errno == ERANGE) {
        CLI_LOG("[cli_parse_double] '%s' is out of double range.", value);
        return false;
    }
    *out = v;
    return true;
}

/**
 * @brief Parse a string into a bool, accepting the common spellings.
 *
 * Recognises (case-insensitively) `true`/`false`, `yes`/`no`, `on`/`off`, and
 * `1`/`0`. Any other string is rejected (returns false) so a typo doesn't
 * silently become `false`.
 *
 * @param value Null-terminated string to parse. May be NULL (returns false).
 * @param out   Receives the parsed value on success. Must not be NULL.
 * @return true if @p value is a recognised boolean (written to @p out); false otherwise.
 */
bool cli_parse_bool(const char* value, bool* out) {
    if (!value || !out) {
        return false;
    }
    if (clip_ieq(value, "true") || clip_ieq(value, "yes") || clip_ieq(value, "on") || clip_ieq(value, "1")) {
        *out = true;
        return true;
    }
    if (clip_ieq(value, "false") || clip_ieq(value, "no") || clip_ieq(value, "off") || clip_ieq(value, "0")) {
        *out = false;
        return true;
    }
    CLI_LOG("[cli_parse_bool] '%s' is not a recognised boolean.", value);
    return false;
}
