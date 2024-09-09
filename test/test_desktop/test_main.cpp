#include <unity.h>

void test_stringToTrendDirection_valid_inputs();
void test_stringToTrendDirection_invalid_inputs();
void test_sample();

void setUp(void) {
    // Common setup code (if any)
}

void tearDown(void) {
    // Common teardown code (if any)
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_stringToTrendDirection_valid_inputs);
    RUN_TEST(test_stringToTrendDirection_invalid_inputs);
    
    RUN_TEST(test_sample);
    
    return UNITY_END();
}