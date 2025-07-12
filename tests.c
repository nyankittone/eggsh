// TODO: Consider replacing CUnit with Check. CUnit I think is missing some things I care about...

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <check.h>

#include <command_builder.h>
#include <hash_map.h>

#define HASH_MAP_TEST_SIZE (256)

void map_test_put_one(void) {
    int value = 1337;
    KeyValuePair array[HASH_MAP_TEST_SIZE];
    HashMap map = newHashMap(array, HASH_MAP_TEST_SIZE, NULL);

    putInMap(&map, "bees", (void*) &value);
    CU_ASSERT_EQUAL(*((int*)getFromMap(&map, "bees")), 1337);
}

void map_test_put_none(void) {
    KeyValuePair array[HASH_MAP_TEST_SIZE];
    HashMap map = newHashMap(array, HASH_MAP_TEST_SIZE, NULL);

    CU_ASSERT_EQUAL(getFromMap(&map, "bees"), NULL);
}

void map_test_put_many(void) {
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

    CU_ASSERT_EQUAL(*((int*) getFromMap(&map, "bees")), 1337);
    CU_ASSERT_EQUAL(*((int*) getFromMap(&map, "bang")), 420);
    CU_ASSERT_EQUAL(*((int*) getFromMap(&map, "boom")), 69);
    CU_ASSERT_EQUAL(*((int*) getFromMap(&map, "damn")), 1984);
    CU_ASSERT_EQUAL(*((int*) getFromMap(&map, "woah mama!")), 1738);
    CU_ASSERT_EQUAL(*((int*) getFromMap(&map, "j")), 5318008);
    CU_ASSERT_EQUAL(*((int*) getFromMap(&map, "gaming")), 80085);
    CU_ASSERT_EQUAL(*((int*) getFromMap(&map, "your mom is critically overweight")), 2);
}

void map_test_remove(void) {
    int values[] = {1337, 420, 69};
    KeyValuePair array[HASH_MAP_TEST_SIZE];
    HashMap map = newHashMap(array, HASH_MAP_TEST_SIZE, NULL);

    putInMap(&map, "A", (void*) values);
    putInMap(&map, "B", (void*) (values + 1));
    putInMap(&map, "C", (void*) (values + 2));
    removeFromMap(&map, "B");

    CU_ASSERT_EQUAL(*((int*)getFromMap(&map, "A")), 1337);
    CU_ASSERT_EQUAL(getFromMap(&map, "B"), NULL);
    CU_ASSERT_EQUAL(*((int*)getFromMap(&map, "C")), 69);
}

int main(void) {
    Suite *suite = tests_tokenizerSuite();
    SRunner *suite_runner = srunner_create(suite);

    srunner_run_all(suite_runner, CK_VERBOSE);
    int failed_tests = srunner_ntests_failed(suite_runner);

    srunner_free(suite_runner);
    return failed_tests > 127 ? 127 : failed_tests;
}

