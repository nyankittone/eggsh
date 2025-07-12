// TODO: Consider replacing CUnit with Check. CUnit I think is missing some things I care about...

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

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

void token_test_one(void) {
    static char input[] = "hewwo\n";

    CommandBuilder cmd = newCommandBuilder();
    tokenizeBuilderInput(setParserInput(&cmd, input, sizeof(input) - 1));

    CU_ASSERT_EQUAL(cmd.total_tokens, 1);
    CU_ASSERT(!strcmp(cmd.tokens.ptr, "hewwo"));
}

void token_test_one_broken(void) {
    static char input[] = "meow";

    CommandBuilder cmd = newCommandBuilder();
    tokenizeBuilderInput(setParserInput(&cmd, input, sizeof(input) - 1));

    CU_ASSERT_EQUAL(cmd.total_tokens, 0);
}

void token_test_many(void) {
    static char input[] = "echo Hello, world!\n";

    CommandBuilder cmd = newCommandBuilder();
    tokenizeBuilderInput(setParserInput(&cmd, input, sizeof(input) - 1));

    CU_ASSERT_EQUAL(cmd.total_tokens, 3);

    const char *string1 = cmd.tokens.ptr;
    const char *string2 = string1 + strlen(string1) + 1;
    const char *string3 = string2 + strlen(string2) + 1;

    CU_ASSERT_STRING_EQUAL(string1, "echo");
    CU_ASSERT_STRING_EQUAL(string2, "Hello,");
    CU_ASSERT_STRING_EQUAL(string3, "world!");
}

void token_test_null_bytes(void) {
    static char input[] = "ech\0o He\0\0ya! :""\0""3\n";

    CommandBuilder cmd = newCommandBuilder();
    tokenizeBuilderInput(setParserInput(&cmd, input, sizeof(input) - 1));

    CU_ASSERT_EQUAL(cmd.total_tokens, 3);

    const char *string1 = cmd.tokens.ptr;
    const char *string2 = string1 + strlen(string1) + 1;
    const char *string3 = string2 + strlen(string2) + 1;

    CU_ASSERT_STRING_EQUAL(string1, "echo");
    CU_ASSERT_STRING_EQUAL(string2, "Heya!");
    CU_ASSERT_STRING_EQUAL(string3, ":3");
}

void token_test_extra_space(void) {
    static char input[] = "echo    meow\n";

    CommandBuilder cmd = newCommandBuilder();
    tokenizeBuilderInput(setParserInput(&cmd, input, sizeof(input) - 1));

    CU_ASSERT_EQUAL(cmd.total_tokens, 2);

    const char *string1 = cmd.tokens.ptr;
    const char *string2 = string1 + strlen(string1) + 1;

    CU_ASSERT_STRING_EQUAL(string1, "echo");
    CU_ASSERT_STRING_EQUAL(string2, "meow");
}

void token_test_two_commands(void) {
    static char input1[] = "echo AAA\n";
    static char input2[] = "echo BBB CCC\n";

    CommandBuilder cmd = newCommandBuilder();
    tokenizeBuilderInput(setParserInput(&cmd, input1, sizeof(input1) - 1));

    CU_ASSERT_EQUAL(cmd.total_tokens, 2);

    const char *string1 = cmd.tokens.ptr;
    const char *string2 = string1 + strlen(string1) + 1;

    CU_ASSERT_STRING_EQUAL(string1, "echo");
    CU_ASSERT_STRING_EQUAL(string2, "AAA");

    tokenizeBuilderInput(setParserInput(newCommand(&cmd), input2, sizeof(input2) - 1));

    CU_ASSERT_EQUAL(cmd.total_tokens, 3);

    string1 = cmd.tokens.ptr;
    string2 = string1 + strlen(string1) + 1;
    const char *string3 = string2 + strlen(string2) + 1;

    CU_ASSERT_STRING_EQUAL(string1, "echo");
    CU_ASSERT_STRING_EQUAL(string2, "BBB");
    CU_ASSERT_STRING_EQUAL(string3, "CCC");
}

void token_test_two_inputs(void) {
    static char input1[] = "echo AAA";
    static char input2[] = " BBB CCC\n";

    CommandBuilder cmd = newCommandBuilder();
    TokenizeCommandReturn code = tokenizeBuilderInput(setParserInput(&cmd, input1, sizeof(input1) - 1));
    CU_ASSERT_EQUAL(code, PARSE_COMMAND_OUT_OF_DATA);

    code = tokenizeBuilderInput(setParserInput(&cmd, input2, sizeof(input2) - 1));
    CU_ASSERT_EQUAL(code, PARSE_COMMAND_OUT_OF_DATA | PARSE_COMMAND_HIT_NEWLINE);

    CU_ASSERT_EQUAL(cmd.total_tokens, 4);

    const char *string1 = cmd.tokens.ptr;
    const char *string2 = string1 + strlen(string1) + 1;
    const char *string3 = string2 + strlen(string2) + 1;
    const char *string4 = string3 + strlen(string3) + 1;

    CU_ASSERT_STRING_EQUAL(string1, "echo");
    CU_ASSERT_STRING_EQUAL(string2, "AAA");
    CU_ASSERT_STRING_EQUAL(string3, "BBB");
    CU_ASSERT_STRING_EQUAL(string4, "CCC");
}

void token_test_leading_whitespace(void) {
    static char input[] = "          \t     bruh\n";

    CommandBuilder cmd = newCommandBuilder();
    tokenizeBuilderInput(setParserInput(&cmd, input, sizeof(input) - 1));

    CU_ASSERT_EQUAL(cmd.total_tokens, 1);
    CU_ASSERT_STRING_EQUAL(cmd.tokens.ptr, "bruh");
}

int main(void) {
    // Tests to run:
    // Tokenizer tests

    if(CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();

    CU_pSuite eggsh_suite = CU_add_suite("TokenizerSuite", NULL, NULL);
    if(eggsh_suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        !CU_add_test(eggsh_suite, "One token", &token_test_one) ||
        !CU_add_test(eggsh_suite, "One imcomplete token", &token_test_one_broken) ||
        !CU_add_test(eggsh_suite, "Three tokens", &token_test_many) ||
        !CU_add_test(eggsh_suite, "Three tokens with null bytes", &token_test_null_bytes) ||
        !CU_add_test(eggsh_suite, "Two tokens with extra space", &token_test_extra_space) ||
        !CU_add_test(eggsh_suite, "Two commands", &token_test_two_commands) ||
        !CU_add_test(eggsh_suite, "Two inputs, one command", &token_test_two_inputs) ||
        !CU_add_test(eggsh_suite, "Leading whitespace", &token_test_extra_space)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    eggsh_suite = CU_add_suite("HashMapSuite", NULL, NULL);
    if(eggsh_suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        !CU_add_test(eggsh_suite, "Hash map with one item", &map_test_put_one) ||
        !CU_add_test(eggsh_suite, "Hash map with many items", &map_test_put_many)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    puts("\n^ I hate reading this output ^");
    return 0;
}

