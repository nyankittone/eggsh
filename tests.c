// TODO: Consider replacing CUnit with a better testing framework. This current one I think is
// missing some things I care about... :(

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include <command_builder.h>

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
        !CU_add_test(eggsh_suite, "Two inputs, one command", &token_test_two_inputs)
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

