#include <stdio.h>
#include <builtin_commands.h>

mBuiltin(commands_echo) {
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

#ifdef RUN_TESTS
Suite *tests_echoSuite(void) {
    Suite *returned = suite_create("echo");
    TCase *integ = tcase_create("echo command");

    // tcase_add_test(integ, dsfgdsfgdg);

    suite_add_tcase(returned, integ);
    return returned;
}
#endif

