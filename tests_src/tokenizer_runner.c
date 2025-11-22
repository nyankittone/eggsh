#include <assert.h>

#include <string.h>
#include <testing/tokenizer_runner.h>

// I want this to automatically add a newline to our string. We're doing this by making a new string
// on the heap. It's slow, but should be the most robust way of doing this, I think.
// I should *probably* have the string not be made here to prevent memory leaks, but that shouldn't
// actually matter since the tests run in a fork of the test suite, not the test suite itself.
// The biggest problem with doing it this way is that my autistic ass will get upset that I'm
// writing this "incorrectly".
Tokenizer tests_gimmeTokenizer(char *const string) {
    size_t string_length = strlen(string);
    char *const new_string = malloc(string_length + 2);
    assert(new_string != NULL);

    memcpy(new_string, string, string_length);
    new_string[string_length] = '\n';
    new_string[string_length + 1] = '\0';

    Tokenizer returned = newTokenizer();
    setTokenizerInput(&returned, new_string, string_length + 2);

    return returned;
}

bool tests_tokenizeCommand(const char **destination, Tokenizer *const tokenizer) {
    return true;
    // Tokenizer tokenizer = newTokenizer();
    // setTokenizerInput(&tokenizer, source, strlen(source));
    //
    // while(true) {
    //     TokenizeCommandReturn result = tokenizeBuilderInput(&tokenizer);
    //     if(result & (PARSE_COMMAND_HIT_NEWLINE)) {
    //         if(!(result & PARSE_COMMAND_COMMAND_STOP)) {
    //             continue;
    //         }
    //
    //         TokenIterator token_iterator = getTokenIterator(&tokenizer);
    //
    //         executeCommand(runner, &token_iterator);
    //         newCommand(&tokenizer);
    //     }
    //
    //     if(result & PARSE_COMMAND_OUT_OF_DATA) {
    //         break;
    //     }
    // }
    //
    // nukeTokenizer(&tokenizer);
}

