#include <tokenizer.h>
#include <assert.h>
#include <stdarg.h>

#include <check.h>

#include <string.h>
#include <testing/tokenizer_runner.h>

Tokenizer tests_gimmeTokenizer(char *const string) {
    Tokenizer returned = newTokenizer();
    setTokenizerInput(&returned, string, strlen(string));
    return returned;
}

Tokenizer tests_gimmeTokenizerWithLength(char *const string, const size_t length) {
    Tokenizer returned = newTokenizer();
    setTokenizerInput(&returned, string, length);
    return returned;
}

void tests_assertTokensNoReset(Tokenizer *const tokenizer, ...) {
    // In a loop, tokenize
    // Keep doing this until a command stop or an out-of-data occurs

    assert(tokenizer != NULL);

    TokenizeCommandReturn result;
    do {
        result = tokenizeBuilderInput(tokenizer);
        if(result == PARSE_COMMAND_OUT_OF_DATA) {
            ck_abort_msg("Unexpected premature OUT_OF_DATA");
        }
    } while(!(result & PARSE_COMMAND_COMMAND_STOP));

    // Fetch tokens found in a loop
    va_list token_case;
    va_start(token_case, tokenizer);

    char *token = tokenizer->tokens.ptr;
    for(size_t i = 0; i < tokenizer->total_tokens; i++) {
        // if any token fails to match, that's a fail.
        // if i ever exceeds the token_count, that's a fail.
        // if our va spits out NULL before we hit token_count, that's a fail.
        char *cmp_token = va_arg(token_case, char*);
        ck_assert_ptr_nonnull(cmp_token);
        ck_assert_str_eq(token, cmp_token);

        token += strlen(token) + 1;
    }

    ck_assert_ptr_null(va_arg(token_case, char*));
    va_end(token_case);
}

void tests_assertTokens(Tokenizer *const tokenizer, ...) {
    assert(tokenizer != NULL);
    newCommand(tokenizer);
    tests_assertTokensNoReset(tokenizer);
}

void tests_assertIncompleteCommand(Tokenizer *const tokenizer) {
    assert(tokenizer != NULL);

    TokenizeCommandReturn result;
    do {
        result = tokenizeBuilderInput(tokenizer);
        if(result & PARSE_COMMAND_COMMAND_STOP) {
            ck_abort_msg("Unexpected compeleted command after last one specified");
        }

    } while(!(result & PARSE_COMMAND_OUT_OF_DATA));
}

void tests_assertNoCommand(Tokenizer *const tokenizer) {
    assert(tokenizer != NULL);
    ck_assert_uint_eq(tokenizer->remaining_length, 0);
    const char *const old_remaining = tokenizer->remaining;
    tokenizeBuilderInput(tokenizer);
    ck_assert_pstr_eq(old_remaining, tokenizer->remaining);
}

