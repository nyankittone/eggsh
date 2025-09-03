#include <argument_parser.h>

CommandIterator newParserIterator(const int argc, char **argv, CommandSchema *const command) {
    return (CommandIterator) {
        .remaining_argc = argc,
        .remaining_argv = argv,
        .command = command,
    };
}

CommandIteration parseArgs(CommandIterator *const iterator) {
    // TODO: barinstorm how to do this function...
}

