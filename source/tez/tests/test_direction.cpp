#include "pch.hpp"
#include <gtest/gtest.h>

#include "tez/direction.hpp"

TEST(Direction, Opposite) {
    using namespace tez;
    typedef direction dir;

    EXPECT_EQ(dir::north,      opposite_direction(dir::south));
    EXPECT_EQ(dir::south,      opposite_direction(dir::north));
    EXPECT_EQ(dir::east,       opposite_direction(dir::west));
    EXPECT_EQ(dir::west,       opposite_direction(dir::east));
    EXPECT_EQ(dir::north_east, opposite_direction(dir::south_west));
    EXPECT_EQ(dir::south_west, opposite_direction(dir::north_east));
    EXPECT_EQ(dir::north_west, opposite_direction(dir::south_east));
    EXPECT_EQ(dir::south_east, opposite_direction(dir::north_west));
    EXPECT_EQ(dir::up,         opposite_direction(dir::down));
    EXPECT_EQ(dir::down,       opposite_direction(dir::up));
    EXPECT_EQ(dir::here,       opposite_direction(dir::none));
    EXPECT_EQ(dir::none,       opposite_direction(dir::here));
}

TEST(Direction, Vector) {
    using namespace tez;
    typedef direction dir;

    auto const check = [](dir const a, dir const b) {
        EXPECT_EQ(0, direction_vector(a).first + direction_vector(b).first);
        EXPECT_EQ(0, direction_vector(a).second + direction_vector(b).second);
    };

    check(dir::north,      dir::south);
    check(dir::south,      dir::north);
    check(dir::east,       dir::west);
    check(dir::west,       dir::east);
    check(dir::north_east, dir::south_west);
    check(dir::south_west, dir::north_east);
    check(dir::north_west, dir::south_east);
    check(dir::south_east, dir::north_west);
    
    BK_TEST_FAILURES {
        EXPECT_THROW(check(dir::up,  dir::down),  assertion_failure);
        EXPECT_THROW(check(dir::down, dir::up),   assertion_failure);
        EXPECT_THROW(check(dir::here, dir::none), assertion_failure);
        EXPECT_THROW(check(dir::none, dir::here), assertion_failure);
    }
}

TEST(Direction, NextPrevCardinal) {
    using namespace tez;
    typedef direction dir;

    EXPECT_EQ(dir::south, next_cardinal_direction(dir::north));
    EXPECT_EQ(dir::east,  next_cardinal_direction(dir::south));
    EXPECT_EQ(dir::west,  next_cardinal_direction(dir::east));
    EXPECT_EQ(dir::north, next_cardinal_direction(dir::west));

    EXPECT_EQ(dir::west,  prev_cardinal_direction(dir::north));
    EXPECT_EQ(dir::north, prev_cardinal_direction(dir::south));
    EXPECT_EQ(dir::south, prev_cardinal_direction(dir::east));
    EXPECT_EQ(dir::east,  prev_cardinal_direction(dir::west));
}