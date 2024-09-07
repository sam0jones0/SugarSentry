#include <unity.h>
#include <sample.h>

void setUp(void)
{
    // setup stuff
}

void tearDown(void)
{
    // tear down stuff
}

void test_sample()
{

    int result = add(1, 2);

    TEST_ASSERT(result == 3);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_sample);
    UNITY_END();
}