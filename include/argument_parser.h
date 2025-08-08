#pragma once

#include <stdbool.h>
#include <rusttypes.h>

#define COMMAND_NODES_PER_NODE (16)

// This is a header outlining an argument parser for this codebase.
// This parser has a few goals:
// * Should be able to handle parsing args for the core program and builtin commands
// * Should be decently fast, especially for processing builtin commands (might be able to use a
//   hash map for quickly doing stuff for builtins)
// * Should be flexible enough to deal with different types of argument parsing formats
//   * Should there be argument parsing for short options only?
// * Should allow for a consistent feeling across most of the builtin commands

// penis
typedef struct {
    bool is_ok;
    int on_fail;
} ConverterResult;

typedef ConverterResult (*ConverterFunction) (
    int remaining_argc, char **remaining_argv, void *const target
);

typedef struct {
    char *name;
    ConverterFunction converter;
} ShellType;

typedef struct {
    char *name;
    ShellType *type;
} OptionParameter;

typedef struct {
    char *short_options; // We may want to not have a field for the short options for some builtins.
    char **long_options;

    // TODO: Might place the below fields in its own struct type for more modular design if needed.
    uint id;
    char *description;
    OptionParameter *parameters; // It won't make sense for all parsers to support multiple
                                 // parameters. What do we do about that?
} CommandOption;

typedef struct CommandTree {
    CommandOption command;
    struct CommandTree *nodes[COMMAND_NODES_PER_NODE];
} CommandTree;

