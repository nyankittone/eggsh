#pragma once

#include <stddef.h>

#ifdef RUN_TESTS
    #include <check.h>
#endif

#define mBuiltin(function_name) int function_name (char *argv[])
typedef int (*BuiltinPtr)(char *argv[]);

mBuiltin(commands_echo);
mBuiltin(commands_printf);
mBuiltin(commands_exit);
mBuiltin(commands_pwd);
mBuiltin(commands_cd);
mBuiltin(commands_true);
mBuiltin(commands_false);
mBuiltin(commands_ecode);

typedef struct {
    const char *name;
    BuiltinPtr function;
} BuiltinAndKey;

// This function is automatically generated by gperf. It is used to get the function for a builtin
// command given the command's name. If there's no builtin that matches the provided key, NULL is
// returned.
const BuiltinAndKey *getShellBuiltin(register const char *key, register size_t length);

#ifdef RUN_TESTS
Suite *tests_echoSuite(void);
#endif

