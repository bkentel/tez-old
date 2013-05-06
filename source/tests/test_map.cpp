#include "pch.hpp"
#include "map.hpp"

#include "room_generator.hpp"

#include <gtest/gtest.h>

TEST(Map, Constructor) {
    auto test_map = map(10, 20);

    EXPECT_EQ(10, test_map.width());
    EXPECT_EQ(20, test_map.height());
}

TEST(Map, AddRoom) {
    auto test_map = map(10, 20);

    std::default_random_engine random(1984);
    room test_room = simple_room_generator(make_random_wrapper(random)).generate();

    test_map.add_room(test_room, 0, 0);

    for (auto const& i : test_room) {
        EXPECT_EQ(*i, test_map.at(i.x, i.y).type);
    }
}
