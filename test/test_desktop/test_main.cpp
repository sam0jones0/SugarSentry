#include <unity.h>

void test_stringToTrendDirection_valid_inputs();
void test_stringToTrendDirection_invalid_inputs();
void test_glucose_reading_construction();
void test_glucose_reading_invalid_input();
void test_sample();
void run_dexcom_client_tests();

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_stringToTrendDirection_valid_inputs);
    RUN_TEST(test_stringToTrendDirection_invalid_inputs);

    RUN_TEST(test_glucose_reading_construction);
    RUN_TEST(test_glucose_reading_invalid_input);

    run_dexcom_client_tests();

    return UNITY_END();
}