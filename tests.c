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

int main(void) {
    // Tests to run:
    // Tokenizer tests

    if(CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();
    CU_pSuite eggsh_suite = CU_add_suite("TokenizerSuite", NULL, NULL);
    if(eggsh_suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if(CU_add_test(eggsh_suite, "One token", &token_test_one) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}

