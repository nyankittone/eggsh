#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#ifdef RUN_TESTS
    #include <check.h>
#endif

#include <tokenizer.h>

static Tokenizer *reallocTokenizer(Tokenizer *const tokenizer) {
    assert(tokenizer != NULL);

    tokenizer->arena_ptr = reallocOrDie (
        tokenizer->arena_ptr,
        tokenizer->tokens.capacity
    );

    // TODO: memmove all the major parts of the memory arena as to avoid stupid problems.
    // (important)

    tokenizer->tokens.ptr = (char*) tokenizer->arena_ptr;
    return tokenizer;
}

Tokenizer newTokenizer(void) {
    Tokenizer returned = {0};
    returned.tokens.capacity = INITIAL_TOKENS_CAPACITY;
    returned.tokens.bytes_written = 0;

    reallocTokenizer(&returned);
    return returned;
}

Tokenizer *nukeTokenizer(Tokenizer *const tokenizer) {
    assert(tokenizer != NULL);
    if(tokenizer->arena_ptr) free(tokenizer->arena_ptr);

    *tokenizer = (Tokenizer) {0};
    tokenizer->tokens = (TokenInfo) {0}; // idk if this line is even nessesary??? It's just here
                                       // in case tokens doesn't get zero-filled by the above
                                       // line.

    return tokenizer;
}

static Tokenizer *addToToken(Tokenizer *const tokenizer, const char *const string, size_t length) {
    assert(tokenizer != NULL);

    // check if there will be a buffer overflow, and if so, re-allocate the array.
    if(tokenizer->tokens.bytes_written + length > tokenizer->tokens.capacity) {
        tokenizer->tokens.capacity *= VECTOR_GROW_MULTIPLIER;
        reallocTokenizer(tokenizer);
    }

    // append the string data in question, nyaaah~
    memcpy(tokenizer->tokens.ptr + tokenizer->tokens.bytes_written, string, length);
    tokenizer->tokens.bytes_written += length;

    return tokenizer;
}

// Do I need some other function like this for creating the first word? I don't think so...
static Tokenizer *newToken(Tokenizer *const tokenizer) {
    addToToken(tokenizer, "", 1);
    tokenizer->total_tokens++;

    return tokenizer;
}

Tokenizer *setTokenizerInput(Tokenizer *tokenizer, char *string, size_t length) {
    assert(tokenizer != NULL && string != NULL);
    tokenizer->remaining = string;
    tokenizer->lagged_remaining = string;
    tokenizer->remaining_length = length;

    return tokenizer;
}

#define INCRIMENT_REMAINING \
    tokenizer->remaining++; \
    if(!(--tokenizer->remaining_length)) return returned | PARSE_COMMAND_OUT_OF_DATA;

static TokenizeCommandReturn handleSingleQuotes(Tokenizer *const tokenizer) {
    TokenizeCommandReturn returned = PARSE_COMMAND_NORMAL;

    // Scan through until we hit a newline, buffer end, or another single quote.
    // We will need to add stuff for handling when a command spans multiple lines after this.
    while(true) {
        switch(*tokenizer->remaining) {
            case '\0':
                addToToken(tokenizer, tokenizer->lagged_remaining, tokenizer->remaining - tokenizer->lagged_remaining);
                INCRIMENT_REMAINING
                tokenizer->lagged_remaining = tokenizer->remaining;

                continue;
            case '\n':
                addToToken(tokenizer, tokenizer->lagged_remaining, tokenizer->remaining - tokenizer->lagged_remaining + 1);
                returned |= PARSE_COMMAND_HIT_NEWLINE;
                INCRIMENT_REMAINING
                tokenizer->lagged_remaining = tokenizer->remaining;

                return returned;
            case '\'':
            case '"':
                tokenizer->inside_single_quotes = false;

                addToToken(tokenizer, tokenizer->lagged_remaining, tokenizer->remaining - tokenizer->lagged_remaining);
                INCRIMENT_REMAINING
                tokenizer->lagged_remaining = tokenizer->remaining;

                return PARSE_COMMAND_NORMAL;
        }

        tokenizer->remaining++;
        if(!(--tokenizer->remaining_length)) {
            addToToken(tokenizer, tokenizer->lagged_remaining, tokenizer->remaining - tokenizer->lagged_remaining);
            return returned | PARSE_COMMAND_OUT_OF_DATA;
        }

        continue;

        break;
    }

    return returned;
}

TokenizeCommandReturn tokenizeBuilderInput(Tokenizer *const tokenizer) {
    assert(tokenizer != NULL);
    if(tokenizer->remaining_length == 0) return PARSE_COMMAND_OUT_OF_DATA;

    TokenizeCommandReturn returned = PARSE_COMMAND_NORMAL;

    // NOTE: `tokenizer->remaining` and `tokenizer->lagged_remaining` are char pointers. They do
    // *not* represent the number of bytes remaining until the end of the buffer we're going
    // through; that would be `tokenizer->remaining_length`.

    // If we're going back to running this function more than one time before the command finishes,
    // and we're in the middle of some whitespace, we'll scan through until we encounter a
    // non-whitespace character or see a line break.
    if(!tokenizer->scanning_word) {
        while(true) {
            switch(*tokenizer->remaining) {
                case '\0':
                case ' ':
                case '\t':
                    INCRIMENT_REMAINING
                    continue;
                case '\n':
                    returned |= PARSE_COMMAND_HIT_NEWLINE | PARSE_COMMAND_COMMAND_STOP;
                    INCRIMENT_REMAINING
                    return returned;
            }

            break;
        }

        tokenizer->scanning_word = true;
    }

    // this condition seems sus and I can't explain why...
    if(tokenizer->inside_single_quotes && (returned |= handleSingleQuotes(tokenizer))) {
        return returned;
    }

    // Syncing lagged_remaining with remaining
    tokenizer->lagged_remaining = tokenizer->remaining;

    // Looping over the remaining part of our input, starting at a non-whitespace character.
    // The loop has two loops for handling a word and handling whitespace respectively.
    while(true) {
        while(true) {
            switch(*tokenizer->remaining) {
                case '\0':
                    addToToken(tokenizer, tokenizer->lagged_remaining, tokenizer->remaining - tokenizer->lagged_remaining);
                    INCRIMENT_REMAINING
                    tokenizer->lagged_remaining = tokenizer->remaining;

                    continue;
                case '\n':
                    addToToken(tokenizer, tokenizer->lagged_remaining, tokenizer->remaining - tokenizer->lagged_remaining);
                    newToken(tokenizer);
                    returned |= PARSE_COMMAND_HIT_NEWLINE | PARSE_COMMAND_COMMAND_STOP;
                    INCRIMENT_REMAINING
                    tokenizer->lagged_remaining = tokenizer->remaining;
                    return returned;
                case '\'':
                case '"':
                    // call a function for handling single quotes
                    // return whatever status was returned by that function unless it's
                    // PARSE_COMMAND_NORMAL
                    tokenizer->inside_single_quotes = true;
                    
                    addToToken(tokenizer, tokenizer->lagged_remaining, tokenizer->remaining - tokenizer->lagged_remaining);
                    INCRIMENT_REMAINING
                    tokenizer->lagged_remaining = tokenizer->remaining;

                    if((returned |= handleSingleQuotes(tokenizer))) {
                        return returned;
                    }

                    continue;
            }

            // moving to the next character as long as we aren't on whitespace.
            if(*tokenizer->remaining != ' ' && *tokenizer->remaining != '\t') {
                // Add GOOD logic to add characters here
                // ^ "what the fuck does the above comment mean?" - me, 4 months after writing that

                // INCRIMENT_REMAINING macro couldn't be used here, since here we need to add token
                // data before returning.
                tokenizer->remaining++;
                if(!(--tokenizer->remaining_length)) {
                    addToToken(tokenizer, tokenizer->lagged_remaining, tokenizer->remaining - tokenizer->lagged_remaining);
                    return returned | PARSE_COMMAND_OUT_OF_DATA;
                }

                continue;
            }

            break;
        }

        tokenizer->scanning_word = false;
        addToToken(tokenizer, tokenizer->lagged_remaining, tokenizer->remaining - tokenizer->lagged_remaining);
        newToken(tokenizer);

        INCRIMENT_REMAINING
        tokenizer->lagged_remaining = tokenizer->remaining; // Another sync with lagged_remaining
                                                            // here

        // TODO: This loop here is identical to the other one at the beginning of this function. I
        // should extract this out to another function in this file, and maybe make it inline.
        while(true) {
            switch(*tokenizer->remaining) {
                case '\0':
                case ' ':
                case '\t':
                    INCRIMENT_REMAINING
                    continue;
                case '\n':
                    returned |= PARSE_COMMAND_HIT_NEWLINE | PARSE_COMMAND_COMMAND_STOP;
                    INCRIMENT_REMAINING
                    return returned;
            }

            break;
        }

        tokenizer->scanning_word = true;
        tokenizer->lagged_remaining = tokenizer->remaining;
    }

    return PARSE_COMMAND_NORMAL;

}

#undef INCRIMENT_REMAINING

TokenIterator getTokenIterator(Tokenizer *const tokenizer) {
    assert(tokenizer != NULL);


    return (TokenIterator) {
        .token_ptr = tokenizer->tokens.ptr,
        .tokens_remaining = tokenizer->total_tokens,
    };
}

char *nextToken(TokenIterator *const iterator) {
    assert(iterator != NULL);
    if(!iterator->tokens_remaining) return NULL;

    char *const returned = iterator->token_ptr;
    iterator->token_ptr += strlen(iterator->token_ptr) + 1;
    iterator->tokens_remaining--;

    return returned;
}

char **pasteRemainingTokens(TokenIterator *const iterator, char **destination) {
    assert(iterator != NULL && destination != NULL);

    for(; iterator->tokens_remaining; iterator->tokens_remaining--) {
        *(destination++) = iterator->token_ptr;
        iterator->token_ptr += strlen(iterator->token_ptr) + 1;
    }

    return destination;
}

Tokenizer *newCommand(Tokenizer *const tokenizer) {
    assert(tokenizer != NULL);

    // TODO: Have this function handle shrinking the memory arena if the vibes for its size are
    // off.
    tokenizer->total_tokens = 0;
    tokenizer->tokens.bytes_written = 0;
    tokenizer->scanning_word = false;

    return tokenizer;
}

#ifdef RUN_TESTS

#include <testing/tokenizer_runner.h>

START_TEST(token_test_one) {
    Tokenizer tokenizer = tests_gimmeTokenizer("hewwo\n");
    tests_assertTokens(&tokenizer, "hewwo", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_one_broken) {
    static char input[] = "meow";

    Tokenizer cmd = newTokenizer();
    tokenizeBuilderInput(setTokenizerInput(&cmd, input, sizeof(input) - 1));

    ck_assert_uint_eq(cmd.total_tokens, 0);
} END_TEST

START_TEST(token_test_many) {
    Tokenizer tokenizer = tests_gimmeTokenizer("echo Hello, world!\n");
    tests_assertTokens(&tokenizer, "echo", "Hello,", "world!", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_null_bytes) {
    static char input[] = "ech\0o He\0\0ya! :""\0""3\n";

    Tokenizer tokenizer = tests_gimmeTokenizerWithLength(input, sizeof(input) - 1);
    tests_assertTokens(&tokenizer, "echo", "Heya!", ":3", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_extra_space) {
    Tokenizer tokenizer = tests_gimmeTokenizer("echo    meow\n");
    tests_assertTokens(&tokenizer, "echo", "meow", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_two_commands) {
    Tokenizer tokenizer = tests_gimmeTokenizer("echo AAA\necho BBB CCC\n");
    tests_assertTokens(&tokenizer, "echo", "AAA", NULL);
    tests_assertTokens(&tokenizer, "echo", "BBB", "CCC", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_buffer_swap) {
    Tokenizer tokenizer = tests_gimmeTokenizer("echo AAA\n");
    tests_assertTokens(&tokenizer, "echo", "AAA", NULL);
    setTokenizerInput(&tokenizer, "echo BBB CCC\n", sizeof("echo BBB CCC\n") - 1);
    tests_assertTokens(&tokenizer, "echo", "BBB", "CCC", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_two_inputs) {
    Tokenizer tokenizer = tests_gimmeTokenizer("echo AAA");
    tests_assertIncompleteCommand(&tokenizer);
    setTokenizerInput(&tokenizer, " BBB CCC\n", sizeof(" BBB CCC\n") - 1);
    tests_assertTokensNoReset(&tokenizer, "echo", "AAA", "BBB", "CCC", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_leading_whitespace) {
    Tokenizer tokenizer = tests_gimmeTokenizer("          \t     bruh\n");
    tests_assertTokens(&tokenizer, "bruh", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_iterator) {
    static char input[] = "meow woof bark bark nyaaaah\n";
    Tokenizer cmd = newTokenizer();
    tokenizeBuilderInput(setTokenizerInput(&cmd, input, sizeof(input) - 1));

    TokenIterator iterator = getTokenIterator(&cmd);
    ck_assert_str_eq(nextToken(&iterator), "meow");
    ck_assert_str_eq(nextToken(&iterator), "woof");
    ck_assert_str_eq(nextToken(&iterator), "bark");
    ck_assert_str_eq(nextToken(&iterator), "bark");
    ck_assert_str_eq(nextToken(&iterator), "nyaaaah");
    ck_assert_ptr_null(nextToken(&iterator));
    ck_assert_ptr_null(nextToken(&iterator));
    ck_assert_ptr_null(nextToken(&iterator));
} END_TEST

START_TEST(token_test_iterator_filling) {
    static char input[] = "oooh eee oooh ah ah tang tang walla walla bing bang\n";
    Tokenizer cmd = newTokenizer();
    tokenizeBuilderInput(setTokenizerInput(&cmd, input, sizeof(input) - 1));

    TokenIterator iterator = getTokenIterator(&cmd);
    char *result[iterator.tokens_remaining + 1];
    *pasteRemainingTokens(&iterator, result) = NULL;

    ck_assert_str_eq(result[0], "oooh");
    ck_assert_str_eq(result[1], "eee");
    ck_assert_str_eq(result[2], "oooh");
    ck_assert_str_eq(result[3], "ah");
    ck_assert_str_eq(result[4], "ah");
    ck_assert_str_eq(result[5], "tang");
    ck_assert_str_eq(result[6], "tang");
    ck_assert_str_eq(result[7], "walla");
    ck_assert_str_eq(result[8], "walla");
    ck_assert_str_eq(result[9], "bing");
    ck_assert_str_eq(result[10], "bang");
    ck_assert_ptr_null(result[11]);
} END_TEST

START_TEST(token_test_single_quote) {
    Tokenizer tokenizer = tests_gimmeTokenizer("Hello, 'world!'\n");
    tests_assertTokens(&tokenizer, "Hello,", "world!", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_single_quote_spam) {
    Tokenizer tokenizer = tests_gimmeTokenizer("Hello, 'w''orld!' ha'h'a'ha'\n");
    tests_assertTokens(&tokenizer, "Hello,", "world!", "hahaha", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_single_quote_spam_harder) {
    Tokenizer tokenizer = tests_gimmeTokenizer("He''''''''''llo, world! hahaha\n");
    tests_assertTokens(&tokenizer, "Hello,", "world!", "hahaha", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_single_quote_empty_parameter) {
    Tokenizer tokenizer = tests_gimmeTokenizer("Hi '' there!\n");
    tests_assertTokens(&tokenizer, "Hi", "", "there!", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_single_quote_all_spaces_parameter) {
    Tokenizer tokenizer = tests_gimmeTokenizer("Hi '    ' there!\n");
    tests_assertTokens(&tokenizer, "Hi", "    ", "there!", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_double_quote) {
    Tokenizer tokenizer = tests_gimmeTokenizer("Hello, \"world!\"\n");
    tests_assertTokens(&tokenizer, "Hello,", "world!", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_double_quote_spam) {
    Tokenizer tokenizer = tests_gimmeTokenizer("Hello, \"w\"\"orld!\" ha\"h\"a\"ha\"\n");
    tests_assertTokens(&tokenizer, "Hello,", "world!", "hahaha", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_double_quote_spam_harder) {
    Tokenizer tokenizer = tests_gimmeTokenizer("He\"\"\"\"\"\"\"\"\"\"llo, world! hahaha\n");
    tests_assertTokens(&tokenizer, "Hello,", "world!", "hahaha", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_double_quote_empty_parameter) {
    Tokenizer tokenizer = tests_gimmeTokenizer("Hi \"\" there!\n");
    tests_assertTokens(&tokenizer, "Hi", "", "there!", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_double_quote_all_spaces_parameter) {
    Tokenizer tokenizer = tests_gimmeTokenizer("Hi \"    \" there!\n");
    tests_assertTokens(&tokenizer, "Hi", "    ", "there!", NULL);
    tests_assertNoCommand(&tokenizer);
} END_TEST

START_TEST(token_test_quote_newlines) {
    Tokenizer tokenizer = tests_gimmeTokenizer("never gonna 'give\nyou\nup'\n");
    tests_assertTokens(&tokenizer, "never", "gonna", "give\nyou\nup", NULL);
    tests_assertNoCommand(&tokenizer);
}

Suite *tests_tokenizerSuite(void) {
    Suite *returned;
    TCase *test_case_core;

    returned = suite_create("Tokenizer");
    test_case_core = tcase_create("Tokenizer Integration");

    tcase_add_test(test_case_core, token_test_one);
    tcase_add_test(test_case_core, token_test_one_broken);
    tcase_add_test(test_case_core, token_test_many);
    tcase_add_test(test_case_core, token_test_null_bytes);
    tcase_add_test(test_case_core, token_test_extra_space);
    tcase_add_test(test_case_core, token_test_two_commands);
    tcase_add_test(test_case_core, token_test_buffer_swap);
    tcase_add_test(test_case_core, token_test_two_inputs);
    tcase_add_test(test_case_core, token_test_leading_whitespace);
    tcase_add_test(test_case_core, token_test_iterator);
    tcase_add_test(test_case_core, token_test_iterator_filling);
    tcase_add_test(test_case_core, token_test_single_quote);
    tcase_add_test(test_case_core, token_test_single_quote_spam);
    tcase_add_test(test_case_core, token_test_single_quote_spam_harder);
    tcase_add_test(test_case_core, token_test_single_quote_empty_parameter);
    tcase_add_test(test_case_core, token_test_single_quote_all_spaces_parameter);
    tcase_add_test(test_case_core, token_test_quote_newlines);
    tcase_add_test(test_case_core, token_test_double_quote);
    tcase_add_test(test_case_core, token_test_double_quote_spam);
    tcase_add_test(test_case_core, token_test_double_quote_spam_harder);
    tcase_add_test(test_case_core, token_test_double_quote_empty_parameter);
    tcase_add_test(test_case_core, token_test_double_quote_all_spaces_parameter);

    suite_add_tcase(returned, test_case_core);

    return returned;
}
#endif

