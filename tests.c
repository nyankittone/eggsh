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

START_TEST(token_test_one) {
    static char input[] = "hewwo\n";

    CommandBuilder cmd = newCommandBuilder();
    tokenizeBuilderInput(setParserInput(&cmd, input, sizeof(input) - 1));

    // CU_ASSERT_EQUAL(cmd.total_tokens, 1);
    // CU_ASSERT(!strcmp(cmd.tokens.ptr, "hewwo"));
    ck_assert_uint_eq(cmd.total_tokens, 1);
    ck_assert_str_eq(cmd.tokens.ptr, "hewwo");
} END_TEST

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

Suite *tokenizerSuite(void) {
    Suite *returned;
    TCase *test_case_core;

    returned = suite_create("Tokenizer");
    test_case_core = tcase_create("Core");

    tcase_add_test(test_case_core, token_test_one);
    suite_add_tcase(returned, test_case_core);

    return returned;
}

int main(void) {
    Suite *suite = tokenizerSuite();
    SRunner *suite_runner = srunner_create(suite);

    srunner_run_all(suite_runner, CK_VERBOSE);
    int number_failed = srunner_ntests_failed(suite_runner);

    srunner_free(suite_runner);
    return number_failed > 127 ? 127 : number_failed;
}

