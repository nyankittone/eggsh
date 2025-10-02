#include <rusttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <builtin_commands.h>

// I hope this conforms to POSIX. Didn't look at the spec *too* deeply on this one...
mBuiltin(commands_exit) {
    if(!argv[1]) exit(0);

    char *hit_end = NULL;
    errno = 0;
    long exit_code = strtol(argv[1], &hit_end, 10);
    if(errno || *(hit_end + strspn(hit_end, " \t\n"))) {
        fputs("Invalid exit code passed! (should be a non-negative integer)\n", stderr);
        return 1;
    } 

    exit(exit_code);
    return 0;
}

