/*
 * Google Test example
 */
#include <gtest/gtest.h>

bool test_func() {
    return true;
}

TEST(UnitTests, DemonstrateGTestMacros) {
    EXPECT_EQ(true, true);
    const bool result = test_func();
    EXPECT_EQ(true, result);
}