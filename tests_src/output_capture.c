#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <builtin_commands.h>
#include <testing/output_capture.h>

#define PIPE_READ (0)
#define PIPE_WRITE (1)

tests_StdoutResult tests_getStdout (
	int (*func)(void *inputs),
	void *inputs,
	char *const buffer,
	const size_t buffer_size
) {
	assert(func != NULL);
	assert(buffer_size > 0);

	// pipe it
	int pipe_ends[2];
	{
		int pipe_result = pipe(pipe_ends);
		assert(pipe_result == 0);
	}

	// fork it
	pid_t child = fork();
	assert(child != -1);

	if(!child) {
		// close unneeded pipe ends
		close(pipe_ends[PIPE_READ]);

		// use dup2() to redirect stdout in the child to the pipe input
		dup2(pipe_ends[PIPE_WRITE], STDOUT_FILENO);

		// run the function fed in
		exit(func(inputs));
	}

	close(pipe_ends[PIPE_WRITE]);
	ssize_t total_bytes_read = 0;
	for (
		ssize_t bytes_read;
		(bytes_read = read (
			pipe_ends[PIPE_READ],
			buffer + total_bytes_read,
			buffer_size - total_bytes_read
		)) != 0 && bytes_read != -1;
		total_bytes_read += bytes_read
	);

	close(pipe_ends[PIPE_READ]);

	// TODO: figure out how to propogate this exit info.
	int exit_info;
	wait(&exit_info);

	// TODO: add this check in the for loop above...
	if(total_bytes_read == buffer_size) {
		buffer[buffer_size - 1] = '\0';
		return (tests_StdoutResult) {true, total_bytes_read, exit_info};
	}

	buffer[total_bytes_read] = '\0';
	return (tests_StdoutResult) {false, total_bytes_read, exit_info};
}

typedef struct {
	BuiltinPtr func;
	int argc;
	char **argv;
	void *extra;
} RunBuiltinParameters;

static int runBuiltin(void *parameters) {
	const RunBuiltinParameters *const casted_parameters = parameters;

	return casted_parameters->func (
		casted_parameters->argv
	);
}

tests_StdoutResult tests_getBuiltinStdout (
	void *extra, char *const buffer, size_t buffer_size, char *command_name, ...
) {
	assert(command_name != NULL);

	// Look up the builtin command.
	BuiltinPtr command_func;
	{
		const BuiltinAndKey *keyval = getShellBuiltin(command_name, strlen(command_name));
		assert(keyval != NULL);
		command_func = keyval->function;
	}

	va_list list;
	va_start(list, command_name);

	// get how many args there are. This will later be useful when I add argc to the builtin command
	// interface.
	int argc = 1;
	for(; va_arg(list, char*) != NULL; argc++);
	va_end(list);

	// Creating array of strings to pass into the builtin command
	char *argv[argc + 1];
	argv[0] = command_name;

	// Copying over values into array to pass
	va_start(list, command_name);
	for(int i = 1; i <= argc; i++) {
		argv[i] = va_arg(list, char*);
	}
	va_end(list);

	RunBuiltinParameters parameters = {
		.func = command_func,
		.argc = argc,
		.argv = argv,
		.extra = extra,
	};

	return tests_getStdout(&runBuiltin, &parameters, buffer, buffer_size);
}

