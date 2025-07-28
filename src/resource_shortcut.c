#include "vector_info.h"
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

