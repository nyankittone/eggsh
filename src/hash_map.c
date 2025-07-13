#define _XOPEN_SOURCE 500

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <hash_map.h>
#include <util.h>

HashMap newHashMap(KeyValuePair *const array, u32 array_size, u32 *const final_array_size) {
    assert(array != NULL && array_size > 0);
    HashMap returned = {.keyvals_ptr = array};

    // I dunno if this bullshit bitshifting code will work tbh....
    u32 bitmask = ~0;
    for(; array_size; array_size >>= 1, bitmask <<= 1);
    bitmask = (~bitmask) >> 1;
    returned.bitmask = bitmask;

    u32 staged_final_array_size = bitmask + 1;
    memset(array, 0, sizeof(*array) * staged_final_array_size);

    if(final_array_size) *final_array_size = staged_final_array_size;
    return returned;
}

// This is the hash function for my hash map. it's just SDBM. I may replace it with something more
// optimal later.
static u32 hashMeMommyUwu(const char *const key) {
    u32 returned = 0;

    for(const char *key_traveller = key; *key_traveller; key_traveller++) {
        returned = returned * 65599 + *key_traveller;
    }

    return returned;
}

#define KEY_VALUE_RESULT_NOT_FOUND ((u8) 0)
#define KEY_VALUE_RESULT_FOUND ((u8) 1)
#define KEY_VALUE_RESULT_FOUND_ON_FIRST ((u8) 2)

typedef struct {
    u8 status;
    KeyValuePair *item;
} KeyValueFindResult;

// This function is used to get a pointer a key-value pair inside a linked list in the underlying
// array for this hash map.
static KeyValueFindResult findKeyValuePair(KeyValuePair *const start, const char *const key) {
    assert(start != NULL);
    KeyValuePair *moving_node = start;

    // iterate through provided linked list nodes
    for(; moving_node->next; moving_node = moving_node->next) {
        // if we encounter a matching key, return immediately
        if(!strcmp(moving_node->key, key)) {
            return (KeyValueFindResult) {
                .status = moving_node == start ? KEY_VALUE_RESULT_FOUND_ON_FIRST : KEY_VALUE_RESULT_FOUND,
                .item = moving_node,
            };
        }
    }

    return (KeyValueFindResult) {
        .status = KEY_VALUE_RESULT_NOT_FOUND,
        .item = moving_node,
    };
}

HashMap *putInMap(HashMap *const map, char *const key, void *const value) {
    assert(map != NULL && key != NULL);

    KeyValuePair *start = map->keyvals_ptr + (hashMeMommyUwu(key) & map->bitmask);
    KeyValueFindResult target = findKeyValuePair(start, key);

    if(target.status) {
        target.item->value = value;
        return map;
    }

    KeyValuePair *target_pair;

    if(!start->key) {
        target_pair = start;
    } else {
        target.item->next = mallocOrDie(sizeof(KeyValuePair));
        target_pair = target.item->next;
    }

    target_pair->key = strdup(key);
    target_pair->value = value;

    return map;
}

HashMap *putInMapIfUnique(HashMap *const map, char *const key, void *const value) {
    assert(map != NULL && key != NULL);

    KeyValuePair *start = map->keyvals_ptr + (hashMeMommyUwu(key) & map->bitmask);
    KeyValueFindResult target = findKeyValuePair(start, key);
    if(target.status) return map;

    KeyValuePair *target_pair;

    if(!start->key) {
        target_pair = start;
    } else {
        target.item->next = mallocOrDie(sizeof(KeyValuePair));
        target_pair = target.item->next;
    }

    target_pair->key = strdup(key);
    target_pair->value = value;
    return map;
}

void *getFromMap(HashMap *const map, const char *const key) {
    assert(map != NULL && key != NULL);
    const KeyValuePair *node = map->keyvals_ptr + (hashMeMommyUwu(key) & map->bitmask);

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
    KeyValuePair *start = map->keyvals_ptr + (hashMeMommyUwu(key) & map->bitmask);

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
        map->keyvals_ptr[i] = (KeyValuePair) {0};
    }

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
    int values[] = {1337, 420, 69, 1984, 1738, 5318008, 80085, 2};
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

    ck_assert_int_eq(*((int*) getFromMap(&map, "bees")), 1337);
    ck_assert_int_eq(*((int*) getFromMap(&map, "bang")), 420);
    ck_assert_int_eq(*((int*) getFromMap(&map, "boom")), 69);
    ck_assert_int_eq(*((int*) getFromMap(&map, "damn")), 1984);
    ck_assert_int_eq(*((int*) getFromMap(&map, "woah mama!")), 1738);
    ck_assert_int_eq(*((int*) getFromMap(&map, "j")), 5318008);
    ck_assert_int_eq(*((int*) getFromMap(&map, "gaming")), 80085);
    ck_assert_int_eq(*((int*) getFromMap(&map, "your mom is critically overweight")), 2);
} END_TEST

START_TEST(mapt_test_1_element_array) {
    int values[] = {1337, 420, 69, 1984, 1738, 5318008, 80085, 2};
    KeyValuePair array[1];
    HashMap map = newHashMap(array, 1, NULL);

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
    tcase_add_test(integ, mapt_test_1_element_array);
    tcase_add_test(integ, map_test_check_small_array_length); // might remove this from integration
                                                              // tests into unit tests
    suite_add_tcase(returned, integ);
    return returned;
}

#undef HASH_MAP_TEST_SIZE
#endif

