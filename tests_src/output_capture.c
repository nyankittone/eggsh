#include <stdlib.h>
#include <assert.h>
#include <output_capture.h>
#include <unistd.h>
#include <sys/wait.h>

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
		return (tests_StdoutResult) {true, total_bytes_read};
	}

	buffer[total_bytes_read] = '\0';
	return (tests_StdoutResult) {false, total_bytes_read};
}
