#pragma once

#include <tokenizer.h>
#include <hash_map.h>

// This struct represents the object that takes a list of tokens from a tokenizer, and actually runs
// a command off of them.
typedef struct {
    // This array might get merged with some other vector at some point on the future, if doing
    // such a thing feels appropriate.
    char **command_line_buffer;
    size_t command_line_capacity;
    char *path_env_store; // This way of keeping track of the PATH environment variable is doomed to
                          // change a lot over time.

    HashMap path_map;
} CommandRunner;

// Function for creating a `CommandRunner`. The parameters passed are for specifying where the hash
// map created and managed by this object should keep its bucket array. This allows for the map to
// use part of the memory for some much larger allocation that many other maps share.
CommandRunner makeTheRunnerIdk(KeyValuePair *const path_map_ptr, const size_t path_map_size);

// This function cleans up any memory owned by the `CommandRunner` object passed in, and then zeroes
// out the struct for it.
CommandRunner *byeByeCommandRunner(CommandRunner *const runner);

typedef struct {
    bool program_exited;
    u8f exit_code;
} ExitStatus;
#define NO_EXIT_STATUS (ExitStatus){.program_exited = false, .exit_code = 0}

// This function is the thing that actually runs commands. It takes in an iterator from the
// tokenizer to pull tokens from that object.
ExitStatus executeCommand(CommandRunner *const runner, TokenIterator *const token_iterator);

