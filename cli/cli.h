/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Cli
*/

#ifndef CLI_H_
#define CLI_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif 

#define CLI_LOGGING_ENABLE

#ifdef CLI_LOGGING_ENABLE 
    #define CLI_LOG(fmt, ...) \
        do { fprintf(stderr, "[CLI LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define CLI_LOG(fmt, ...) do { } while (0)
#endif

typedef struct CliParser CliParser;
typedef struct CliOption CliOption;
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
    CLI_ERROR_COMMAND_NOT_FOUND,
} CliStatusCode;

typedef enum {
    CLI_NO_ARG,   // No argument following the option
    CLI_REQUIRED_ARG, // An argument value is required following the option
    CLI_OPTIONAL_ARG  // The argument value is optional following the option
} CliOptionType;

// Function pointer types
typedef void (*CliOptionHandler)(const CliOption *option, const char *value, void *userData);
typedef void (*CliCommandHandler)(const CliCommand *command, int argc, char *argv[], void *userData);
typedef bool (*CliArgumentValidator)(const char *value, void *userData);
typedef void (*CliErrorHandler)(const CliParser *parser, const char *error, void *userData);
typedef bool (*CliOptionValidator)(const char *);
typedef void (*CliPreExecutionHook)(const CliParser *parser, void *userData);
typedef void (*CliPostExecutionHook)(const CliParser *parser, void *userData);


struct CliOption {
    const char *longOpt;        // Long option string, e.g., "--help"
    char shortOpt;              // Short option character, e.g., 'h'
    CliOptionType optionType;   // Type of option (no, required, or optional argument)
    CliOptionHandler handler;   // Handler function to be called when the option is parsed
    CliArgumentValidator validator; // Validator function for argument value
    const char *description;    // Description of the option for help display
    void *userData;             // User data to be passed to the handler and validator
    char *customErrorMessage;
    CliOptionValidator optionValidator; // Function pointer to the validator
    char *validationErrorMessage;
};

typedef struct CliOptionGroup {
    const char *groupName;    // Name of the option group
    CliOption *options;       // Array of options
    size_t numOptions;        // Number of options
} CliOptionGroup;

typedef struct CliCommand {
    const char *name;                      // Name of the command
    const char **aliases;                  // Array of aliases for the command
    size_t numAliases;                     // Number of aliases
    void (*handler)(const struct CliCommand *command, int argc, char *argv[], void *userData);
    const char *description;               // Description of the command
    void *userData;                        // Optional user data
} CliCommand;


typedef struct {
    int code;
    char message[256];
} CliError;

// CLI parser configuration and state
struct CliParser {
    CliOption *options;            // Array of options
    CliCommand *commands;          // Array of commands
    size_t numOptions;             // Number of options
    size_t numCommands;            // Number of commands
    char *progName;                // Program name for usage display
    char *usage;                   // Custom usage message
    CliErrorHandler errorHandler;  // Function to handle errors
    CliError lastError;
    bool strictMode;               // Indicates whether strict mode is enabled
    CliCommandHandler defaultCommandHandler;
    CliOptionGroup *optionGroups;  // Array of option groups
    size_t numOptionGroups;        // Number of option groups
    CliPreExecutionHook preExecutionHook;
    CliPostExecutionHook postExecutionHook;
    void *preExecutionHookUserData; 
    void *userData;
};

CliParser* cli_parser_create(const char *progName);

const CliOption* cli_find_option(const CliParser *parser, const char *longOpt, char shortOpt);
const CliCommand* cli_find_command(const CliParser *parser, const char *name);

void cli_parser_deallocate(CliParser *parser);
void cli_set_error_handler(CliParser *parser, CliErrorHandler handler);
void cli_display_error(const CliParser *parser, const char *error);
void cli_print_help(const CliParser *parser);
void cli_set_custom_usage(CliParser *parser, const char *usage);
void cli_enable_strict_mode(CliParser *parser, bool enable);
void cli_set_default_command_handler(CliParser *parser, CliCommandHandler handler);
void cli_add_option_group(CliParser *parser, const char *groupName, const CliOption *options, size_t numOptions);
void cli_remove_option_group(CliParser *parser, const char *groupName);
void cli_print_version(const CliParser *parser, const char *version);
void cli_update_description(CliParser *parser, const char *name, const char *newDescription, bool isCommand);
void cli_enter_interactive_mode(CliParser *parser, const char *prompt);
void cli_set_option_error_message(CliParser *parser, const char *longOpt, char shortOpt, const char *errorMessage);
void cli_set_pre_execution_hook(CliParser *parser, CliPreExecutionHook hook);
void cli_set_post_execution_hook(CliParser *parser, CliPostExecutionHook hook);

bool cli_set_option_dependencies(CliParser *parser, const char *longOpt, char shortOpt, const char *dependsOnLongOpt, char dependsOnShortOpt);
bool cli_prompt_confirmation(const char *promptMessage);
bool cli_add_option_alias(CliParser *parser, const char *optionName, const char *alias);
bool cli_parse_args_with_delimiter(CliParser *parser, int argc, char *argv[], const char *delimiter);
bool cli_register_command_alias(CliParser *parser, const char *commandName, const char *alias);
bool cli_process_option_group(CliParser *parser, const char *groupName, int argc, char *argv[]);
bool cli_validate_option_argument(const CliOption *option, const char *value);
bool cli_register_command(CliParser *parser, const CliCommand *command);
bool cli_unregister_command(CliParser *parser, const char *name);
bool cli_register_option(CliParser *parser, const CliOption *option);
bool cli_unregister_option(CliParser *parser, const char *longOpt, char shortOpt);

CliStatusCode cli_parse_args(CliParser *parser, int argc, char *argv[]);
CliError cli_get_last_error(const CliParser *parser);

#ifdef __cplusplus 
}
#endif 

#endif 
