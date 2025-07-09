#pragma once

// What kind of data structures should be kept for executing a command??
// - the command line array itself, of course
// - A hash map for keeping the names of executables found in PATH, and their
//   paths
// - static hash map of functions for internal commands
// - hash maps for aliases and functions, whenever I implement those

// Should this struct hold on to pointers for its fields? Or no? It may be a good idea to not do
// that for performance reasons, but that could make designing the system less flexible.
typedef struct {
    char placeholder;
} CommandRunner;


