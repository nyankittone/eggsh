#pragma once

// This header file exposes global variables that change between every single program rebuild.
// This can include the date and time the program was built on, but can also include things such as
// the version string, for reasons shown below.
// Many of the globals exposed here are at least partially defined in the file `version_config.mk`.

typedef struct {
    const char *program_name;
    const char *version_string; // Version number. For prerelease builds, the text "-pre-", as well
                                // as a truncated sha256 hash of the source code is appended to the
                                // end.
} BuildTimeGlobals;

extern const BuildTimeGlobals build; // Global variable containing things that change between
                                     // program rebuilds, i.e. the build date/time or version
                                     // string.

