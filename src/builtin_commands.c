#include <stdio.h>
#include <builtin_commands.h>

mBuiltin(commands_echo) {
    puts("TESTING ECHO");
    argv++;

    if(!argv[0]) {
        putchar('\n');
        return 0;
    }

    fputs(*argv, stdout);
    for(argv++; *argv; argv++) {
        putchar(' ');
        fputs(*argv, stdout);
    }

    putchar('\n');
    return 0;
}

mBuiltin(commands_cd) {
    puts(argv[0]);
    return 0;
}

mBuiltin(commands_pwd) {
    puts(argv[0]);
    return 0;
}

mBuiltin(commands_exit) {
    puts(argv[0]);
    return 0;
}

mBuiltin(commands_printf) {
    puts(argv[0]);
    return 0;
}

