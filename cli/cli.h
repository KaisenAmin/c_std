#ifndef CLI_H_
#define CLI_H_

#include <stdbool.h>
#include <stddef.h>

#define CLI_LOGGING_ENABLE

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

struct CliCommand {
    const char *name;           // Command name
    CliCommandHandler handler;  // Handler function to be called when the command is parsed
    const char *description;    // Description of the command for help display
    void *userData;             // User data to be passed to the handler
};

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
    bool pipeliningEnabled;
    void *userData;
};

// Initializes a new CLI parser with the specified program name for usage messages.
CliParser* cli_parser_create(const char *progName);

// Finds and returns a pointer to an option by its long name or short character.
const CliOption* cli_find_option(const CliParser *parser, const char *longOpt, char shortOpt);

// Searches for and returns a command by its name.
const CliCommand* cli_find_command(const CliParser *parser, const char *name);

// Frees all resources allocated by the CLI parser.
void cli_parser_deallocate(CliParser *parser);

// Sets a custom function to handle parsing errors.
void cli_set_error_handler(CliParser *parser, CliErrorHandler handler);

// Displays an error message through the configured error handler.
void cli_display_error(const CliParser *parser, const char *error);

// Prints a detailed help message, including descriptions of all options and commands.
void cli_print_help(const CliParser *parser);

// Allows setting a custom usage message, overriding the default.
void cli_set_custom_usage(CliParser *parser, const char *usage);

// Enables or disables strict mode, where unrecognized options trigger errors.
void cli_enable_strict_mode(CliParser *parser, bool enable);

// Sets a default command handler to be used when no specific command is matched.
void cli_set_default_command_handler(CliParser *parser, CliCommandHandler handler);

// Groups related options together under a named group for better organization.
void cli_add_option_group(CliParser *parser, const char *groupName, const CliOption *options, size_t numOptions);

// Removes a previously defined option group.
void cli_remove_option_group(CliParser *parser, const char *groupName);

// Prints the version information of the application.
void cli_print_version(const CliParser *parser, const char *version);

// Updates the description of an option or command.
void cli_update_description(CliParser *parser, const char *name, const char *newDescription, bool isCommand);

// Enters an interactive mode where commands can be input repeatedly.
void cli_enter_interactive_mode(CliParser *parser, const char *prompt);

// Sets a custom error message for a specific option's validation failure.
void cli_set_option_error_message(CliParser *parser, const char *longOpt, char shortOpt, const char *errorMessage);

// Registers a function to be called before any command execution.
void cli_set_pre_execution_hook(CliParser *parser, CliPreExecutionHook hook);

// Registers a function to be called after any command execution.
void cli_set_post_execution_hook(CliParser *parser, CliPostExecutionHook hook);

// allowing the output of one command to be used as input for another, enhancing scripting capabilities.
void cli_enable_pipelining(CliParser *parser, bool enable);

// Defines dependencies between options, where the presence of one option requires the presence of another.
bool cli_set_option_dependencies(CliParser *parser, const char *longOpt, char shortOpt, const char *dependsOnLongOpt, char dependsOnShortOpt);

// Prompts the user for confirmation with a customizable message and waits for a Y/N response.
bool cli_prompt_confirmation(const char *promptMessage);

// allowing users to use alternative names for commands and options for ease of use.
bool cli_add_option_alias(CliParser *parser, const char *optionName, const char *alias);

// Parses command line arguments using a specified delimiter.
bool cli_parse_args_with_delimiter(CliParser *parser, int argc, char *argv[], const char *delimiter);

// Registers an alias for a command.
bool cli_register_command_alias(CliParser *parser, const char *commandName, const char *alias);

// Parses command line arguments for a specific option group.
bool cli_process_option_group(CliParser *parser, const char *groupName, int argc, char *argv[]);

// Validates an option's argument using the option's custom validator function.
bool cli_validate_option_argument(const CliOption *option, const char *value);

// Registers a new command, along with its handler, for execution.
bool cli_register_command(CliParser *parser, const CliCommand *command);

// Removes a previously registered command from the parser.
bool cli_unregister_command(CliParser *parser, const char *name);

// Registers a new option with the parser for command line argument parsing.
bool cli_register_option(CliParser *parser, const CliOption *option);

// Unregisters an option from the parser using its long or short identifier.
bool cli_unregister_option(CliParser *parser, const char *longOpt, char shortOpt);

// Parses command line arguments according to the configured options and commands.
CliStatusCode cli_parse_args(CliParser *parser, int argc, char *argv[]);

// get last error of CliParser
CliError cli_get_last_error(const CliParser *parser);

#endif // CLI_H_
