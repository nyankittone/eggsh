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

    bool scanning_word; // Internal flag used by the tokenizer to see if we're in the middle of
                        // a word right now or not. Keeping this state is important so that it can
                        // persist between many calls to `tokenizeBuilderInput`.
    bool inside_single_quotes;
    bool inside_double_quotes; // TODO: Consider replacing all of these boolean flags with a single
                               // long that we bitmask on to get flags. That might be slower,
                               // though.
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

// This data type is returned by the `tokenizeBuilderInput` function, and acts as a status code for
// the function on why it stopped running. Each bit in the code represents certain conditions, i.e.
// if a newline was hit, or we run out of bytes in the buffer provided by the parser.
typedef u8 TokenizeCommandReturn;

#define PARSE_COMMAND_NORMAL (0)

// This bit value is returned by `tokenizeBuilderInput` when the tokenizer runs out of bytes to read
// from. This indicates that the caller of that function should load more bytes of data from the
// stream it's getting stuff from, unless PARSE_COMMAND_COMMAND_STOP is also returned.
#define PARSE_COMMAND_OUT_OF_DATA (1)

// This bit value is raised when the parser hits a new line. This does not necessarily mean that a
// new command must be executed, since a command can be made up of multiple lines, but this is a
// needed bit, since when running interactively, we'll need to get the user's input for each line.
#define PARSE_COMMAND_HIT_NEWLINE (2)

// This bit value is the one used to indicate when a whole command is done being processed, and we
// can now try to run it. A TokenIterator object is needed to be used to get the tokens out of the
// tokenizer by this point.
#define PARSE_COMMAND_COMMAND_STOP (4)

// This function makes the tokenizer object actually try to get some tokens out of the current
// buffer that the tokenizer is set to use. The actual strings for the tokens are stored inside its
// own little data structure that, once ready, can be revealed using the `TokenIterator` object.
// This function will return a code that indicates a variety of things, such as if it;s run through
// the buffer it was provided and needs more data, it's finished parsing through a command and needs
// something to execute it, if it's hit a newline character and needs the caller to give it another
// line of input, and so on. Code that calls this function will need to properly handle the values
// that can be returned from it for whatever the caller is trying to do; look at the
// `PARSE_COMMAND_{xyz}` macros to see the bits that can be returned.
TokenizeCommandReturn tokenizeBuilderInput(Tokenizer *const tokenizer);

// To actually use the data tokenized by the tokenizer, we have this data structure defined for
// getting the tokens one-by-one. Each iteration of this iterator returns the pointer to the next
// token that was found, and this continues until it runs out of tokens.
// This data structure's exact purpose is likely to change in the future as this tokenizer grows
// into being a full-blown parser. I may instead need an iterator of sorts for iterating through an
// AST.
typedef struct {
    u32 tokens_remaining;
    char *token_ptr;
} TokenIterator;

// Function for spawning a new `TokenIterator` for a specified tokenizer.
TokenIterator getTokenIterator(Tokenizer *const tokenizer);

// Function for getting just one token out of a `TokenIterator`.
char *nextToken(TokenIterator *const iterator);

// This function loops over a `TokenIterator` passed in, and puts the pointers to the tokens for it
// into a buffer for an array specified.
char **pasteRemainingTokens(TokenIterator *const iterator, char **destination);

#ifdef RUN_TESTS
Suite *tests_tokenizerSuite(void);
#endif

