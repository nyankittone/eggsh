#pragma once

#include <hash_map.h>

typedef struct {
    void *hash_map_arrays;
    HashMap path_map;
} CommandRunner;

CommandRunner makeTheRunnerIdk(void);
CommandRunner *byeByeCommandRunner(CommandRunner *const runner);

typedef struct {
    bool program_exited;
    u8f exit_code;
} ExitStatus;
#define NO_EXIT_STATUS (ExitStatus){.program_exited = false, .exit_code = 0}

ExitStatus executeCommand(CommandRunner *const runner, char **token_list);
