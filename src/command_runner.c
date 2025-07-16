#include "hash_map.h"
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

#define PATH_MAP_ARRAY_SIZE (8192)

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

CommandRunner makeTheRunnerIdk(void) {
    CommandRunner returned;
    returned.hash_map_arrays = mallocOrDie(sizeof(KeyValuePair) * PATH_MAP_ARRAY_SIZE);
    returned.path_map = newHashMap((KeyValuePair*) returned.hash_map_arrays, PATH_MAP_ARRAY_SIZE, NULL);

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
    free(runner->hash_map_arrays);
    runner->hash_map_arrays = NULL;

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

ExitStatus executeCommand(CommandRunner *const runner, char **token_list) {
    assert(runner != NULL && token_list != NULL);
    
    char *const base_directory = getFromMap(&runner->path_map, token_list[0]);
    if(!base_directory) return actuallySpawnCommand(token_list[0], token_list);

    char full_path[strlen(base_directory) + strlen(token_list[0]) + 2];
    sprintf(full_path, "%s/%s", base_directory, token_list[0]);
    return actuallySpawnCommand(full_path, token_list);
}

