#pragma once

#include <command_builder.h>
#include <hash_map.h>

typedef struct {
    void *hash_map_arrays;
    
    // This array might get merged with some other vector at some point on the future, if doing
    // such a thing feels appropriate.
    char **command_line_buffer;
    size_t command_line_capacity;

    HashMap path_map;
} CommandRunner;

CommandRunner makeTheRunnerIdk(void);
CommandRunner *byeByeCommandRunner(CommandRunner *const runner);

typedef struct {
    bool program_exited;
    u8f exit_code;
} ExitStatus;
#define NO_EXIT_STATUS (ExitStatus){.program_exited = false, .exit_code = 0}

ExitStatus executeCommand(CommandRunner *const runner, TokenIterator *const token_iterator);

