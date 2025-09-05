#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <argument_parser.h>

int handlePositionalOrSubcommand (
    const CommandSchema *const command, char *const string, char **next_positional
) {
    // TODO: This function *could* be optimized if we cache the subcommand strings in a hash map
    for(u8f i = 0; i < command->subcommand_count; i++) {
        const CommandSchema *const subcommand = command->subcommands + i;
        if(!strcmp(string, subcommand->name)) return subcommand->id;
    }

    *(next_positional++) = string;
    return NO_OPTION_ID;
}

CommandIterator newParserIterator(const int argc, char **argv, CommandSchema *const command) {
    return (CommandIterator) {
        .remaining_argc = argc,
        .remaining_argv = argv,
        .command = command,
        .current_short_option = NULL,
    };
}

// TODO: Optimize all of this. Maybe using hash maps? Or maybe don't bother bc it's a fucking
// argument parser...
CommandIteration parseArgs(CommandIterator *const iterator) {
    assert(iterator != NULL);

    // TODO: brainstorm how to do this function...
    // Go through each argv value one-by-one
    // Check if the string starts with a '-'.
        // If no, then that's either a subcommand or positional argument.
        // Check if there are subcommands for this command. If no, then add a positional argument.
        // Else, see if the argument we're looking at is in the subcommand list.
            // If yes, return with the ID for that subcommand.
            // Else, return an error and move to the next argument for the next iteration.
        // If yes, that means it's probably an option. Now we need to check if there's *another*
        //   dash immediately after that.
            // If no, is that where the string ends? If yes, go to the part checking if it's a
            //   positional arg or a subcommand. Else, it's a short option or series of. Scan the
            //   remaining string for short options.
                // If we see an invalid option, short or long, we'll need something in the iterator
                // that dynamically decides if that should cause an error, simply be ignored, or
                // treated as a positional arg/subcommand.

    // Let's try to split this function up into really tiny functions more than normal, to encourage
    // re-use of components!
    for(; iterator->remaining_argc; iterator->remaining_argv++, iterator->remaining_argc--) {
        if(**iterator->remaining_argv != '-') {
            // we are either a subcommand or a positional argument

        } else {
            // we are likely an option
        }
    }
    
    return FULL_CMD_ITER_DONE;
}

