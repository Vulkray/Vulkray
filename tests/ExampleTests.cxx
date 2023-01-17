/*
 * Google Test example
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2023, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
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