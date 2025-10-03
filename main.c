#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <tokenizer.h>
#include <command_runner.h>
#include <resource_shortcut.h>
#include <argument_parser.h>

const CommandSchema command_line = {
    .name = "eggsh",
    .id = 0,
    .subcommand_count = 0, // TODO: should I make this so that the subcommand list is
                           // NULL-terminated?

    .short_description = "A fast, friendly, yet familiar shell for UNIX",
    .header = NULL,
    .footer = NULL,
    .displayHelp = NULL,

    .options = {
        {
            .id = 1,
            .short_options = "c",
            .long_options = {"string", NULL},
            
            .description = "Execute the text passed by the argument passed to this option",
            .parameters = {NULL_STRUCT},
        },
        NULL_STRUCT
    },
};

void runFile(int file_descriptor, CommandRunner *const runner) {
    // assert that the file passed in is valid
    // check if the input file descriptor is connected to a TTY. If yes, run in interactive mode.

    #define READ_BUFFER_SIZE (64)

    Tokenizer cmd = newTokenizer();

    char buffer[READ_BUFFER_SIZE];
    ssize_t buffer_length;

    while(true) {
        // read from file
        fprintf(stderr, "\33[1;35m%s >\33[m ", resources.working_directory);
        buffer_length = read(file_descriptor, buffer, READ_BUFFER_SIZE);
        if(!buffer_length) break; // A buffer length of zero means we hit EOF

        // add text read to builder for processing
        setTokenizerInput(&cmd, buffer, buffer_length);

        // process it
        // do this in a loop until we get a signal saying we hit a newline
        while(true) {
            TokenizeCommandReturn result = tokenizeBuilderInput(&cmd);
            if(result & PARSE_COMMAND_HIT_NEWLINE) {
                break;
            }

            if(result & PARSE_COMMAND_OUT_OF_DATA) {
                buffer_length = read(file_descriptor, buffer, READ_BUFFER_SIZE);
                setTokenizerInput(&cmd, buffer, buffer_length);

                continue;
            }
        }

        // run a command
        TokenIterator token_iterator = getTokenIterator(&cmd);
        executeCommand(runner, &token_iterator);
        newCommand(&cmd);
    }

    nukeTokenizer(&cmd);

    #undef READ_BUFFER_SIZE
}

int main(void) {
    #define PATH_MAP_ARRAY_SIZE (8192)

    initResources();

    // allocate a memory arena for all of my hash maps, including the one for your mom
    void *map_arena = mallocOrDie(sizeof(KeyValuePair) * PATH_MAP_ARRAY_SIZE);

    CommandRunner runner = makeTheRunnerIdk(map_arena, PATH_MAP_ARRAY_SIZE);

    runFile(STDIN_FILENO, &runner);

    byeByeCommandRunner(&runner);
    free(map_arena);
    return EXIT_SUCCESS;

    cleanUpResources();

    #undef PATH_MAP_ARRAY_SIZE
}

