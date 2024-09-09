#include <unity.h>
#include <sample.h>

void test_sample()
{
    int result = add(1, 2);

    TEST_ASSERT(result == 3);
}
