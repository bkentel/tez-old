#include "pch.hpp"

#include "tez/map.hpp"
#include "tez/map_layout.hpp"
#include "tez/room_generator.hpp"

#include <gtest/gtest.h>

TEST(Map, Constructor) {
    auto test_map = tez::map(10, 20);

    EXPECT_EQ(10, test_map.width());
    EXPECT_EQ(20, test_map.height());
}

TEST(Map, AddRoom) {
    auto test_map = tez::map(10, 20);

    std::default_random_engine random(1984);
    tez::room test_room = tez::simple_room_generator(bklib::make_random_wrapper(random)).generate();

    test_map.add_room(test_room, 0, 0);

    for (auto const& i : test_room) {
        EXPECT_EQ(*i, test_map.at(i.x, i.y).type);
    }
}

TEST(MapCreation, Test) {
    for(unsigned n = 0; n < 1000; ++n) {
    std::default_random_engine engine(::GetTickCount());
    auto random = bklib::make_random_wrapper(engine);

    tez::map_layout layout;

    auto gen_simple   = tez::simple_room_generator(random);
    auto gen_compound = tez::compound_room_generator(random);

    for (int i = 0; i < 20; ++i) {
        if (i % 4 == 0) {
            layout.add_room(gen_compound.generate(), random);
        } else {
            layout.add_room(gen_simple.generate(), random);
        }
    }

    layout.normalize();
    auto test_map = layout.make_map();

    std::cout << test_map;
    }
}
