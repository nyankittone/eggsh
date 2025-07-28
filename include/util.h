#ifndef UWUTIL_INCLUDED
#define UWUTIL_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#define mMax(x, y) (x > y ? x : y)
#define mMin(x, y) (x > y ? y : x)

#define FATAL_ERROR_TEXT "\33[1;91mfatal:\33[m "
#define REALLY_FATAL_ERROR_TEXT "\33[1;91mKILL IT WITH FIRE:\33[m "

enum ExitCode {
    EXIT_PARSING_FAILURE = 1,
    EXIT_OUT_OF_MEMORY,
    EXIT_PRE_LAUNCH_ERROR,
};

void panic(int exit_code, const char *const message_format, ...);
void OOMPanic(void);

void *reallocOrDie(void *old_block, size_t new_allocated_size);
void *mallocOrDie(size_t new_allocated_size);

#define mHaltAndCatchFire(message) { const char *const file = __FILE__; const int line = __LINE__; \
    fprintf(stderr, "%s\33[1;97m%s, line %d:\33[m %s\n", REALLY_FATAL_ERROR_TEXT, file, line, (message)); \
    fputs("note: This error message should *NEVER* appear. If it does, this program has a bug!\n", stderr); \
    abort(); \
}

#endif

