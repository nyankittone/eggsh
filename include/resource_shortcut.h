#pragma once

#include <util.h>

typedef struct {
	char *working_directory;
	const char *old_working_directory;
	const char *home_directory;
} ResourceShortcut;

extern ResourceShortcut resources;

// This function will set up the "cache" of environment variables and other similar things for more
// efficient access.
void initResources(void);

void cleanUpResources(void);

// This must:
// * get the working directory
// * somehow preserve 
// * scan through the path string
// * for every occurrence of "." or ".." for a directory, ignore it or trim off the last direcotry in
//   the working directory respectively
// * once the final string is there, set OLDPWD to the old string
// * set PWD appropriately
const StringAndLength stageNewWD(const char *path);

void applyNewWD(const StringAndLength *const string);

