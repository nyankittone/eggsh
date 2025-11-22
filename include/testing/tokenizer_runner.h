#pragma once

#include <tokenizer.h>
#include <stdbool.h>

Tokenizer tests_gimmeTokenizer(char *const string);
Tokenizer tests_gimmeTokenizerWithLength(char *const string, const size_t length);

void tests_assertTokens(Tokenizer *const tokenizer, ...);
void tests_assertTokensNoReset(Tokenizer *const tokenizer, ...);
void tests_assertIncompleteCommand(Tokenizer *const tokenizer);
void tests_assertNoCommand(Tokenizer *const tokenizer);

