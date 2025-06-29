#pragma once

#include <util.h>
#include <rusttypes.h>
#include <vector_info.h>

#define INITIAL_TOKENS_CAPACITY (4096)
#define INITIAL_TREE_CAPACITY (32)
#define INITIAL_CMDLINE_CAPACITY (64)

typedef struct {
    size_t bytes_written, capacity;
    char *ptr;
} TokenInfo;

typedef struct {
    size_t capacity;
    char **ptr;
} CommandLineSpace;

// TODO: Add a tree data structure in here when it's time for getting an AST working.
typedef struct {
    void *arena_ptr;

    // TODO: Expand the auxiliary array here to also include the syntax tree once one is needed.
    TokenInfo tokens;
    CommandLineSpace command_line;

    u32 total_tokens; // TODO: move this to be in a struct within an entry of the tree

    char *lagged_remaining;
    char *remaining;
    size_t remaining_length;

    bool scanning_word;
} CommandBuilder;

CommandBuilder newCommandBuilder(void);
CommandBuilder *nukeCommandBuilder(CommandBuilder *const builder);
CommandBuilder *setParserInput(CommandBuilder *builder, char *string, size_t length);
CommandBuilder *newCommand(CommandBuilder *const builder);

typedef u8 TokenizeCommandReturn;
#define PARSE_COMMAND_NORMAL (0)
#define PARSE_COMMAND_OUT_OF_DATA (1)
#define PARSE_COMMAND_HIT_NEWLINE (2)

TokenizeCommandReturn tokenizeBuilderInput(CommandBuilder *const builder);

typedef struct {
    bool program_exited;
    u8f exit_code;
} ExitStatus;

#define NO_EXIT_STATUS (ExitStatus){.program_exited = false, .exit_code = 0}
ExitStatus runCommand(CommandBuilder *const builder);

