#define _POSIX_C_SOURCE 200112L

#include <vector_info.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include <util.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <resource_shortcut.h>

#define WORKING_DIRECTORY_ALLOCATION_MINIMUM (2048)
#define WORKING_DIRECTORY_ALLOCATION_EXTRA (128)

ResourceShortcut resources;

typedef struct {
	size_t length, capacity;
} WorkingDirectoryTracker;

static WorkingDirectoryTracker wd_tracker;

void initResources(void) {
	resources.home_directory = getenv("HOME");

	// is PWD present? If so, use that directly. Else, call getcwd().
	// BUG: Currently, if the PWD environment variable is set, it will *always* be used for the
	// working directory, even if it's wrong (i.e. the user set PWD to make it different from the
	// true working directory). This is not correct, and what we need to do is load both the current
	// working directory and PWD, and check their inode numbers to see if they match. We may need to
	// also expand out the paths to be their non-symlinked versions, but idk.
	char *const pwd_variable = getenv("PWD");
	if(pwd_variable) {
		wd_tracker.capacity = strlen(pwd_variable) + WORKING_DIRECTORY_ALLOCATION_EXTRA;
		wd_tracker.capacity = wd_tracker.capacity < WORKING_DIRECTORY_ALLOCATION_MINIMUM ? WORKING_DIRECTORY_ALLOCATION_MINIMUM : wd_tracker.capacity;

		resources.working_directory = mallocOrDie(wd_tracker.capacity);
		strcpy(resources.working_directory, pwd_variable);

		return;
	}

	wd_tracker.capacity = WORKING_DIRECTORY_ALLOCATION_MINIMUM;
	resources.working_directory = mallocOrDie(wd_tracker.capacity);

	while(true) {
		errno = 0;
		if(getcwd(resources.working_directory, wd_tracker.capacity)) break;

		switch(errno) {
			case ERANGE:
				wd_tracker.capacity *= VECTOR_GROW_MULTIPLIER;
				resources.working_directory = reallocOrDie(resources.working_directory, wd_tracker.capacity);
				continue;
			case ENOMEM:
				OOMPanic();
				break;
			default:
				assert(errno != EFAULT);
				assert(errno != EINVAL);

				// TODO: Use the home directory as a fallback before trying the root directory.
				resources.working_directory[0] = '/';
				resources.working_directory[1] = '\0';
				goto end_getting_working_directory;
		}
	}

	end_getting_working_directory:

	// I may not *need* the length here... running this could slow things down in the future. Idk.
	wd_tracker.length = strlen(resources.working_directory);
	if(setenv("PWD", resources.working_directory, false) == -1) OOMPanic();
}

