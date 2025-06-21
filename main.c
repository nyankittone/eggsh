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

typedef struct {
    size_t bytes_written, capacity;
    char *ptr;
} TokensAndTree;

// TODO: Add a tree data structure in here when it's time for getting an AST working.
typedef struct {

    // TODO: Expand the auxilliary array here to also include the syntax tree once one is needed.
    TokensAndTree tokens;

    u32 total_tokens;
    char *remaining;
    size_t remaining_length;

    bool scanning_word;
} CommandBuilder;

CommandBuilder newCommandBuilder(void) {
    CommandBuilder returned = {0};
    returned.tokens.ptr = mallocOrDie(INITIAL_TOKENS_CAPACITY);
    returned.tokens.capacity = INITIAL_TOKENS_CAPACITY;
    returned.tokens.bytes_written = 0;

    return returned;
}

CommandBuilder *nukeCommandBuilder(CommandBuilder *const builder) {
    assert(builder != NULL);

    if(builder->tokens.ptr) free(builder->tokens.ptr);

    *builder = (CommandBuilder) {0};
    builder->tokens = (TokensAndTree) {0}; // idk if this line is even nessesary??? It's just here
                                           // in case tokens doesn't get zero-filled by the above
                                           // line.

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

ExitStatus runCommand(const CommandBuilder *const builder) {
    assert(builder != NULL);

    // I can put it on the stack if I use a VLA. Surely this won't cause any stack overflow issues.
    char *command_line[builder->total_tokens + 1];
    command_line[builder->total_tokens] = NULL;

    char *token = builder->tokens.ptr;
    for(u32 i = 0; i < builder->total_tokens; i++) {
        command_line[i] = token;
        token += strlen(token) + 1;
    }

    pid_t child = fork();
    switch(child) {
        case -1:
            fputs("oops something went wrong and I didn't feel like doing error handling lmao\n", stderr);
            return NO_EXIT_STATUS;
        case 0:
            execv(command_line[0], command_line);
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

