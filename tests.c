#include <stdio.h>

#include <check.h>

#include <tokenizer.h>
#include <hash_map.h>
#include <testing/output_capture.h>

int bruh(void *whatever) {
    puts("Meow!");

    return 69;
}

int main(void) {
    SRunner *suite_runner = srunner_create(tests_tokenizerSuite());
    srunner_add_suite(suite_runner, tests_hashMapSuite());
    srunner_add_suite(suite_runner, tests_echoSuite());

    srunner_run_all(suite_runner, CK_VERBOSE);
    int failed_tests = srunner_ntests_failed(suite_runner);
    int total_tests = srunner_ntests_run(suite_runner);

    srunner_free(suite_runner);

    printf (
        "\n\33[1;%sm%d/%d tests failed\33[m %s\n",
        failed_tests == 0 ? "92" : "91",
        failed_tests,
        total_tests,
        failed_tests == 0 ? "🎉 🎉 🎉" : "¯\\_(ツ)_/¯"
    );

    return failed_tests > 127 ? 127 : failed_tests;
}

