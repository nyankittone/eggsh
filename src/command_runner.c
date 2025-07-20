#include "builtin_commands.h"
#include "command_builder.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include <util.h>
#include <command_runner.h>

// I have no idea what I'm doing!!!! :3
// I love writing slop!!!

#define COMMAND_LINE_INITIAL_SIZE (512)

static int hashDirectory(HashMap *const map, char *const path) {
    assert(map != NULL && path != NULL);

    DIR *directory = opendir(path);
    if(!directory) return errno;

    // TODO: We might be able to determine the path's length before running this function. If we
    // can, just have the string's length get passed as a parameter instead.
    const size_t path_length = strlen(path);
    struct stat file_stats;

    for(struct dirent *directory_entry; directory_entry = readdir(directory);) {
        // concatenate the filename with its associated directory. I'll probably want a proper,
        // **fast**, way of doing this concatination work, but for now, I'll just shove it into a
        // VLA until I find something faster... or maybe this is the fastest reasonable way to do
        // it, who knows?
        char cheeky_merged_string_lmao[strlen(directory_entry->d_name) + path_length + 2];
        sprintf(cheeky_merged_string_lmao, "%s/%s", path, directory_entry->d_name);

        // stat the filename to see if it's a regular file
        if(stat(cheeky_merged_string_lmao, &file_stats)) {
            // TODO: Add real error handling here
            continue;
        }

        if(!S_ISREG(file_stats.st_mode)) continue;
        putInMapIfUnique(map, directory_entry->d_name, path);
    }

    closedir(directory);
    return 0;
}

CommandRunner makeTheRunnerIdk(KeyValuePair *const path_map_ptr, const size_t path_map_size) {
    CommandRunner returned;
    returned.path_map = newHashMap(path_map_ptr, path_map_size, NULL);
    returned.command_line_buffer = mallocOrDie(sizeof(char**) * COMMAND_LINE_INITIAL_SIZE); // grrrrr
    returned.command_line_capacity = COMMAND_LINE_INITIAL_SIZE;

    // TODO: Implement looking into PATH for this stuff *properly*. (will require making that
    // string memory allocater mrraow) 
    hashDirectory(&returned.path_map, "/bin");
    hashDirectory(&returned.path_map, "/home/tiffany/.local/bin");
    hashDirectory(&returned.path_map, "/home/tiffany/.nix-profile/bin");
    return returned;
}

CommandRunner *byeByeCommandRunner(CommandRunner *const runner) {
    assert(runner != NULL);

    wipeMap(&runner->path_map);
    free(runner->command_line_buffer);
    runner->command_line_capacity = 0;

    return runner;
}

static ExitStatus actuallySpawnCommand(const char *const program_path, char **const parameters) {
    pid_t child = fork();
    switch(child) {
        case -1:
            fputs("oops something went wrong and I didn't feel like doing error handling lmao\n", stderr);
            return NO_EXIT_STATUS;
        case 0:
            execv(program_path, parameters);
            perror("Failed to exec");
            exit(255);
    }

    int exit_info;
    wait(&exit_info);

    return (ExitStatus) {true, WEXITSTATUS(exit_info)};
}

ExitStatus executeCommand(CommandRunner *const runner, TokenIterator *const iterator) {
    assert(runner != NULL && iterator != NULL);

    // I may need to re-locate this re-allocating logic if I end up making this underlying array
    // part of another allocated region... meowww
    if(iterator->tokens_remaining >= runner->command_line_capacity) {
        runner->command_line_capacity = iterator->tokens_remaining + 1;
        runner->command_line_buffer = reallocOrDie(runner->command_line_buffer, sizeof(char**) * runner->command_line_capacity);
    }
    
    // get the first token from the iterator
    char *const first_token = nextToken(iterator);
    if(!first_token) {
        fputs("No command was run!\n", stderr);
        return NO_EXIT_STATUS;
    }

    // check if it's a built-in shell command first. If so, run the corresponding builtin
    {
        const BuiltinAndKey *builtin = getShellBuiltin(first_token, strlen(first_token));
        if(builtin) {
            *runner->command_line_buffer = first_token;
            *pasteRemainingTokens(iterator, runner->command_line_buffer + 1) = NULL;

            return (ExitStatus) {
                .program_exited = true,
                .exit_code = builtin->function(runner->command_line_buffer),
            };
        }
    }

    // If it's not a shell builtin, simply add that first token to the buffer and try running an
    // external command
    *runner->command_line_buffer = first_token;
    *pasteRemainingTokens(iterator, runner->command_line_buffer + 1) = NULL;

    char *const base_directory = getFromMap(&runner->path_map, runner->command_line_buffer[0]);
    if(!base_directory) return actuallySpawnCommand(runner->command_line_buffer[0], runner->command_line_buffer);

    char full_path[strlen(base_directory) + strlen(runner->command_line_buffer[0]) + 2];
    sprintf(full_path, "%s/%s", base_directory, runner->command_line_buffer[0]);
    return actuallySpawnCommand(full_path, runner->command_line_buffer);
}

