#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <tokenizer.h>
#include <command_runner.h>
#include <resource_shortcut.h>
#include <argument_parser.h>
#include <shell_types.h>

enum {
    ARG_ID_EGGSH,
    ARG_ID_STRING,
};

static CommandSchema command_line = {
    .name = "eggsh",
    .id = ARG_ID_EGGSH,
    .subcommand_count = 0,

    .short_description = "A fast, friendly, yet familiar shell for UNIX",
    .header = NULL,
    .footer = NULL,
    .displayHelp = NULL,

    .options = {
        {
            .id = ARG_ID_STRING,
            .short_options = "c",
            .long_options = {"string", NULL},

            .description = "Execute the text passed by the argument passed to this option",
            .parameters = {
                {
                    .name = "command",
                    .type = &shell_type_string,
                },
                NULL_STRUCT
            },
        },
        NULL_STRUCT
    },
};

// Running code from a script or from a string passed on the command line makes the shell run in
// non-interactive mode. This should be where we have a special flag set indicating that, and the
// prompt that runs at the end of each command should not display. If we're reading from stdin and
// stdin is an actual terminal, then we run in interactive mode.
static void runString(char *const string, CommandRunner *const runner) {
    puts("what");

    Tokenizer tokenizer = newTokenizer();
    setTokenizerInput(&tokenizer, string, strlen(string));

    while(true) {
        TokenizeCommandReturn result = tokenizeBuilderInput(&tokenizer);
        if(result & (PARSE_COMMAND_HIT_NEWLINE)) {
            TokenIterator token_iterator = getTokenIterator(&tokenizer);

            for(int i = 0; i < token_iterator.tokens_remaining; i++) {
                printf("%s$\n", token_iterator.token_ptr + i);
            }

            executeCommand(runner, &token_iterator);
            newCommand(&tokenizer);
        }

        if(result & PARSE_COMMAND_OUT_OF_DATA) {
            break;
        }
    }

    nukeTokenizer(&tokenizer);
}

static void runFile(int file_descriptor, CommandRunner *const runner) {
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

int main(int argc, char *argv[]) {
    #define PATH_MAP_ARRAY_SIZE (8192)

    char *command_string = NULL;

    // parse command line parameters here
    CommandIterator iterator = newParserIterator(argc - 1, argv + 1, &command_line, argv);
    for(CommandIteration iteration; (iteration = parseArgs(&iterator)).status != COMMAND_ITER_DONE;) {
        if(iteration.status == COMMAND_ITER_PARAMETER) switch(iteration.id) {
            case ARG_ID_STRING:
                {
                ConverterResult result = iteration.converters[0].converter(iteration.converters[0].convertee, &command_string);

                if (!result.is_ok) {
                    // TODO: do some kinda error handling here. Likely this will be adding to
                    // the compound error.
                    fputs("\33[1;91mur mom fat lmao\n", stderr);
                    return 69;
                }

                iterator.remaining_argv++;
                iterator.remaining_argc--;
                break;
                }
        }
    }

    // This at the moment just sets up the global for the current working directory. This will
    // change in the future so it's not a global, hopefully.
    initResources();

    // allocate a memory arena for all of my hash maps, including the one for your mom
    void *map_arena = mallocOrDie(sizeof(KeyValuePair) * PATH_MAP_ARRAY_SIZE);

    CommandRunner runner = makeTheRunnerIdk(map_arena, PATH_MAP_ARRAY_SIZE);

    if(command_string) {
        runString(command_string, &runner);
    } else {
        runFile(STDIN_FILENO, &runner);
    }

    byeByeCommandRunner(&runner);
    free(map_arena);
    cleanUpResources();

    return EXIT_SUCCESS;


    #undef PATH_MAP_ARRAY_SIZE
}

