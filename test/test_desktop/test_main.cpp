#include <unity.h>

// Forward declarations of test-specific setup/teardown
extern void setUp(void);
extern void tearDown(void);

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Run all test groups
    extern void run_arduino_json_parser_tests();
    run_arduino_json_parser_tests();

    return UNITY_END();
}
