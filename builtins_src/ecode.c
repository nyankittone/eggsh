#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <builtin_commands.h>
#include <stdio.h>

// TODO: Need to add a --help argument to all of these commands... meow

mBuiltin(commands_ecode) {
	if(!argv[1]) {
		fputs("No exit code passed!\n", stderr);
		return 1;
	}
	
	char *hit_end;
    errno = 0;
	long returned = strtol(argv[1], &hit_end, 10);
	if(errno || *(hit_end + strspn(hit_end, " \t\n"))) {
		fputs("Invalid code passed! (should be a non-negative integer)\n", stderr);
		return 1;
	}

	return returned;
}

mBuiltin(commands_true) {
	return 0;
}

mBuiltin(commands_false) {
	return 1;
}

