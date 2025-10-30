#pragma once

typedef struct {
    const char *program_name;
    const char *version_string;
} BuildTimeGlobals;

extern const BuildTimeGlobals build;

