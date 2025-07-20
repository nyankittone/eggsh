#pragma once

#include <stddef.h>

#define mBuiltin(function_name) int function_name (char *argv[])
typedef int (*BuiltinPtr)(char *argv[]);

mBuiltin(commands_echo);
mBuiltin(commands_printf);
mBuiltin(commands_exit);
mBuiltin(commands_pwd);
mBuiltin(commands_cd);

typedef struct {
    const char *name;
    BuiltinPtr value;
} BuiltinAndKey;

BuiltinPtr getShellBuiltin(const char *const key);
const BuiltinAndKey *generatedLookupFunction(register const char *key, register size_t length);

