#include <unity.h>
#include <string.h>

// Forward declarations
namespace test_secure_http_client
{
    void setUp(void);
    void tearDown(void);
}

namespace test_dexcom_client
{
    void setUp(void);
    void tearDown(void);
}

// Global setUp and tearDown
void setUp(void)
{
    // Call the appropriate setUp based on current test group
    if (Unity.CurrentTestName != nullptr &&
        strstr(Unity.CurrentTestName, "secure_http_client") != nullptr)
    {
        test_secure_http_client::setUp();
    }
    else
    {
        test_dexcom_client::setUp();
    }
}

void tearDown(void)
{
    // Call the appropriate tearDown based on current test group
    if (Unity.CurrentTestName != nullptr &&
        strstr(Unity.CurrentTestName, "secure_http_client") != nullptr)
    {
        test_secure_http_client::tearDown();
    }
    else
    {
        test_dexcom_client::tearDown();
    }
}

void test_stringToTrendDirection_valid_inputs();
void test_stringToTrendDirection_invalid_inputs();
void test_glucose_reading_construction();
void test_glucose_reading_invalid_input();
void test_sample();
void run_secure_http_client_tests(void);
void run_dexcom_client_tests(void);

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    run_secure_http_client_tests();

    // run_dexcom_client_tests();
    // RUN_TEST(test_stringToTrendDirection_valid_inputs);
    // RUN_TEST(test_stringToTrendDirection_invalid_inputs);

    // RUN_TEST(test_glucose_reading_construction);
    // RUN_TEST(test_glucose_reading_invalid_input);

    return UNITY_END();
}