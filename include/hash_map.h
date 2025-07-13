#pragma once

#ifdef RUN_TESTS
    #include <check.h>
#endif

#include <rusttypes.h>

// This is my first-ever attempt at making a hash map in C. I'm doing it from scratch here instead
// of using a library for it, because I want to learn how to make a hash map good.

// TODO: Right now, I'm making this hash map so that it just duplicates the keys for the string
// using `strdup`. This is probably not very efficient. I will experiment with implementing a pool
// of memory for keeping strings in, to reduce calls to `malloc` and `free` when adding values
// to my hash map. I may also be able to do something similar for some of the values as well.

typedef struct KeyValuePair {
    char *key;
    void *value;
    struct KeyValuePair *next;
} KeyValuePair;

typedef struct {
    u32 bitmask;
    KeyValuePair *keyvals_ptr;
} HashMap;

// This function signature is possibly shit
HashMap newHashMap(KeyValuePair *const array, u32 array_size, u32 *const final_array_size);

// This function adds a value with a specified key to the given hash map. `map` and `key` should be
// non-NULL values. `value` may be NULL. If there's an entry already in the given hash map of the
// same key, it is overwritten.
HashMap *putInMap(HashMap *const map, char *const key, void *const value);


HashMap *putInMapIfUnique(HashMap *const map, char *const key, void *const value);

// This function attempts to return a pointer to the item in the given hash map with the key
// provided. Both `map` and `key` must be non-NULL values. If there's no value associated with `key`
// in the given map, NULL is returned.
void *getFromMap(HashMap *const map, const char *const key);

// this function removes a single key-value pair from the given map. Both `map` and `key` must be
// non-NULL values. Trying to remove a key that isn't in the map does not result in an error.
HashMap *removeFromMap(HashMap *const map, const char *const key);

// This function clears out all of the key-value pairs in the underlying array for the hash map,
// and also de-allocates any additional memory that was allocated by adding entries.
HashMap *wipeMap(HashMap *const map);

// Exposing function for addding a test suite for this hash map.
#ifdef RUN_TESTS
Suite *tests_hashMapSuite(void);
#endif

