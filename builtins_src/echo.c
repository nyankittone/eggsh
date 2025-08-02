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
START_TEST(command_echo_hello) {
    char buffer[NORMAL_BUFFER_SIZE];
    tests_StdoutResult result = tests_getBuiltinStdout(NULL, buffer, NORMAL_BUFFER_SIZE, "echo", "Hello,", "world!~", ":3", NULL);
    tests_assertStdout(buffer, &result, "Hello, world!~ :3\n");
} END_TEST

START_TEST(command_echo_one) {
    char buffer[NORMAL_BUFFER_SIZE];
    tests_StdoutResult result = tests_getBuiltinStdout(NULL, buffer, NORMAL_BUFFER_SIZE, "echo", "Heya!", NULL);
    tests_assertStdout(buffer, &result, "Heya!\n");
} END_TEST

START_TEST(command_echo_none) {
    char buffer[NORMAL_BUFFER_SIZE];
    tests_StdoutResult result = tests_getBuiltinStdout(NULL, buffer, NORMAL_BUFFER_SIZE, "echo", NULL);
    tests_assertStdout(buffer, &result, "\n");
} END_TEST

START_TEST(command_echo_with_spaces) {
    char buffer[NORMAL_BUFFER_SIZE];
    tests_StdoutResult result = tests_getBuiltinStdout(NULL, buffer, NORMAL_BUFFER_SIZE,
        "echo", "meow   ", "    woof",
    NULL);
    tests_assertStdout(buffer, &result, "meow        woof\n");
} END_TEST

START_TEST(command_echo_empty_strings) {
    char buffer[NORMAL_BUFFER_SIZE];
    tests_StdoutResult result = tests_getBuiltinStdout(NULL, buffer, NORMAL_BUFFER_SIZE,
        "echo", "", "", "", "", "", "", "", "",
    NULL);
    tests_assertStdout(buffer, &result, "       \n");
} END_TEST

Suite *tests_echoSuite(void) {
    Suite *returned = suite_create("echo");
    TCase *echo_command = tcase_create("echo");

    tcase_add_test(echo_command, command_echo_hello);
    tcase_add_test(echo_command, command_echo_one);
    tcase_add_test(echo_command, command_echo_none);
    tcase_add_test(echo_command, command_echo_with_spaces);
    tcase_add_test(echo_command, command_echo_empty_strings);

    suite_add_tcase(returned, echo_command);
    return returned;
}
#endif

