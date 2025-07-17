#include "hash_map.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <command_builder.h>
#include <command_runner.h>

void runFile(int file_descriptor, CommandRunner *const runner) {
    // assert that the file passed in is valid
    // check if the input file descriptor is connected to a TTY. If yes, run in interactive mode.

    #define READ_BUFFER_SIZE (64)

    CommandBuilder cmd = newCommandBuilder();

    char buffer[READ_BUFFER_SIZE];
    ssize_t buffer_length;

    while(true) {
        // read from file
        fputs("\33[1;35m>\33[m ", stderr);
        buffer_length = read(file_descriptor, buffer, READ_BUFFER_SIZE);
        if(!buffer_length) break; // A buffer length of zero means we hit EOF

        // add text read to builder for processing
        setParserInput(&cmd, buffer, buffer_length);

        // process it
        // do this in a loop until we get a signal saying we hit a newline
        while(true) {
            TokenizeCommandReturn result = tokenizeBuilderInput(&cmd);
            if(result & PARSE_COMMAND_HIT_NEWLINE) {
                break;
            }

            if(result & PARSE_COMMAND_OUT_OF_DATA) {
                buffer_length = read(file_descriptor, buffer, READ_BUFFER_SIZE);
                setParserInput(&cmd, buffer, buffer_length);

                continue;
            }
        }

        // run a command
        TokenIterator token_iterator = getTokenIterator(&cmd);
        executeCommand(runner, &token_iterator);
        newCommand(&cmd);
    }

    nukeCommandBuilder(&cmd);

    #undef READ_BUFFER_SIZE
}

int main(void) {
    #define PATH_MAP_ARRAY_SIZE (8192)

    // allocate a memory arena for all of my hash maps, including the one for your mom
    void *map_arena = mallocOrDie(sizeof(KeyValuePair) * PATH_MAP_ARRAY_SIZE);

    CommandRunner runner = makeTheRunnerIdk(map_arena, PATH_MAP_ARRAY_SIZE);

    runFile(STDIN_FILENO, &runner);

    byeByeCommandRunner(&runner);
    free(map_arena);
    return EXIT_SUCCESS;

    #undef PATH_MAP_ARRAY_SIZE
}

