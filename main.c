#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <util.h>
#include <rusttypes.h>

#define VECTOR_GROW_MULTIPLIER (1.5)
#define INITIAL_TOKENS_CAPACITY (4096)
#define INITIAL_TREE_CAPACITY (32)
#define INITIAL_CMDLINE_CAPACITY (64)

typedef struct {
    size_t bytes_written, capacity;
    char *ptr;
} TokenInfo;

typedef struct {
    size_t capacity;
    char **ptr;
} CommandLineSpace;

// TODO: Add a tree data structure in here when it's time for getting an AST working.
typedef struct {
    void *arena_ptr;

    // TODO: Expand the auxilliary array here to also include the syntax tree once one is needed.
    TokenInfo tokens;
    CommandLineSpace command_line;

    u32 total_tokens; // TODO: move this to be a in a struct within an entry of the tree
    char *remaining;
    size_t remaining_length;

    bool scanning_word;
} CommandBuilder;

static CommandBuilder *reallocCommandBuilder(CommandBuilder *const builder) {
    assert(builder != NULL);

    builder->arena_ptr = reallocOrDie (
        builder->arena_ptr,
        builder->command_line.capacity * sizeof(char*) + builder->tokens.capacity
    );

    builder->command_line.ptr = (char**) builder->arena_ptr;
    builder->tokens.ptr = ((char*) builder->arena_ptr) + builder->command_line.capacity * sizeof(char*);

    return builder;
}

CommandBuilder newCommandBuilder(void) {
    CommandBuilder returned = {0};
    returned.tokens.capacity = INITIAL_TOKENS_CAPACITY;
    returned.command_line.capacity = INITIAL_CMDLINE_CAPACITY;
    returned.tokens.bytes_written = 0;

    reallocCommandBuilder(&returned);
    return returned;
}

CommandBuilder *nukeCommandBuilder(CommandBuilder *const builder) {
    assert(builder != NULL);
    if(builder->arena_ptr) free(builder->arena_ptr);

    *builder = (CommandBuilder) {0};
    builder->tokens = (TokenInfo) {0}; // idk if this line is even nessesary??? It's just here
                                       // in case tokens doesn't get zero-filled by the above
                                       // line.

    builder->command_line = (CommandLineSpace) {0};
    return builder;
}

static CommandBuilder *addToToken(CommandBuilder *const builder, const char *const string, size_t length) {
    assert(builder != NULL);

    // check if there will be a buffer overflow, and if so, re-allocate the array.
    if(builder->tokens.bytes_written + length > builder->tokens.capacity) {
        builder->tokens.capacity *= VECTOR_GROW_MULTIPLIER;
        builder->tokens.ptr = reallocOrDie(builder->tokens.ptr, builder->tokens.capacity);
    }

    // append the string data in question, nyaaah~
    memcpy(builder->tokens.ptr + builder->tokens.bytes_written, string, length);
    builder->tokens.bytes_written += length;

    return builder;
}

// Do I need some other function like this for creating the first word? I don't think so...
static CommandBuilder *newToken(CommandBuilder *const builder) {
    addToToken(builder, "", 1);
    builder->total_tokens++;

    return builder;
}

CommandBuilder *setParserInput(CommandBuilder *builder, char *string, size_t length) {
    assert(builder != NULL && string != NULL);
    builder->remaining = string;
    builder->remaining_length = length;

    return builder;
}

typedef u8 TokenizeCommandReturn;
#define PARSE_COMMAND_NORMAL (0)
#define PARSE_COMMAND_OUT_OF_DATA (1)
#define PARSE_COMMAND_HIT_NEWLINE (2)

// This function takes a string representing part of a command line, and adds it to a CommandData
// struct. This function may have to get refactored in the future when I start using an AST for
// this stuff, but honestly I'm not sure.
TokenizeCommandReturn tokenizeBuilderInput(CommandBuilder *const builder) {
    assert(builder != NULL);
    if(builder->remaining_length == 0) return PARSE_COMMAND_OUT_OF_DATA;

    #define INCRIMENT_REMAINING \
        builder->remaining++; \
        if(!(--builder->remaining_length)) return PARSE_COMMAND_OUT_OF_DATA;

    // TODO: Consider adding a macro for incrimenting this builder pointer.
    // scan through remaining segment of remaining byte-by-byte.
    if(!builder->scanning_word) {
        while(true) {
            switch(*builder->remaining) {
                case '\0':
                case ' ':
                case '\t':
                    INCRIMENT_REMAINING
                    continue;
            }

            break;
        }

        builder->scanning_word = true;

        INCRIMENT_REMAINING
    }

    while(true) {
        while(true) {
            if(*builder->remaining == '\0') {
                INCRIMENT_REMAINING
                continue;
            }

            if(*builder->remaining != ' ' && *builder->remaining != '\t') {
                // Add logic to add characters here

                INCRIMENT_REMAINING
                continue;
            }

            break;
        }

        builder->scanning_word = false;

        INCRIMENT_REMAINING

        while(true) {
            switch(*builder->remaining) {
                case '\0':
                case ' ':
                case '\t':
                    INCRIMENT_REMAINING
                    continue;
            }

            break;
        }

        builder->scanning_word = true;

        INCRIMENT_REMAINING

    }

    return PARSE_COMMAND_NORMAL;

    #undef INCRIMENT_REMAINING
}

typedef struct {
    bool program_exited;
    u8f exit_code;
} ExitStatus;

#define NO_EXIT_STATUS (ExitStatus){.program_exited = false, .exit_code = 0}

ExitStatus runCommand(CommandBuilder *const builder) {
    assert(builder != NULL);

    if(builder->total_tokens >= builder->command_line.capacity) {
        builder->command_line.capacity = builder->total_tokens + 1;
        reallocCommandBuilder(builder);
    }

    builder->command_line.ptr[builder->total_tokens] = NULL;
    char *token = builder->tokens.ptr;

    for(u32 i = 0; i < builder->total_tokens; i++) {
        builder->command_line.ptr[i] = token;
        token += strlen(token) + 1;
    }

    pid_t child = fork();
    switch(child) {
        case -1:
            fputs("oops something went wrong and I didn't feel like doing error handling lmao\n", stderr);
            return NO_EXIT_STATUS;
        case 0:
            execv(builder->command_line.ptr[0], builder->command_line.ptr);
            perror("Failed to exec");
            exit(255);
    }

    int exit_info;
    wait(&exit_info);

    return (ExitStatus) {true, WEXITSTATUS(exit_info)};
}

int main(void) {
    CommandBuilder cmd = newCommandBuilder();
    addToToken(&cmd, "/bin/l", 6);
    addToToken(&cmd, "s", 1);
    newToken(&cmd);
    addToToken(&cmd, "-al", 3);
    newToken(&cmd);
    addToToken(&cmd, "/", 1);
    newToken(&cmd);
    addToToken(&cmd, "--color", 7);
    newToken(&cmd);

    ExitStatus exit_stuff = runCommand(&cmd);
    nukeCommandBuilder(&cmd);

    int exit_code = 1;
    if(!exit_stuff.program_exited) {
        fputs("no program was executed.\n", stderr);
    } else {
        exit_code = exit_stuff.exit_code;
    }

    return exit_code;
}

