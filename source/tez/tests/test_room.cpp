#include "pch.hpp"
#include "tez/room.hpp"

#include "tez/room_generator.hpp"

#include <gtest/gtest.h>

class RoomTest : public ::testing::Test {
public :
    typedef tez::room::grid_t grid_t;

    static const unsigned WIDTH  = 5;
    static const unsigned HEIGHT = 10;

    RoomTest()
        : engine(std::random_device()())
        , test_room(
            tez::simple_room_generator(bklib::make_random_wrapper(engine)).generate()
        )
    {
    }

    std::default_random_engine engine;
    tez::room test_room;
};

TEST_F(RoomTest, Bounds) {
    auto const w = test_room.width();
    auto const h = test_room.height();

    EXPECT_NE(0U, w);
    EXPECT_NE(0U, h);

    auto const bounds = test_room.bounds();
    
    EXPECT_EQ(0, bounds.left);
    EXPECT_EQ(0, bounds.top);
    EXPECT_EQ(w, bounds.right);
    EXPECT_EQ(h, bounds.bottom);

    EXPECT_EQ(0, test_room.left());
    EXPECT_EQ(0, test_room.top());
    EXPECT_EQ(w, test_room.right());
    EXPECT_EQ(h, test_room.bottom());

    EXPECT_EQ(w, bounds.width());
    EXPECT_EQ(h, bounds.height());

    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            EXPECT_NE(tez::tile_category::empty, test_room.at(x, y));
        }
    }

    BK_TEST_FAILURES {
        EXPECT_THROW(test_room.at(0, h), assertion_failure);
        EXPECT_THROW(test_room.at(w, 0), assertion_failure);
        EXPECT_THROW(test_room.at(w, h), assertion_failure);
    }
}

TEST_F(RoomTest, Iterator) {

}

TEST_F(RoomTest, FindConnectable) {
    auto random = bklib::make_random_wrapper(engine);

    auto const w = test_room.width();
    auto const h = test_room.height();

    for (auto i = 0; i < 100; ++i) {
        auto const p =
            test_room.find_connection_point(tez::direction::north, random);
        EXPECT_EQ(0U, p.y);
        EXPECT_LT(p.x, w);
        EXPECT_GE(p.x, 0U);
    }

    for (auto i = 0; i < 100; ++i) {
        auto const p =
            test_room.find_connection_point(tez::direction::south, random);
        EXPECT_EQ(h-1, p.y);
        EXPECT_LT(p.x, w);
        EXPECT_GE(p.x, 0U);
    }

    for (auto i = 0; i < 100; ++i) {
        auto const p =
            test_room.find_connection_point(tez::direction::west, random);
        EXPECT_EQ(0U, p.x);
        EXPECT_LT(p.y, h);
        EXPECT_GE(p.y, 0U);
    }

    for (auto i = 0; i < 100; ++i) {
        auto const p =
            test_room.find_connection_point(tez::direction::east, random);
        EXPECT_EQ(w-1, p.x);
        EXPECT_LT(p.y, h);
        EXPECT_GE(p.y, 0U);
    }
}

TEST_F(RoomTest, Constructor) {

}

TEST_F(RoomTest, SwapMove) {
    auto random = bklib::make_random_wrapper(engine);

    auto& room_a = test_room;
    auto  room_b = tez::room(tez::simple_room_generator(random).generate());

    const unsigned      W0 = 5;
    const unsigned      H0 = 10;

    const unsigned      W1 = 4;
    const unsigned      H1 = 9;

    swap(room_a, room_b);
    
    EXPECT_EQ(W0, room_b.width());    
    EXPECT_EQ(H0, room_b.height());
    
    EXPECT_EQ(W1, room_a.width());    
    EXPECT_EQ(H1, room_a.height());
}
