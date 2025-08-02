#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <builtin_commands.h>

// I'm honestly completely unsure if I'll ever have a use for the first two functions defined here.

typedef struct {
	bool exceeded_buffer;
	size_t bytes_read;
} tests_StdoutResult;

tests_StdoutResult tests_getStdout(int (*func)(void *inputs), void *inputs, char *const buffer, size_t buffer_size);

bool tests_getBuiltinStdout(size_t buffer_size, BuiltinPtr command, ...);

