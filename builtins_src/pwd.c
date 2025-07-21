#include <stdio.h>
#include <builtin_commands.h>

mBuiltin(commands_pwd) {
    puts(argv[0]);
    return 0;
}

