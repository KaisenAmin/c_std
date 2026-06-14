/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Cli
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in cli.c (file-level convention).
 *
 * Object-oriented command-line argument parser — options, sub-commands,
 * aliases, option groups, validation, pre/post hooks, interactive mode.
 */

#ifndef CLI_H_
#define CLI_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define CLI_LOGGING_ENABLE */

#ifdef CLI_LOGGING_ENABLE
    #define CLI_LOG(fmt, ...) \
        do { fprintf(stderr, "[CLI LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define CLI_LOG(...) do { } while (0)
#endif



typedef struct CliParser  CliParser;
typedef struct CliOption  CliOption;
typedef struct CliCommand CliCommand;

typedef enum {
    CLI_SUCCESS,
    CLI_ERROR_PARSE,
    CLI_ERROR_OPTION_NOT_FOUND,
    CLI_ERROR_INVALID_ARGUMENT,
    CLI_ERROR_NONE,
    CLI_ERROR_ALLOCATION_FAILED,
    CLI_DISABLE_STRICT_MODE,
    CLI_ERROR_VALIDATION_FAILED,
    CLI_ERROR_COMMAND_NOT_FOUND
} CliStatusCode;


typedef enum {
    CLI_NO_ARG,         /* no argument follows the option                   */
    CLI_REQUIRED_ARG,   /* an argument value is required                    */
    CLI_OPTIONAL_ARG    /* argument value is optional                       */
} CliOptionType;



typedef void  (*CliOptionHandler)    (const CliOption* option, const char* value, void* userData);
typedef void  (*CliCommandHandler)   (const CliCommand* command, int argc, char* argv[], void* userData);
typedef void  (*CliPreExecutionHook) (const CliParser* parser, void* userData);
typedef void  (*CliPostExecutionHook)(const CliParser* parser, void* userData);
typedef void  (*CliErrorHandler)     (const CliParser* parser, const char* error, void* userData);
typedef bool  (*CliArgumentValidator)(const char* value, void* userData);
typedef bool  (*CliOptionValidator)  (const char*);


/* ------------------------------------------------------------------ */
/* Public types — option / command / group / error / parser           */
/* ------------------------------------------------------------------ */

struct CliOption {
    const char*           longOpt;
    char                  shortOpt;
    CliOptionType         optionType;
    CliOptionHandler      handler;
    CliArgumentValidator  validator;
    const char*           description;
    void*                 userData;
    char*                 customErrorMessage;
    CliOptionValidator    optionValidator;
    char*                 validationErrorMessage;
};


typedef struct CliOptionGroup {
    const char*  groupName;
    CliOption*   options;
    size_t       numOptions;
} CliOptionGroup;


struct CliCommand {
    const char*  name;
    const char** aliases;
    size_t       numAliases;
    void (*handler)(const struct CliCommand* command, int argc, char* argv[], void* userData);
    const char*  description;
    void*        userData;
};


typedef struct {
    int  code;
    char message[256];
} CliError;


struct CliParser {
    CliOption*            options;
    CliCommand*           commands;
    size_t                numOptions;
    size_t                numCommands;
    char*                 progName;
    char*                 usage;
    CliErrorHandler       errorHandler;
    CliError              lastError;
    bool                  strictMode;
    CliCommandHandler     defaultCommandHandler;
    CliOptionGroup*       optionGroups;
    size_t                numOptionGroups;
    CliPreExecutionHook   preExecutionHook;
    CliPostExecutionHook  postExecutionHook;
    void*                 preExecutionHookUserData;
    void*                 userData;
};


/* ------------------------------------------------------------------ */
/* Construction / destruction                                         */
/* ------------------------------------------------------------------ */

CliParser*        cli_parser_create                  (const char* progName);
void              cli_parser_deallocate              (CliParser* parser);


/* ------------------------------------------------------------------ */
/* Option registration                                                */
/* ------------------------------------------------------------------ */

bool              cli_register_option                (CliParser* parser, const CliOption* option);
bool              cli_unregister_option              (CliParser* parser, const char* longOpt, char shortOpt);
bool              cli_add_option_alias               (CliParser* parser, const char* optionName, const char* alias);
bool              cli_set_option_dependencies        (CliParser* parser, const char* longOpt, char shortOpt, const char* dependsOnLongOpt, char dependsOnShortOpt);
void              cli_set_option_error_message       (CliParser* parser, const char* longOpt, char shortOpt, const char* errorMessage);


/* ------------------------------------------------------------------ */
/* Option groups                                                      */
/* ------------------------------------------------------------------ */

void              cli_add_option_group               (CliParser* parser, const char* groupName, const CliOption* options, size_t numOptions);
void              cli_remove_option_group            (CliParser* parser, const char* groupName);
bool              cli_process_option_group           (CliParser* parser, const char* groupName, int argc, char* argv[]);


/* ------------------------------------------------------------------ */
/* Command registration                                               */
/* ------------------------------------------------------------------ */

bool              cli_register_command               (CliParser* parser, const CliCommand* command);
bool              cli_unregister_command             (CliParser* parser, const char* name);
bool              cli_register_command_alias         (CliParser* parser, const char* commandName, const char* alias);
void              cli_set_default_command_handler    (CliParser* parser, CliCommandHandler handler);


/* ------------------------------------------------------------------ */
/* Parsing / dispatch                                                 */
/* ------------------------------------------------------------------ */

CliStatusCode     cli_parse_args                     (CliParser* parser, int argc, char* argv[]);
bool              cli_parse_args_with_delimiter      (CliParser* parser, int argc, char* argv[], const char* delimiter);
bool              cli_validate_option_argument       (const CliOption* option, const char* value);


/* ------------------------------------------------------------------ */
/* Typed argument conversion (stateless string -> value helpers)      */
/* ------------------------------------------------------------------ */

bool              cli_parse_int                      (const char* value, int* out);
bool              cli_parse_long                     (const char* value, long* out);
bool              cli_parse_double                   (const char* value, double* out);
bool              cli_parse_bool                     (const char* value, bool* out);


/* ------------------------------------------------------------------ */
/* Lookup                                                             */
/* ------------------------------------------------------------------ */

const CliOption*  cli_find_option                    (const CliParser* parser, const char* longOpt, char shortOpt);
const CliCommand* cli_find_command                   (const CliParser* parser, const char* name);


/* ------------------------------------------------------------------ */
/* Hooks / configuration                                              */
/* ------------------------------------------------------------------ */

void              cli_set_error_handler              (CliParser* parser, CliErrorHandler handler);
void              cli_set_pre_execution_hook         (CliParser* parser, CliPreExecutionHook hook);
void              cli_set_post_execution_hook        (CliParser* parser, CliPostExecutionHook hook);
void              cli_set_custom_usage               (CliParser* parser, const char* usage);
void              cli_enable_strict_mode             (CliParser* parser, bool enable);
void              cli_update_description             (CliParser* parser, const char* name, const char* newDescription, bool isCommand);


/* ------------------------------------------------------------------ */
/* Output / interactive                                               */
/* ------------------------------------------------------------------ */

void              cli_print_help                     (const CliParser* parser);
void              cli_print_version                  (const CliParser* parser, const char* version);
void              cli_display_error                  (const CliParser* parser, const char* error);
void              cli_enter_interactive_mode         (CliParser* parser, const char* prompt);
bool              cli_prompt_confirmation            (const char* promptMessage);


/* ------------------------------------------------------------------ */
/* Diagnostics                                                        */
/* ------------------------------------------------------------------ */

CliError          cli_get_last_error                 (const CliParser* parser);


#ifdef __cplusplus
}
#endif

#endif 
