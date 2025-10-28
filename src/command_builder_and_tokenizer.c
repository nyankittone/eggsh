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

TokenizeCommandReturn tokenizeBuilderInput(Tokenizer *const tokenizer) {
    assert(tokenizer != NULL);
    if(tokenizer->remaining_length == 0) return PARSE_COMMAND_OUT_OF_DATA;

    TokenizeCommandReturn returned = PARSE_COMMAND_NORMAL;

    #define INCRIMENT_REMAINING \
        tokenizer->remaining++; \
        if(!(--tokenizer->remaining_length)) return returned | PARSE_COMMAND_OUT_OF_DATA;

    // scan through remaining segment of remaining byte-by-byte.
    if(!tokenizer->scanning_word) {
        while(true) {
            switch(*tokenizer->remaining) {
                case '\0':
                case ' ':
                case '\t':
                    INCRIMENT_REMAINING
                    continue;
                case '\n':
                    returned |= PARSE_COMMAND_HIT_NEWLINE;
                    INCRIMENT_REMAINING
                    return returned;
            }

            break;
        }

        tokenizer->scanning_word = true;
    }

    tokenizer->lagged_remaining = tokenizer->remaining;

    while(true) {
        while(true) {
            switch(*tokenizer->remaining) {
                case '\0':
                    addToToken(tokenizer, tokenizer->lagged_remaining, tokenizer->remaining - tokenizer->lagged_remaining);
                    INCRIMENT_REMAINING
                    tokenizer->lagged_remaining = tokenizer->remaining;

                    continue;
                case '\n':
                    // Add logic to add characters here
                    addToToken(tokenizer, tokenizer->lagged_remaining, tokenizer->remaining - tokenizer->lagged_remaining);
                    newToken(tokenizer);
                    returned |= PARSE_COMMAND_HIT_NEWLINE;
                    INCRIMENT_REMAINING
                    tokenizer->lagged_remaining = tokenizer->remaining;
                    return returned;
            }

            if(*tokenizer->remaining != ' ' && *tokenizer->remaining != '\t') {
                // Add GOOD logic to add characters here

                // INCRIMENT_REMAINING macro couldn't be used here. This mightr be a sign I should
                // make a refactor here. Or maybe all of the tokenizer code. Who knows?
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
        tokenizer->lagged_remaining = tokenizer->remaining;

        while(true) {
            switch(*tokenizer->remaining) {
                case '\0':
                case ' ':
                case '\t':
                    INCRIMENT_REMAINING
                    continue;
                case '\n':
                    returned |= PARSE_COMMAND_HIT_NEWLINE;
                    INCRIMENT_REMAINING
                    return returned;
            }

            break;
        }

        tokenizer->scanning_word = true;
        tokenizer->lagged_remaining = tokenizer->remaining;
    }

    return PARSE_COMMAND_NORMAL;

    #undef INCRIMENT_REMAINING
}

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

    return tokenizer;
}

#ifdef RUN_TESTS
START_TEST(token_test_one) {
    static char input[] = "hewwo\n";

    Tokenizer cmd = newTokenizer();
    tokenizeBuilderInput(setTokenizerInput(&cmd, input, sizeof(input) - 1));

    ck_assert_uint_eq(cmd.total_tokens, 1);
    ck_assert_str_eq(cmd.tokens.ptr, "hewwo");
} END_TEST

START_TEST(token_test_one_broken) {
    static char input[] = "meow";

    Tokenizer cmd = newTokenizer();
    tokenizeBuilderInput(setTokenizerInput(&cmd, input, sizeof(input) - 1));

    ck_assert_uint_eq(cmd.total_tokens, 0);
} END_TEST

START_TEST(token_test_many) {
    static char input[] = "echo Hello, world!\n";

    Tokenizer cmd = newTokenizer();
    tokenizeBuilderInput(setTokenizerInput(&cmd, input, sizeof(input) - 1));

    ck_assert_uint_eq(cmd.total_tokens, 3);

    const char *string1 = cmd.tokens.ptr;
    const char *string2 = string1 + strlen(string1) + 1;
    const char *string3 = string2 + strlen(string2) + 1;

    ck_assert_str_eq(string1, "echo");
    ck_assert_str_eq(string2, "Hello,");
    ck_assert_str_eq(string3, "world!");
} END_TEST

START_TEST(token_test_null_bytes) {
    static char input[] = "ech\0o He\0\0ya! :""\0""3\n";

    Tokenizer cmd = newTokenizer();
    tokenizeBuilderInput(setTokenizerInput(&cmd, input, sizeof(input) - 1));

    ck_assert_uint_eq(cmd.total_tokens, 3);

    const char *string1 = cmd.tokens.ptr;
    const char *string2 = string1 + strlen(string1) + 1;
    const char *string3 = string2 + strlen(string2) + 1;

    ck_assert_str_eq(string1, "echo");
    ck_assert_str_eq(string2, "Heya!");
    ck_assert_str_eq(string3, ":3");
} END_TEST

START_TEST(token_test_extra_space) {
    static char input[] = "echo    meow\n";

    Tokenizer cmd = newTokenizer();
    tokenizeBuilderInput(setTokenizerInput(&cmd, input, sizeof(input) - 1));

    ck_assert_uint_eq(cmd.total_tokens, 2);

    const char *string1 = cmd.tokens.ptr;
    const char *string2 = string1 + strlen(string1) + 1;

    ck_assert_str_eq(string1, "echo");
    ck_assert_str_eq(string2, "meow");
} END_TEST

START_TEST(token_test_two_commands) {
    static char input1[] = "echo AAA\n";
    static char input2[] = "echo BBB CCC\n";

    Tokenizer cmd = newTokenizer();
    tokenizeBuilderInput(setTokenizerInput(&cmd, input1, sizeof(input1) - 1));

    ck_assert_uint_eq(cmd.total_tokens, 2);

    const char *string1 = cmd.tokens.ptr;
    const char *string2 = string1 + strlen(string1) + 1;

    ck_assert_str_eq(string1, "echo");
    ck_assert_str_eq(string2, "AAA");

    tokenizeBuilderInput(setTokenizerInput(newCommand(&cmd), input2, sizeof(input2) - 1));

    ck_assert_uint_eq(cmd.total_tokens, 3);

    string1 = cmd.tokens.ptr;
    string2 = string1 + strlen(string1) + 1;
    const char *string3 = string2 + strlen(string2) + 1;

    ck_assert_str_eq(string1, "echo");
    ck_assert_str_eq(string2, "BBB");
    ck_assert_str_eq(string3, "CCC");
} END_TEST

START_TEST(token_test_two_inputs) {
    static char input1[] = "echo AAA";
    static char input2[] = " BBB CCC\n";

    Tokenizer cmd = newTokenizer();
    TokenizeCommandReturn code = tokenizeBuilderInput(setTokenizerInput(&cmd, input1, sizeof(input1) - 1));
    ck_assert_uint_eq(code, PARSE_COMMAND_OUT_OF_DATA);

    code = tokenizeBuilderInput(setTokenizerInput(&cmd, input2, sizeof(input2) - 1));
    ck_assert_uint_eq(code, PARSE_COMMAND_OUT_OF_DATA | PARSE_COMMAND_HIT_NEWLINE);

    ck_assert_uint_eq(cmd.total_tokens, 4);

    const char *string1 = cmd.tokens.ptr;
    const char *string2 = string1 + strlen(string1) + 1;
    const char *string3 = string2 + strlen(string2) + 1;
    const char *string4 = string3 + strlen(string3) + 1;

    ck_assert_str_eq(string1, "echo");
    ck_assert_str_eq(string2, "AAA");
    ck_assert_str_eq(string3, "BBB");
    ck_assert_str_eq(string4, "CCC");
} END_TEST

START_TEST(token_test_leading_whitespace) {
    static char input[] = "          \t     bruh\n";

    Tokenizer cmd = newTokenizer();
    tokenizeBuilderInput(setTokenizerInput(&cmd, input, sizeof(input) - 1));

    ck_assert_uint_eq(cmd.total_tokens, 1);
    ck_assert_str_eq(cmd.tokens.ptr, "bruh");
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

Suite *tests_tokenizerSuite(void) {
    Suite *returned;
    TCase *test_case_core;

    returned = suite_create("Tokenizer");
    test_case_core = tcase_create("Integration");

    tcase_add_test(test_case_core, token_test_one);
    tcase_add_test(test_case_core, token_test_one_broken);
    tcase_add_test(test_case_core, token_test_many);
    tcase_add_test(test_case_core, token_test_null_bytes);
    tcase_add_test(test_case_core, token_test_extra_space);
    tcase_add_test(test_case_core, token_test_two_commands);
    tcase_add_test(test_case_core, token_test_two_inputs);
    tcase_add_test(test_case_core, token_test_leading_whitespace);
    tcase_add_test(test_case_core, token_test_iterator);
    tcase_add_test(test_case_core, token_test_iterator_filling);

    suite_add_tcase(returned, test_case_core);

    return returned;
}
#endif

