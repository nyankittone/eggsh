#include <stdio.h>
#include <builtin_commands.h>

mBuiltin(commands_printf) {
    puts(argv[0]);
    return 0;
}


