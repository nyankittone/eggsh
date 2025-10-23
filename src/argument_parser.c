#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <argument_parser.h>
#include <string.h>

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

void collectArgConverters(ConverterWithInput dest[5], const OptionParameter *const params, char **relative_argv) {
    // I wonder if SIMD could make this faster. I genuinely don't know.
    u8f i = 0;

    for(; memcmp(params + i, &NULL_OPTION_PARAMETER, sizeof(OptionParameter)); i++) {
        dest[i].converter = params[i].type->converter;
        dest[i].convertee = relative_argv[i];
    }

    dest[i] = (ConverterWithInput) {0};
}

CommandIteration getLongOptionReturn(CommandIterator *const iterator, char *arg) {
    arg += 2;

    // Look up the option from the one provided. This might also benefit from a hash map, bc
    // performing a linear search over this makes me feel sad.
    CommandOption *const option_array = iterator->command->options;

    // Looping through all options, and their long flags inside each option (a single
    // option can have multiple names/flags)
    for(u8f i = 0; i < iterator->command->options_count; i++) {
        for(char **option_text = option_array[i].long_options; *option_text; option_text++) {
            if(!strcmp(*option_text, arg)) {
                CommandIteration returned;
                returned.status = COMMAND_ITER_PARAMETER;
                returned.id = option_array[i].id;
                collectArgConverters(returned.converters, option_array[i].parameters, iterator->remaining_argv);

                return returned;
            }
        }
    }

    // If we're running code here, that means the flag passed is invalid.
    return FULL_CMD_ITER_NONE;
}

// I think this function is wrong from the start. We need to loop though options *inside* each flag,
// not the other way around...
CommandIteration getShortOptionReturn(const CommandIterator *const iterator, const char tested_flag) {
    // iterate through options
    // NOTE: I will 100% have to make this into some kind of array for quick lookup.
    const CommandOption *current_option = iterator->command->options;
    for(; memcmp(current_option, &NULL_OPTION, sizeof(&current_option)); current_option++) { 
        fputs("ayo we looping here\n", stderr);
        // iterate through individual option flags
        for (
            const char *known_flag = current_option->short_options;
            *known_flag;
            known_flag++
        ) {
            fprintf(stderr, "guhhh k:%c t:%c\n", *known_flag, tested_flag);

            if(*known_flag == tested_flag) {
                fputs("passed!?!?\n", stderr);
                CommandIteration returned;
                returned.status = COMMAND_ITER_PARAMETER;
                returned.id = current_option->id;
                collectArgConverters (
                    returned.converters, current_option->parameters, iterator->remaining_argv + 1
                );
                fputs("nothing happened yet dfggbfdgbdf!?!?\n", stderr);
                fprintf(stderr, "%s meow\n", returned.converters[0].convertee);
                fputs("BRUH BRUH BRUH\n", stderr);

                // I may have to mutate the remaining_argv pointer by this point.

                return returned;
            }
        }
    }

    fputs("what\n", stderr);

    // If no match for a flag occurred, then return with none and set an error. TODO: add an error
    // data type to set and return.
    return FULL_CMD_ITER_NONE;
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

    if(iterator->current_short_option) {
        // NOTE: I will 100% have to make this into some kind of array for quick lookup.
        CommandIteration maybe_returned = getShortOptionReturn (
            iterator, *iterator->current_short_option
        );

        if(maybe_returned.status == COMMAND_ITER_NONE) {panic(69, "Placeholder error\n");} // TODO: add error handling!

        if(!*(++iterator->current_short_option)) {
            iterator->current_short_option = NULL;
            iterator->remaining_argv++;
            iterator->remaining_argc--;
        }

        return maybe_returned;
    }

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
            }
            case ARGPARSE_OPT_SHORT:
                fputs("when you\n", stderr);
                // Iterate through all options and their short options to find a matching char. This
                // also could theoretically benefit from a hash map.

                iterator->current_short_option = *iterator->remaining_argv + 1;
                // NOTE: I will 100% have to make this into some kind of array for quick lookup.
                CommandIteration maybe_returned = getShortOptionReturn (
                    iterator, *iterator->current_short_option
                );

                if(maybe_returned.status == COMMAND_ITER_NONE) {panic(69, "Placeholder error\n");} // TODO: add error handling!

                // Setting this to NULL so the code will go down the right path on next iteration
                // We should likely also increment 
                if(!*(++iterator->current_short_option)) {
                    iterator->current_short_option = NULL;
                    iterator->remaining_argv++;
                    iterator->remaining_argc--;
                }

                return maybe_returned;

                break;
            case ARGPARSE_OPT_LONG:
                // If the long option is just a "--", then all other arguments passed will be
                // positional arguments. Shove all the args into the positional arg array and
                // return.
                if((*iterator->remaining_argv)[2] == '\0') {
                    // memmove() the remaining args passed into the positional_args array
                    iterator->remaining_argv++;
                    iterator->remaining_argc--;

                    memmove (
                        iterator->positional_argv + iterator->positional_argc,
                        iterator->remaining_argv,
                        iterator->remaining_argc * sizeof(iterator->positional_argv[0])
                    );

                    iterator->positional_argc += iterator->remaining_argc;
                    iterator->positional_argv[iterator->positional_argc] = NULL;

                    iterator->remaining_argc = 0;
                    return FULL_CMD_ITER_DONE;
                }

                CommandIteration returned = getLongOptionReturn(iterator, *iterator->remaining_argv);
                iterator->remaining_argv++;
                iterator->remaining_argc--;

                // On an error, we should not return this, but instead silently add to our error
                // object, and then return when we get to the next option, or when we hit the end of
                // our parameters, whatever is first. TODO: Implement error handling here.
                return returned;
        }
    }

    // MULL-terminating the new positional list
    iterator->positional_argv[iterator->positional_argc] = NULL;
    return FULL_CMD_ITER_DONE;
}

// TODO: Add tests

