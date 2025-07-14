#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include <util.h>
#include <command_runner.h>

#define PATH_MAP_ARRAY_SIZE (8192)

CommandRunner makeTheRunnerIdk(void) {
    CommandRunner returned;
    returned.hash_map_arrays = mallocOrDie(sizeof(KeyValuePair) * PATH_MAP_ARRAY_SIZE);
    returned.path_map = newHashMap((KeyValuePair*) returned.hash_map_arrays, PATH_MAP_ARRAY_SIZE, NULL);

    return returned;
}

CommandRunner *byeByeCommandRunner(CommandRunner *const runner) {
    assert(runner != NULL);

    wipeMap(&runner->path_map);
    free(runner->hash_map_arrays);
    runner->hash_map_arrays = NULL;

    return runner;
}

ExitStatus executeCommand(CommandRunner *const runner, char **token_list) {
    assert(runner != NULL && token_list != NULL);

    pid_t child = fork();
    switch(child) {
        case -1:
            fputs("oops something went wrong and I didn't feel like doing error handling lmao\n", stderr);
            return NO_EXIT_STATUS;
        case 0:
            execv(token_list[0], token_list);
            perror("Failed to exec");
            exit(255);
    }

    int exit_info;
    wait(&exit_info);

    return (ExitStatus) {true, WEXITSTATUS(exit_info)};
}

