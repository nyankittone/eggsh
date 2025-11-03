#pragma once

#include <stdbool.h>
#include <rusttypes.h>
#include <util.h>

// How many subcommands are allowed per command?
#define COMMAND_NODES_PER_NODE (16)

// Macro for the maximum possible length for a command or subcommand in bytes, including the null
// byte.
#define COMMAND_NAME_MAX (32) 

// Maximum number of parameters allowed per command option, plus one, to account for a null value.
#define COMMAND_OPTION_PARAMETER_LIMIT (5)

// How many options are allowed per command, accounting for a null value at the end?
#define COMMAND_OPTION_COUNT_MAX (32)

// This is a header outlining an argument parser for this codebase.
// This parser has a few goals:
// * Should be able to handle parsing args for the core program and builtin commands
// * Should be decently fast, especially for processing builtin commands (might be able to use a
//   hash map for quickly doing stuff for builtins)
// * Should be flexible enough to deal with different types of argument parsing formats
//   * Should there be argument parsing for short options only?
// * Should allow for a consistent feeling across most of the builtin commands
// * Should have good support for subcommands. A command like `help` should be able to scan through
//   a struct of a command schema and pull out subcommands to read help from.

// We probably want a more sophisticated type used for on_fail than a simple integer. Purrhaps a
// special "compound error" type is in the works too? A good argument parser really could use good
// error handling.
typedef struct {
    bool is_ok;
    int on_fail; // TODO: Replace this with an actual error type!
} ConverterResult;

typedef ConverterResult (*ConverterFunction) (
    char *const arg, void *const output
);

typedef struct {
    ConverterFunction converter;
    char *convertee;
} ConverterWithInput;

typedef struct {
    char *name, *short_name;
    char *highlighting; // String containing ANSI escape sequence to use for highlighting
    ConverterFunction converter; // I may want to add more functions like this in the future if
                                 // needed.
} ShellType;

typedef struct {
    char *name;
    ShellType *type;
} OptionParameter;

#define NULL_OPTION_PARAMETER ((OptionParameter) {0})

typedef struct {
    char *short_options;
    char *long_options[8]; // NULL-terminated list of long options. TODO: Experiment with making this
                         // into a hash map.

    // TODO: Might place the below fields in its own struct type for more modular design if needed.
    int id;
    char *description;
    OptionParameter parameters[COMMAND_OPTION_PARAMETER_LIMIT]; // 4 parameters, 1 NULL terminator
} CommandOption;

#define NULL_OPTION ((CommandOption) {0})

typedef struct CommandSchema {
    char name[COMMAND_NAME_MAX]; // This is a char array that is 32 bytes in size.
    int id; // It's questionable if this data point should exist for the root command.

    char *short_description, *header, *footer;

    // function ptr used for displaying help. This includes displaying the command synopsis,
    // printing the header and footer text, and the list of options.
    void (*displayHelp)(const struct CommandSchema *const command, char **command_line);

    // TODO: Add limits on how many options and subcommands there can be by making these pointers
    // into arrays. That might be faster.

    CommandOption options[COMMAND_OPTION_COUNT_MAX]; // List of all supported flags for this command

    u8f subcommand_count; // The subcommand array is the only one in the command schema that is not
                          // null-value terminated.
    struct CommandSchema *subcommands; // list of all subcommands for this command
} CommandSchema;

// Basic iterator type for the default argument parser provided. Could change this to an interface
// at some point if it makes the design better...
typedef struct {
    int remaining_argc;
    char **remaining_argv;
    CommandSchema *const command;
    const char *current_short_option; // Used to track the short option we're on for a parameter
                                      // between iterations
    int positional_argc;
    char **positional_argv;
} CommandIterator;

// idk fully what I'm cooking here with this data type...
typedef struct {
    enum {
        COMMAND_ITER_NONE, // TODO: Remove this enumeration in the future. It's pretty much useless.
        COMMAND_ITER_DONE,
        COMMAND_ITER_PARAMETER,
    } status;
    int id;
    ConverterWithInput converters[COMMAND_OPTION_PARAMETER_LIMIT]; // the first index should be NULL on a subcommand
} CommandIteration;

// Macro the expands out to a `CommandIteration` struct indicating we're done going through options.
#define FULL_CMD_ITER_DONE ((CommandIteration) {.status = COMMAND_ITER_DONE})

#define mCmdIterSubcommand(the_id) ((CommandIteration) {.status = COMMAND_ITER_PARAMETER, .id = (the_id)})

// I don't like how this macro exists in my code lmao
#define FULL_CMD_ITER_NONE ((CommandIteration) {.status = COMMAND_ITER_NONE})
#define NO_OPTION_ID ((int) -1)

CommandIterator newParserIterator(const int argc, char **argv, CommandSchema *const command, char **positional_args);
CommandIteration parseArgs(CommandIterator *const iterator);

