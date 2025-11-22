#pragma once

#include <tokenizer.h>
#include <stdbool.h>

Tokenizer tests_gimmeTokenizer(char *const string);

// This function is used to paste a single group of tokens from a string of text representing a
// command. Each invocation will return the tokens of the nest command in the string yet to be
// processed.
bool tests_tokenizeCommand(const char **destination, Tokenizer *const tokenizer);

void tests_assertStrings(char **array, size_t length, ...);

