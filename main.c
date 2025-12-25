// #define _POSIX_C_SOURCE 199309L
#define _XOPEN_SOURCE 500

#include <signal.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <setjmp.h>

#include <tokenizer.h>
#include <command_runner.h>
#include <resource_shortcut.h>
#include <argument_parser.h>
#include <shell_types.h>
#include <misc.h>

jmp_buf sigint_buf;

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

static inline int noSignal(const int signal, struct sigaction *const old_handler) {
    return sigaction(signal, &(struct sigaction){
        .sa_handler = SIG_IGN,
    }, old_handler);
}

static void handleSigint(int _) {
    fputc('\n', stderr);
    sigaction(SIGINT, &(struct sigaction) {
        .sa_handler = &handleSigint,
        .sa_flags = SA_NODEFER,
    }, NULL);
    longjmp(sigint_buf, 1);
}

// Properly setting up signal handling that is appropriate for interactive sessions, according to
// POSIX.
static void setupSignals(void) {
    noSignal(SIGTERM, NULL);
    noSignal(SIGQUIT, NULL);
    noSignal(SIGTTIN, NULL);
    noSignal(SIGTTOU, NULL);
    noSignal(SIGTSTP, NULL);

    sigaction(SIGINT, &(struct sigaction) {
        .sa_handler = &handleSigint,
        .sa_flags = SA_NODEFER,
    }, NULL);
}

// Running code from a script or from a string passed on the command line makes the shell run in
// non-interactive mode. This should be where we have a special flag set indicating that, and the
// prompt that runs at the end of each command should not display. If we're reading from stdin and
// stdin is an actual terminal, then we run in interactive mode.
static void runString(char *const string, CommandRunner *const runner) {
    const size_t string_length = strlen(string); // This is a little hacky, but I don't care rn, I'm
                                                 // tired.
    string[string_length] = '\n';

    Tokenizer tokenizer = newTokenizer();
    setTokenizerInput(&tokenizer, string, string_length + 1);

    while(true) {
        TokenizeCommandReturn result = tokenizeBuilderInput(&tokenizer);
        if(result & (PARSE_COMMAND_COMMAND_STOP)) {
            TokenIterator token_iterator = getTokenIterator(&tokenizer);

            executeCommand(runner, &token_iterator);
            newCommand(&tokenizer);
        } else if(result & PARSE_COMMAND_HIT_NEWLINE) continue;

        if(result & PARSE_COMMAND_OUT_OF_DATA) {
            break;
        }
    }

    nukeTokenizer(&tokenizer);
}

typedef enum {
    RUN_COMMAND_NEXT_COMMAND,
    RUN_COMMAND_STOP,
    RUN_COMMAND_NEXT_LINE,
} RunCommandFromFileReturn;

// This code does not correctly handle command stops that are not caused by a line break (eg. a
// semicolon character). So semicolon characters are not handled the way they should when reading
// from a file or running in interactive mode...
static RunCommandFromFileReturn runCommandFromFile (
    CommandRunner *const runner, Tokenizer *const tokenizer,
    const int file_descriptor, char *const buffer,
    const int buffer_size
) {
    ssize_t buffer_length;

    #define RUN_THE_COMMAND { \
        TokenIterator token_iterator = getTokenIterator(tokenizer); \
        executeCommand(runner, &token_iterator); \
        newCommand(tokenizer); \
    }

    // Need to add a handle for a command stop without a newline. This may require adding new
    // return types or doing a refactor...
    TokenizeCommandReturn result = tokenizeBuilderInput(tokenizer);
    if((result & PARSE_COMMAND_COMMAND_STOP) && !(result & PARSE_COMMAND_HIT_NEWLINE)) {
        RUN_THE_COMMAND;
    } else if((result & PARSE_COMMAND_HIT_NEWLINE)) {
        if(!(result & PARSE_COMMAND_COMMAND_STOP)) {
            return RUN_COMMAND_NEXT_LINE;
        }

        RUN_THE_COMMAND;

        return RUN_COMMAND_NEXT_COMMAND;
    }

    // This is just a placeholder until proper handling of this case pops up. At the moment, this
    // being false is probably a bug or something, idk.
    assert((result & PARSE_COMMAND_OUT_OF_DATA) == true);

    buffer_length = read(file_descriptor, buffer, buffer_size);
    if(!buffer_length) return RUN_COMMAND_STOP; // A buffer length of zero means we hit EOF

    // add text read to builder for processing
    setTokenizerInput(tokenizer, buffer, buffer_length);

    // process it
    // do this in a loop until we get a signal saying we hit a newline
    while(true) {
        result = tokenizeBuilderInput(tokenizer);
        if(result & PARSE_COMMAND_HIT_NEWLINE) {
            break;
        }

        if(result & PARSE_COMMAND_COMMAND_STOP) {
            RUN_THE_COMMAND;

            continue;
        }

        if(result & PARSE_COMMAND_OUT_OF_DATA) {
            buffer_length = read(file_descriptor, buffer, buffer_size);
            if(!buffer_length) return RUN_COMMAND_STOP;

            setTokenizerInput(tokenizer, buffer, buffer_length);

            continue;
        }
    }

    if(!(result & PARSE_COMMAND_COMMAND_STOP)) {
        return RUN_COMMAND_NEXT_LINE;
    }

    RUN_THE_COMMAND;
    return RUN_COMMAND_NEXT_COMMAND;
}

static void runFile(int file_descriptor, CommandRunner *const runner) {
    // assert that the file passed in is valid
    // check if the input file descriptor is connected to a TTY. If yes, run in interactive mode.

    #define READ_BUFFER_SIZE (64)
    char buffer[READ_BUFFER_SIZE];

    Tokenizer cmd = newTokenizer();

    // I may have to add an extra check to ensure if the file descriptor is stdin.
    if(isatty(file_descriptor)) {
        setupSignals();
        printVersionInfo(stderr);

        RunCommandFromFileReturn result = RUN_COMMAND_NEXT_COMMAND;
        while(result != RUN_COMMAND_STOP) {
            if(result == RUN_COMMAND_NEXT_COMMAND) {
                setjmp(sigint_buf);
                fprintf(stderr, "\33[1;35m%s >\33[m ", resources.working_directory);
            } else {
                setjmp(sigint_buf);
                fputs("\33[1;35m?\33[m ", stderr);
            }

            result = runCommandFromFile(runner, &cmd, file_descriptor, buffer, READ_BUFFER_SIZE);
        }
    } else {
        while (runCommandFromFile (
            runner, &cmd, file_descriptor, buffer, READ_BUFFER_SIZE
        ) != RUN_COMMAND_STOP);
    }

    nukeTokenizer(&cmd);

    #undef READ_BUFFER_SIZE
}

int main(int argc, char *argv[]) {
    #define PATH_MAP_ARRAY_SIZE (8192)

    char *command_string = NULL;

    // parse command line parameters here
    // This code looks like moon runes after not looking at it for like 2 months...
    // How was I supposed to do the abstraction here that I wanted to? Variadic function?
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
    // TODO: Add error handling for here, and also a write-out for the new argc.

    // This at the moment just sets up the global for the current working directory. This will
    // change in the future so it's not a global, hopefully.
    initResources();

    // allocate a memory arena for all of my hash maps, including the one for your mom
    void *map_arena = mallocOrDie(sizeof(KeyValuePair) * PATH_MAP_ARRAY_SIZE);
    CommandRunner runner = makeTheRunnerIdk(map_arena, PATH_MAP_ARRAY_SIZE);

    if(command_string) {
        runString(command_string, &runner);
        goto finish_main; // Consider making a cleanup function here instead of doing stupid-ass
                           // gotos.
    } 

    // It's weird how we're checking this parameter after initializing our memory chunks and
    // whatnot... eeeeh
    if(iterator.positional_argc > 0) {
        const int file = open(iterator.positional_argv[0], O_RDONLY, 0);
        if(file == -1) {
            perror("Failed to open provided path");
            return 12; // I really need to think of a proper exit code for this.
        }

        runFile(file, &runner);

        if(close(file) == -1) {
            perror("Failed to close opened file"); // We're not returning early as there's no need;
                                                   // we're  shutting down the program by this point
                                                   // anyway.
        }
    } else {
        runFile(STDIN_FILENO, &runner);
    }

    finish_main:
    byeByeCommandRunner(&runner);
    free(map_arena);
    cleanUpResources();

    return EXIT_SUCCESS;

    #undef PATH_MAP_ARRAY_SIZE
}

