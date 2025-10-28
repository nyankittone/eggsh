#pragma once

// There are plans to add a space for an abstract syntax tree to fit in the data structure, so if
// you see references to an AST, that's why...

// I also need to clean up some old function names. This thing is called `CommandBuilder`, but I
// want to start referring it to `Tokenizer` or something like that.

#include <util.h>
#include <rusttypes.h>
#include <vector_info.h>

#ifdef RUN_TESTS
    #include <check.h>
#endif

// This is how large the section in the memory arena for storing the command line tokens is
// initially, in bytes. The size is prone to growing if needed.
#define INITIAL_TOKENS_CAPACITY (4096)

// This is how large the section in the memory arena for storing the syntax tree nodes is
// initially, in how many nodes can be stored.  The size is prone to growing if needed.
#define INITIAL_TREE_CAPACITY (32)

// This struct is part of the `Tokenizer` structure, and includes data related to managing the
// segment in memory for storing the strings for tokens parsed. Each token is a null-terminated
// string.
typedef struct {
    size_t bytes_written;
    size_t capacity; // How many bytes in total can be contained in the slice of memory holding the
                     // tokens. If more is needed at any point, it is realloc()'ed.
    char *ptr; // A pointer pointing to the beginning area of the token list. The value of this
               // pointer is likely to change if the area of memory the tokens reside in ever gets
               // reallocated.
} TokenInfo;

// This struct basically is responsible for tokenizing and parsing commands, and storing information
// for doing so. It manages a memory arena containing raw string data for each token it finds, a
// syntax tree for complex commands involving e.g. pipe operations, and reserved space for building
// a command line out of the tokens in the arena that functions like `execv()` can understand.
typedef struct {
    void *arena_ptr;

    // TODO: Expand the auxiliary array here to also include the syntax tree once one is needed.
    TokenInfo tokens;

    u32 total_tokens; // TODO: move this to be in a struct within an entry of the tree

    char *lagged_remaining;
    char *remaining;
    size_t remaining_length; // This is used to refer to how many bytes we have left before the
                             // caller of the tokenizer needs to fetch it more data or quit.

    bool scanning_word;
} Tokenizer;

// Function for creating a new Tokenizer object. This function will cause the program to stop
// if allocating memory for it behind the scenes fails. This function is not to be confused with
// `newCommand`, which resets an existing tokenizer to prepare it for a new line of text.
Tokenizer newTokenizer(void);

// This function frees the underlying memory arena allocated for `builder`, and sets all values
// inside `builder` to 0. The same `builder` passed in is also returned
Tokenizer *nukeTokenizer(Tokenizer *const tokenizer);

// This function prepares `builder` to tokenize the contents of the string `string` with a length of
// `length`. Note that `string` in this case is *not* a null-terminated string as is normal in C,
// but a length-prefixed one, that may contain any number of null bytes in it. This is because the
// input data, whether coming from a terminal or from a script file, is allowed to have null bytes
// in it.
Tokenizer *setTokenizerInput(Tokenizer *tokenizer, char *string, size_t length);

// This function resets the array of tokens in the underlying memory arena for `builder`, alongside
// the syntax tree in it, effectively making way for a new command to be used by `builder`. If the
// size of the memory arena has grown to a very large size, this function will also shrink it to
// its original size.
Tokenizer *newCommand(Tokenizer *const tokenizer);

typedef u8 TokenizeCommandReturn;
#define PARSE_COMMAND_NORMAL (0)

// This bit value is returned by `tokenizeBuilderInput` when the tokenizer runs out of bytes to read
// from. This indicates that the caller of that function should load more bytes of data from the
// stream it's getting stuff from, unless PARSE_COMMAND_COMMAND_STOP is also returned.
#define PARSE_COMMAND_OUT_OF_DATA (1)

//
#define PARSE_COMMAND_HIT_NEWLINE (2)
#define PARSE_COMMAND_COMMAND_STOP (4)

TokenizeCommandReturn tokenizeBuilderInput(Tokenizer *const tokenizer);

typedef struct {
    u32 tokens_remaining;
    char *token_ptr;
} TokenIterator;

TokenIterator getTokenIterator(Tokenizer *const tokenizer);
char *nextToken(TokenIterator *const iterator);
char **pasteRemainingTokens(TokenIterator *const iterator, char **destination);

#ifdef RUN_TESTS
Suite *tests_tokenizerSuite(void);
#endif

