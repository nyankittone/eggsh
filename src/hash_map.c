// This hash map implementation is slow - I can feel it. It takes a noticable amount of time for
// the shell to start when I plugged this map into it...
// I guess I can start by not strdup()'ing every fucking key when inserting data...

#define _XOPEN_SOURCE 500

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <hash_map.h>
#include <util.h>

HashMap newHashMap(KeyValuePair *const array, u32 array_size, u32 *const final_array_size) {
    assert(array != NULL && array_size > 0);
    HashMap returned = {.keyvals_ptr = array};

    // Setting the "bitmask" value, which is used in getting the final index into the array after
    // hashing. TODO: See if the bitmask value can be set in a way that is faster than this.
    u32 bitmask = ~0;
    for(; array_size; array_size >>= 1, bitmask <<= 1);
    bitmask = (~bitmask) >> 1;
    returned.bitmask = bitmask;

    u32 staged_final_array_size = bitmask + 1;
    memset(array, 0, sizeof(*array) * staged_final_array_size);

    if(final_array_size) *final_array_size = staged_final_array_size;
    return returned;
}

// This is the hash function for my hash map. It's just SDBM. I may replace it with something more
// optimal later.
static u32 hashMeMommyUwu(const char *const key) {
    u32 returned = 0;

    for(const char *key_traveller = key; *key_traveller; key_traveller++) {
        returned = returned * 65599 + *key_traveller;
    }

    return returned;
}

static KeyValuePair *getStartPtr(HashMap *const map, const char *const key) {
    return map->keyvals_ptr + (hashMeMommyUwu(key) & map->bitmask);
}

HashMap *putInMap(HashMap *const map, char *const key, void *const value) {
    assert(map != NULL && key != NULL);
    KeyValuePair *start = getStartPtr(map, key);

    // scan everything except for the first node to see if we have key
    // if we find key, overwrite the node's value
    KeyValuePair *current = start->next,
                 *old = start;
    for(; current; old = current, current = current->next) {
        if(!strcmp(current->key, key)) {
            current->value = value;
            return map;
        }
    }

    // check the first node
    // if the first node has a NULL key in it, we can just use that right away.
    if(!start->key) {
        start->key = strdup(key);
        start->value = value;
        return map;
    }

    // if it's non-NULL, does it match our key? If so, replace the value and return.
    if(!strcmp(start->key, key)) {
        start->value = value;
        return map;
    }

    old->next = mallocOrDie(sizeof(KeyValuePair));
    old->next->key = strdup(key);
    old->next->value = value;
    return map;
}

HashMap *putInMapIfUnique(HashMap *const map, char *const key, void *const value) {
    assert(map != NULL && key != NULL);
    KeyValuePair *start = getStartPtr(map, key);

    // scan everything except for the first node to see if we have key
    // if we find key, overwrite the node's value
    KeyValuePair *current = start->next,
                 *old = start;
    for(; current; old = current, current = current->next) {
        if(!strcmp(current->key, key)) return map;
    }

    // check the first node
    // if the first node has a NULL key in it, we can just use that right away.
    if(!start->key) {
        start->key = strdup(key);
        start->value = value;
        return map;
    }

    // if it's non-NULL, does it match our key? If so, return.
    if(!strcmp(start->key, key)) return map;

    old->next = mallocOrDie(sizeof(KeyValuePair));
    old->next->key = strdup(key);
    old->next->value = value;
    return map;
}

void *getFromMap(HashMap *const map, const char *const key) {
    assert(map != NULL && key != NULL);
    const KeyValuePair *node = getStartPtr(map, key);

    if(!node->key) {
        if(!node->next) return NULL;
        node = node->next;
    }

    do {
        if(!strcmp(node->key, key)) return node->value;
        node = node->next;
    } while(node);

    return NULL;
}

HashMap *removeFromMap(HashMap *const map, const char *const key) {
    assert(map != NULL && key != NULL);
    KeyValuePair *start = getStartPtr(map, key);

    // Check first item first outside of a loop
    if(start->key && !strcmp(start->key, key)) {
        start->key = NULL;
        return map;
    }

    // Loop over the rest lmao
    KeyValuePair *old = start, *current = start->next;
    for(; current; old = current, current = current->next) {
        if(!strcmp(current->key, key)) {
            old->next = current->next;
            free(current->key);
            free(current);

            break;
        }
    }

    return map;
}

// This function destroys the linked list for a hash map specified by `head`.
static void cleanUpLinkedList(KeyValuePair *const head) {
    if(!head) return;

    KeyValuePair *current_ptr = head,
                 *next_ptr;

    do {
        next_ptr = current_ptr->next;
        free(current_ptr->key); // TODO: Remove this line when the key is no longer being strdup()'d.
        free(current_ptr);
        current_ptr = next_ptr;
    } while(current_ptr);
}

HashMap *wipeMap(HashMap *const map) {
    assert(map != NULL);

    // get size of array from bitmask
    u32 array_size = map->bitmask + 1;

    // loop through all elements of the array, including destroying the linked lists attached to
    // each one if one exists
    for(u32 i = 0; i < array_size; i++) {
        cleanUpLinkedList(map->keyvals_ptr[i].next);
    }

    memset(map->keyvals_ptr, 0, sizeof(KeyValuePair) * array_size);
    return map;
}

#ifdef RUN_TESTS
#define HASH_MAP_TEST_SIZE (256)

START_TEST(map_test_put_one) {
    int value = 1337;
    KeyValuePair array[HASH_MAP_TEST_SIZE];
    HashMap map = newHashMap(array, HASH_MAP_TEST_SIZE, NULL);

    putInMap(&map, "bees", (void*) &value);
    int meow = *(int*)(getFromMap(&map, "bees"));
    ck_assert_int_eq(meow, 1337);
} END_TEST

START_TEST(map_test_put_none) {
    KeyValuePair array[HASH_MAP_TEST_SIZE];
    HashMap map = newHashMap(array, HASH_MAP_TEST_SIZE, NULL);

    void *value = getFromMap(&map, "bees");
    ck_assert_ptr_null(value);
} END_TEST

START_TEST(map_test_put_many) {
    int values[] = {1337, 420, 69, 1984, 1738, 5318008, 80085, 2};
    KeyValuePair array[HASH_MAP_TEST_SIZE];
    HashMap map = newHashMap(array, HASH_MAP_TEST_SIZE, NULL);

    putInMap(&map, "bees", (void*) values);
    putInMap(&map, "bang", (void*) (values + 1));
    putInMap(&map, "boom", (void*) (values + 2));
    putInMap(&map, "damn", (void*) (values + 3));
    putInMap(&map, "woah mama!", (void*) (values + 4));
    putInMap(&map, "j", (void*) (values + 5));
    putInMap(&map, "gaming", (void*) (values + 6));
    putInMap(&map, "your mom is critically overweight", (void*) (values + 7));

    ck_assert_int_eq(*((int*) getFromMap(&map, "bees")), 1337);
    ck_assert_int_eq(*((int*) getFromMap(&map, "bang")), 420);
    ck_assert_int_eq(*((int*) getFromMap(&map, "boom")), 69);
    ck_assert_int_eq(*((int*) getFromMap(&map, "damn")), 1984);
    ck_assert_int_eq(*((int*) getFromMap(&map, "woah mama!")), 1738);
    ck_assert_int_eq(*((int*) getFromMap(&map, "j")), 5318008);
    ck_assert_int_eq(*((int*) getFromMap(&map, "gaming")), 80085);
    ck_assert_int_eq(*((int*) getFromMap(&map, "your mom is critically overweight")), 2);
} END_TEST

START_TEST(map_test_remove) {
    int values[] = {1337, 420, 69};
    KeyValuePair array[HASH_MAP_TEST_SIZE];
    HashMap map = newHashMap(array, HASH_MAP_TEST_SIZE, NULL);

    putInMap(&map, "A", (void*) values);
    putInMap(&map, "B", (void*) (values + 1));
    putInMap(&map, "C", (void*) (values + 2));
    removeFromMap(&map, "B");

    ck_assert_int_eq(*((int*)getFromMap(&map, "A")), 1337);
    ck_assert_ptr_null(getFromMap(&map, "B"));
    ck_assert_int_eq(*((int*)getFromMap(&map, "C")), 69);
} END_TEST

START_TEST(map_test_small_array) {
    int values[] = {1337, 420, 69, 1984, 1738, 5318008, 80085, 2, 70, 89, 1234567, 4};
    KeyValuePair array[2];
    HashMap map = newHashMap(array, 2, NULL);

    putInMap(&map, "bees", (void*) values);
    putInMap(&map, "bang", (void*) (values + 1));
    putInMap(&map, "boom", (void*) (values + 2));
    putInMap(&map, "damn", (void*) (values + 3));
    putInMap(&map, "woah mama!", (void*) (values + 4));
    putInMap(&map, "j", (void*) (values + 5));
    putInMap(&map, "gaming", (void*) (values + 6));
    putInMap(&map, "your mom is critically overweight", (void*) (values + 7));
    putInMap(&map, "idk", (void*) (values + 8));
    putInMap(&map, "I love testing", (void*) (values + 9));
    putInMap(&map, "meow", (void*) (values + 10));
    putInMap(&map, "nyaa :3", (void*) (values + 11));

    ck_assert_int_eq(*((int*) getFromMap(&map, "bees")), 1337);
    ck_assert_int_eq(*((int*) getFromMap(&map, "bang")), 420);
    ck_assert_int_eq(*((int*) getFromMap(&map, "boom")), 69);
    ck_assert_int_eq(*((int*) getFromMap(&map, "damn")), 1984);
    ck_assert_int_eq(*((int*) getFromMap(&map, "woah mama!")), 1738);
    ck_assert_int_eq(*((int*) getFromMap(&map, "j")), 5318008);
    ck_assert_int_eq(*((int*) getFromMap(&map, "gaming")), 80085);
    ck_assert_int_eq(*((int*) getFromMap(&map, "your mom is critically overweight")), 2);
    ck_assert_int_eq(*((int*) getFromMap(&map, "idk")), 70);
    ck_assert_int_eq(*((int*) getFromMap(&map, "I love testing")), 89);
    ck_assert_int_eq(*((int*) getFromMap(&map, "meow")), 1234567);
    ck_assert_int_eq(*((int*) getFromMap(&map, "nyaa :3")), 4);
} END_TEST

START_TEST(map_test_check_small_array_length) {
    KeyValuePair a_thing;
    u32 final_size = 0;
    HashMap map = newHashMap(&a_thing, 1, &final_size);

    ck_assert_uint_eq(final_size, 1);
    ck_assert_uint_eq(map.bitmask, 0);
} END_TEST

Suite *tests_hashMapSuite(void) {
    Suite *returned = suite_create("Hash Map");
    TCase *integ = tcase_create("HashMap Integration");

    tcase_add_test(integ, map_test_put_one);
    tcase_add_test(integ, map_test_put_none);
    tcase_add_test(integ, map_test_put_many);
    tcase_add_test(integ, map_test_remove);
    tcase_add_test(integ, map_test_small_array);
    tcase_add_test(integ, map_test_check_small_array_length); // might remove this from integration
                                                              // tests into unit tests
    suite_add_tcase(returned, integ);
    return returned;
}

#undef HASH_MAP_TEST_SIZE
#endif

