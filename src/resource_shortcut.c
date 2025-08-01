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
#include <rusttypes.h>

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

static bool appendOneToPath(const char *const path, const size_t path_length, size_t *const remaining_capacity, const size_t offset, size_t *const write_point) {
	if(!path_length) return false;

	// is a directory "."? Then don't append it
	// is a directory ".."? Then un-append the last thing added
	// Else, append that path
	if(path[0] == '.') {
		if(path[1] == '\0' || path[1] == '/') return false;
		if(path[1] == '.' && (path[2] == '\0' || path[2] == '/')) {
			// totally not scuffed code at all,,, (also TODO: test alternative ways of doing this to
			// see if this can be done faster)
			resources.working_directory[*write_point -= 1] = '\0';
			const char *new_write_point = strrchr(resources.working_directory + offset + 1, '/');
			if(!new_write_point) new_write_point = resources.working_directory + offset + 1;

			*write_point = new_write_point - resources.working_directory;
			return true;
		}
	}

	if(*remaining_capacity <= path_length) {
		wd_tracker.capacity *= VECTOR_GROW_MULTIPLIER;

		if(*remaining_capacity <= path_length) {
			wd_tracker.capacity = wd_tracker.length + path_length +
				WORKING_DIRECTORY_ALLOCATION_EXTRA;
		}

		*remaining_capacity = wd_tracker.capacity - offset;
		resources.working_directory = reallocOrDie(resources.working_directory, wd_tracker.capacity);
	}

	memcpy(resources.working_directory + *write_point, path, path_length);
	*write_point += path_length;

	return true;
}

static size_t appendToPath(const char *path, const size_t offset, size_t length) {
	size_t write_point = offset + length;
	size_t remaining_capacity = wd_tracker.capacity - offset;

	// iterate through path provided
	for(const char *slash; slash = strchr(path, '/'); path = slash + 1) {

		size_t length_to_slash = slash - path;
		if(appendOneToPath (
			path,
			length_to_slash,
			&remaining_capacity,
			offset,
			&write_point
		)) resources.working_directory[write_point++] = '/';

	}

	// Run the stuff in the loop one more outside of it
	appendOneToPath(path, strlen(path), &remaining_capacity, offset, &write_point);

	resources.working_directory[write_point] = '\0'; // idk if it makes sense to have this here...
	return write_point - offset;
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

		if(stat(resources.working_directory, &working_directory_stats) == -1) {

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
			resources.working_directory[0] = '/';
			wd_tracker.length = appendToPath(pwd_variable + 1, 0, 1);
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

// I feel like this code should get unit tested at some point...


// This must:
// * get the working directory
// * somehow preserve the state of the working directory before any changes were made
// * scan through the path string
// * for every occurrence of "." or ".." for a directory, ignore it or trim off the last direcotry in
//   the working directory respectively
// * once the final string is there, set OLDPWD to the old string
// * set PWD appropriately
const StringAndLength stageNewWD(const char *path) {
	assert(path != NULL);
	if(!*path) return NO_STRING_AND_LENGTH;

	size_t minimum_buffer_size = (wd_tracker.length + 1) << 1;
	if(minimum_buffer_size > wd_tracker.capacity) {
		// reallocate the working directory buffer to be larger :3
		wd_tracker.capacity = minimum_buffer_size + WORKING_DIRECTORY_ALLOCATION_EXTRA;
		resources.working_directory = reallocOrDie(resources.working_directory, wd_tracker.capacity);
	}

	size_t new_directory_start = wd_tracker.length + 1;
	size_t new_length;

	// First check if the path provided is absolute. If so, iterate through that string only. Else,
	// iterate through the old working directory, and *then* the path provided.
	if(*path == '/') {
		new_length = 1;
		resources.working_directory[new_directory_start] = '/';
		new_length = appendToPath(path + 1, new_directory_start, new_length);
	} else {
		new_length = wd_tracker.length;
		memcpy(resources.working_directory + new_directory_start, resources.working_directory, new_length);
		if(new_length > 1) resources.working_directory[new_directory_start + new_length++] = '/';

		printf("AAAAA %zu\n", new_length);
		new_length = appendToPath(path, new_directory_start, new_length);
		printf("BBBBB %zu\n", new_length);

		if (
			resources.working_directory[new_directory_start + new_length - 1] == '/' &&
			new_length > 1
		) new_length--;
	}

	return (StringAndLength) {
		.ptr = resources.working_directory + new_directory_start,
		.length = new_length
	};
}

// TODO: Will it be faster to just pass this struct by value? I should test that,,
void applyNewWD(const StringAndLength *const new_path) {
	// Shift the memory contents over to where it should be and make it null-terminated.
	memmove(resources.working_directory, new_path->ptr, new_path->length);
	resources.working_directory[new_path->length] = '\0';
	wd_tracker.length = new_path->length;
}

