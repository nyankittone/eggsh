#pragma once

#include <stdbool.h>
#include <rusttypes.h>

#define COMMAND_NODES_PER_NODE (16)
#define COMMAND_NAME_MAX (32)

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

// We probably want a more sophisticated type used for on_fail than a simple integer.
typedef struct {
    bool is_ok;
    int on_fail;
} ConverterResult;

typedef ConverterResult (*ConverterFunction) (
    int remaining_argc, char **remaining_argv, void *const target
);

typedef struct {
    char *name;
    char *highlighting; // String containing ANSI escape sequence to use for highlighting
    ConverterFunction converter;
} ShellType;

typedef struct {
    char *name;
    ShellType *type;
} OptionParameter;

typedef struct {
    char *short_options;
    char **long_options;

    // TODO: Might place the below fields in its own struct type for more modular design if needed.
    uint id;
    char *description;
    OptionParameter *parameters; // TODO: Make this a fixed array... maybe.
} CommandOption;

typedef struct CommandSchema {
    char name[COMMAND_NAME_MAX]; // This is a char array that is 32 bytes in size.
    uint id; // It's questionable if this data point should exist for the root command.

    char *short_description, *header, *footer;

    // function ptr used for displaying help. This includes displaying the command synopsis,
    // printing the header and footer text, and the list of options.
    void (*displayHelp)(const struct CommandSchema *const command);

    CommandOption *options; // NULL-terminated list of all supported flags for this command
    struct CommandSchema *subcommands; // NULL-terminated list of all subcommands for this command
    // I may change the above two to be fixed arrays rather than pointers for speed purposes...
} CommandSchema;

// Basic iterator type for the default argument parser provided. Could change this to an interface
// at some point if it makes the design better...
typedef struct {
    int remaining_argc;
    char **remaining_argv;
    CommandSchema *const command;
} CommandIterator;

// idk fully what I'm cooking here with this data type...
typedef struct {
    enum {
        COMMAND_ITER_DONE,
        COMMAND_ITER_BAD_FLAG,
        COMMAND_ITER_BAD_SUBCOMMAND,
        COMMAND_ITER_PARAMETER,
    } status;
    union {
        struct {
            uint id;
            ConverterFunction converter; // I believe this should be NULL on a subcommand???
        } on_success;
        struct {
            char *bad_thing;
        } on_fail;
    } data;
} CommandIteration;

CommandIterator newParserIterator(const int argc, const int argv, CommandSchema *const command);
CommandIteration parseArgs(CommandIterator *const iterator);

