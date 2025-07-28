#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#include <resource_shortcut.h>
#include <vector_info.h>
#include <util.h>

#define WORKING_DIRECTORY_ALLOCATION_MINIMUM (2048)
#define WORKING_DIRECTORY_ALLOCATION_EXTRA (256)

ResourceShortcut resources;

typedef struct {
	size_t length, capacity;
} WorkingDirectoryTracker;

static WorkingDirectoryTracker wd_tracker;

static void useFallbackDirectory(void) {
	// TODO: Use the home directory as a fallback before trying the root directory.
	resources.working_directory[0] = '/';
	resources.working_directory[1] = '\0';
	wd_tracker.length = 1;

	if(setenv("PWD", resources.working_directory, false) == -1) OOMPanic();
}

void initResources(void) {
	resources.home_directory = getenv("HOME");

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

				// TODO: Just make this thing return directly from here instead of doing a funny
				// goto.
				useFallbackDirectory();
				return;
		}
	}

	// I may not *need* the length here... running this could slow things down in the future. Idk.
	wd_tracker.length = strlen(resources.working_directory);

	char *const pwd_variable = getenv("PWD");
	if(pwd_variable) {
		struct stat working_directory_stats, pwd_variable_stats;

		if(!stat(resources.working_directory, &working_directory_stats)) {
			// TODO: add logging to here to announce the fail
			useFallbackDirectory();
			return;
		}

		assert(S_ISDIR(working_directory_stats.st_mode));

		if(!stat(pwd_variable, &pwd_variable_stats)) {
			// TODO: add logging to here to announce the fail
			// Use the other current working directory that was obtained
			if(setenv("PWD", resources.working_directory, false) == -1) OOMPanic();
			return;
		}

		// If the inode numbers match, we'll use the PWD environment variable for the working
		// directory.
		if(working_directory_stats.st_ino == pwd_variable_stats.st_ino) {
			wd_tracker.length = strlen(pwd_variable);
			if(wd_tracker.length >= wd_tracker.capacity) {
				wd_tracker.capacity = wd_tracker.length + WORKING_DIRECTORY_ALLOCATION_EXTRA;
				resources.working_directory = reallocOrDie(resources.working_directory, wd_tracker.capacity);
			}

			strcpy(resources.working_directory, pwd_variable);
			return;
		}
	}

	if(setenv("PWD", resources.working_directory, false) == -1) OOMPanic();
}

void cleanUpResources(void) {
	free(resources.working_directory);
	resources = (ResourceShortcut) {0};
	wd_tracker = (WorkingDirectoryTracker) {0};
}

// This must:
// * get the working directory
// * somehow preserve the state of the working directory before any changes were made
// * scan through the path string
// * for every occurrence of "." or ".." for a directory, ignore it or trim off the last direcotry in
//   the working directory respectively
// * once the final string is there, set OLDPWD to the old string
// * set PWD appropriately
void updatePWD(const char *path) {
	size_t minimum_buffer_size = (wd_tracker.length + 1) << 1;
	if(minimum_buffer_size > wd_tracker.capacity) {
		// reallocate the working directory buffer to be larger :3
		wd_tracker.capacity = minimum_buffer_size + WORKING_DIRECTORY_ALLOCATION_EXTRA;
		resources.working_directory = reallocOrDie(resources.working_directory, wd_tracker.capacity);
	}

	char *new_working_directory = resources.working_directory + wd_tracker.length + 1;
	// PERF: How do I want to deal with when I encounter a ".." sequence?
	// Should I just backtrack a pointer until I find a "/"? (might be slow due to not using
	// optimized standard library functions)
	// Should I just run strrchr() on new_working_directory? (will be slower for larger paths, but
	// maybe that's fine)
	// Should I have an index in wd_tracker for how long each directory in the working directory
	// path is at all times, so that I can just roll back a pointer by that amount? (could be faster
	// than the former two approaches, but I'm unsure; I'll have to benchmark this approach because
	// it *feels* like the right solution.)
}

