#include <stddef.h>
#include <assert.h>

#include <hash_map.h>

HashMap newHashMap(KeyValuePair *const array, u32 array_size, u32 *const final_array_size) {
    assert(array != NULL && array_size > 0);
    HashMap returned = {.keyvals_ptr = array};

    // Need to create a bitmask for the final array size.
    // Final array size should the biggest power of two that can "fit" inside the passed in
    // array size.

    // I dunno if this bullshit bitshifting code will work tbh....
    u32 bitmask = ~0;
    for(; array_size; array_size >>= 1, bitmask <<= 1);
    bitmask = (~bitmask) >> 1;
    returned.bitmask = bitmask;

    return returned;
}

