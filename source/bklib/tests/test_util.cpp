#include "pch.hpp"
#include "bklib/util.hpp"

#include <gtest/gtest.h>

TEST(Util, MinMax) {
    EXPECT_EQ(-1, bklib::min(10, 20, 15, 0, -1));
    EXPECT_EQ(20, bklib::max(10, 20, 15, 0, -1));

}
