#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <builtin_commands.h>

// TODO: Make this implementation of `cd` properly POSIX-conforming. ALSO, fix the bug where this
// behaves incorrectly when `cd`ing into a path with symlinks in it.
mBuiltin(commands_cd) {
    if(!argv[1]) {
        // Is there a faster way of referencing the HOME environment variable that doesn't involve
        // us just caching the value of it?
        char *const home_directory = getenv("HOME");
        if(!home_directory) {
            fputs("Cannot switch to home directory: $HOME is not set!\n", stderr);
            return 1;
        }

        return commands_cd((char*[]){argv[0], home_directory, NULL});
    }

    if(chdir(argv[1]) == -1) {
        perror("Cannot change directory");
        return 2;
    }

    return 0;
}
