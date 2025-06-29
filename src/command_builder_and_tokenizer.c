#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <command_builder.h>

static CommandBuilder *reallocCommandBuilder(CommandBuilder *const builder) {
    assert(builder != NULL);

    builder->arena_ptr = reallocOrDie (
        builder->arena_ptr,
        builder->command_line.capacity * sizeof(char*) + builder->tokens.capacity
    );

    // TODO: memmove all the major parts of the memory arena as to avoid stupid problems.
    // (important)

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
        reallocCommandBuilder(builder);
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
    builder->lagged_remaining = string;
    builder->remaining_length = length;

    return builder;
}

TokenizeCommandReturn tokenizeBuilderInput(CommandBuilder *const builder) {
    assert(builder != NULL);
    if(builder->remaining_length == 0) return PARSE_COMMAND_OUT_OF_DATA;

    TokenizeCommandReturn returned = PARSE_COMMAND_NORMAL;

    #define INCRIMENT_REMAINING \
        builder->remaining++; \
        if(!(--builder->remaining_length)) return returned | PARSE_COMMAND_OUT_OF_DATA;

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
                case '\n':
                    returned |= PARSE_COMMAND_HIT_NEWLINE;
                    INCRIMENT_REMAINING
                    return returned;
            }

            break;
        }

        builder->scanning_word = true;
    }

    builder->lagged_remaining = builder->remaining;

    while(true) {
        while(true) {
            switch(*builder->remaining) {
                case '\0':
                    addToToken(builder, builder->lagged_remaining, builder->remaining - builder->lagged_remaining);
                    INCRIMENT_REMAINING
                    builder->lagged_remaining = builder->remaining;

                    continue;
                case '\n':
                    // Add logic to add characters here
                    addToToken(builder, builder->lagged_remaining, builder->remaining - builder->lagged_remaining);
                    newToken(builder);
                    returned |= PARSE_COMMAND_HIT_NEWLINE;
                    INCRIMENT_REMAINING
                    builder->lagged_remaining = builder->remaining;
                    return returned;
            }

            if(*builder->remaining != ' ' && *builder->remaining != '\t') {
                // Add GOOD logic to add characters here

                INCRIMENT_REMAINING
                continue;
            }

            break;
        }

        builder->scanning_word = false;
        addToToken(builder, builder->lagged_remaining, builder->remaining - builder->lagged_remaining);
        newToken(builder);

        INCRIMENT_REMAINING
        builder->lagged_remaining = builder->remaining;

        while(true) {
            switch(*builder->remaining) {
                case '\0':
                case ' ':
                case '\t':
                    INCRIMENT_REMAINING
                    continue;
                case '\n':
                    returned |= PARSE_COMMAND_HIT_NEWLINE;
                    INCRIMENT_REMAINING
                    return returned;
            }

            break;
        }

        builder->scanning_word = true;
        builder->lagged_remaining = builder->remaining;
    }

    return PARSE_COMMAND_NORMAL;

    #undef INCRIMENT_REMAINING
}

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

CommandBuilder *newCommand(CommandBuilder *const builder) {
    assert(builder != NULL);

    // TODO: Have this function handle shrinking the memory arena if the vibes for its size are
    // off.
    builder->total_tokens = 0;
    builder->tokens.bytes_written = 0;

    return builder;
}

