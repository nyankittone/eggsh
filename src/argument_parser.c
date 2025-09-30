#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <argument_parser.h>

int isSubcommand(const CommandSchema *const command, char *const string) {
    // TODO: This function *could* be optimized if we cache the subcommand strings in a hash map. Is
    // that something even worht doing? Idk. Probably not, but I may want to think about it later.
    for(u8f i = 0; i < command->subcommand_count; i++) {
        const CommandSchema *const subcommand = command->subcommands + i;
        if(!strcmp(string, subcommand->name)) return subcommand->id;
    }

    return NO_OPTION_ID;
}

typedef enum {
    ARGPARSE_OPT_NOT,
    ARGPARSE_OPT_SHORT,
    ARGPARSE_OPT_LONG,
} ArgumentOptionType;

ArgumentOptionType getArgOptionType(const char *const parameter) {
    assert(parameter != NULL);
    return *parameter == '-' ? (parameter[1] == '-' ? ARGPARSE_OPT_LONG : ARGPARSE_OPT_SHORT)
        : ARGPARSE_OPT_NOT;
}

void collectArgConverters(ConverterFunction dest[5], const OptionParameter *const params) {
    // I wonder if SIMD could make this faster. I genuinely don't know.
    u8f i = 0;

    for(; memcmp(params + i, &NULL_OPTION_PARAMETER, sizeof(OptionParameter)); i++) {
        dest[i] = params[i].type->converter;
    }

    dest[i] = NULL;
}

CommandIteration getLongOptionReturn(const CommandSchema *const command, char *arg) {
    arg += 2;

    // Look up the option from the one provided. This might also benefit from a hash map, bc
    // performing a linear search over this makes me feel sad.
    CommandOption *const option = command->options;

    // Looping through all options, and their long flags inside each option (a single
    // option can have multiple names/flags)
    for(u8f i = 0; i < command->options_count; i++) {
        for(char **option_text = option[i].long_options; *option_text; option_text++) {
            if(!strcmp(*option_text, arg)) {
                CommandIteration returned;
                returned.status = COMMAND_ITER_PARAMETER;
                returned.data.on_success.id = option->id;
                collectArgConverters(returned.data.on_success.converters, option->parameters);

                return returned;
            }
        }
    }

    return mCmdIterBadFlag(arg);
}

CommandIterator newParserIterator(const int argc, char **argv, CommandSchema *const command, char **positional_args) {
    return (CommandIterator) {
        .remaining_argc = argc,
        .remaining_argv = argv,
        .command = command,
        .current_short_option = NULL,
        .positional_argc = 0,
        .positional_argv = positional_args,
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
        switch(getArgOptionType(*iterator->remaining_argv)) {
            case ARGPARSE_OPT_NOT:
            {
                // we are either a subcommand or a positional argument
                const int subcommand_id = isSubcommand(iterator->command, *iterator->remaining_argv);
                if(subcommand_id == NO_OPTION_ID) {
                    iterator->positional_argv[iterator->positional_argc++] = *iterator->remaining_argv;
                    continue;
                }

                iterator->remaining_argv++;
                iterator->remaining_argc--;
                return mCmdIterSubcommand(subcommand_id);
                break;
            }
            case ARGPARSE_OPT_SHORT:
                // TODO: Implement
                break;
            case ARGPARSE_OPT_LONG:

                // If the long option is just a "--", then all other arguments passed will be
                // positional arguments. Shove all the args into the positional arg array and
                // return.
                if(*iterator->remaining_argv[2] == '\0') {
                    // memmove() the remaining args passed into the positional_args array
                    iterator->remaining_argv++;
                    iterator->remaining_argc--;

                    iterator->positional_argc = iterator->remaining_argc;
                    memmove(iterator->positional_argv, iterator->remaining_argv, iterator->remaining_argc);
                    iterator->positional_argv[iterator->positional_argc] = NULL;
                    iterator->remaining_argc = 0;
                    return FULL_CMD_ITER_DONE;
                }


                CommandIteration returned = getLongOptionReturn(iterator->command, *iterator->remaining_argv);
                iterator->remaining_argv++;
                iterator->remaining_argc--;
                return returned;

                break;
        }
    }
    
    return FULL_CMD_ITER_DONE;
}

// TODO: Add tests

