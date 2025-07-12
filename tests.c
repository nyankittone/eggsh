#include <check.h>

#include <command_builder.h>
#include <hash_map.h>

int main(void) {
    SRunner *suite_runner = srunner_create(tests_tokenizerSuite());
    srunner_add_suite(suite_runner, tests_hashMapSuite());

    srunner_run_all(suite_runner, CK_VERBOSE);
    int failed_tests = srunner_ntests_failed(suite_runner);

    srunner_free(suite_runner);
    return failed_tests > 127 ? 127 : failed_tests;
}

