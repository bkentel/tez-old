#include "pch.hpp"
#include "room.hpp"

#include <gtest/gtest.h>

class RoomTest : public ::testing::Test {
public :
    struct test_generator {
        test_generator(tile_category value, unsigned w, unsigned h)
            : value(value), w(w), h(h)
        {
        }

        grid2d<tile_category> generate() {
            return grid2d<tile_category>(w, h, value);
        }

        tile_category value;
        unsigned w, h;
    };
};

TEST_F(RoomTest, Iterator) {
    static const unsigned      W = 5;
    static const unsigned      H = 10;
    static tile_category const V = tile_category::floor;

    room test_room = test_generator(V, W, H);

    unsigned count = 0;
    for (auto& i : test_room) {
        EXPECT_EQ(tile_category::floor, *i);
        
        i = tile_category::empty;
        
        count++;
    }

    EXPECT_EQ(W*H, count);
    
    [](room const& r) {
        for (auto const& i : r) {
            EXPECT_EQ(tile_category::empty, *i);
        }
    }(test_room);
}

TEST_F(RoomTest, FindConnectable) {
    static const unsigned      W  = 5;
    static const unsigned      H = 10;
    static tile_category const V  = tile_category::floor;

    std::default_random_engine engine(1984);
    auto random = make_random_wrapper(engine);

    auto test_room = room(test_generator(V, W, H));

    for (auto i = 0; i < 100; ++i) {
        auto const p =
            test_room.find_connectable_point(random, direction::north);
        EXPECT_EQ(0U, p.y);
        EXPECT_LT(p.x, W);
        EXPECT_GE(p.x, 0U);
    }

    for (auto i = 0; i < 100; ++i) {
        auto const p =
            test_room.find_connectable_point(random, direction::south);
        EXPECT_EQ(H-1, p.y);
        EXPECT_LT(p.x, W);
        EXPECT_GE(p.x, 0U);
    }

    for (auto i = 0; i < 100; ++i) {
        auto const p =
            test_room.find_connectable_point(random, direction::west);
        EXPECT_EQ(0U, p.x);
        EXPECT_LT(p.y, H);
        EXPECT_GE(p.y, 0U);
    }

    for (auto i = 0; i < 100; ++i) {
        auto const p =
            test_room.find_connectable_point(random, direction::east);
        EXPECT_EQ(W-1, p.x);
        EXPECT_LT(p.y, H);
        EXPECT_GE(p.y, 0U);
    }
}

TEST_F(RoomTest, Constructor) {
    static const unsigned      W  = 5;
    static const unsigned      H = 10;
    static tile_category const V  = tile_category::empty;

    auto test_room = room(test_generator(V, W, H));

    EXPECT_EQ(W, test_room.width());
    EXPECT_EQ(H, test_room.height());

    EXPECT_EQ(0, test_room.left());
    EXPECT_EQ(0, test_room.top());
    EXPECT_EQ(W, test_room.right());
    EXPECT_EQ(H, test_room.bottom());

    for (unsigned yi = 0; yi < H; ++yi) {
        for (unsigned xi = 0; xi < W; ++xi) {
            EXPECT_EQ(V, test_room.at(xi, yi));
        }
    }
}

TEST_F(RoomTest, SwapMove) {
    static const unsigned      W0 = 5;
    static const unsigned      H0 = 10;
    static tile_category const V0 = tile_category::empty;

    static const unsigned      W1 = 4;
    static const unsigned      H1 = 9;
    static tile_category const V1 = tile_category::ceiling;

    auto room_a = room(test_generator(V0, W0, H0));
    auto room_b = room(test_generator(V1, W1, H1));

    swap(room_a, room_b);
    
    EXPECT_EQ(W0, room_b.width());    
    EXPECT_EQ(H0, room_b.height());
    
    for (unsigned yi = 0; yi < H0; ++yi) {
        for (unsigned xi = 0; xi < W0; ++xi) {
            EXPECT_EQ(V0, room_b.at(xi, yi));
        }
    }

    EXPECT_EQ(W1, room_a.width());    
    EXPECT_EQ(H1, room_a.height());
    
    for (unsigned yi = 0; yi < H1; ++yi) {
        for (unsigned xi = 0; xi < W1; ++xi) {
            EXPECT_EQ(V1, room_a.at(xi, yi));
        }
    }

    room_a = std::move(room_b);

    EXPECT_EQ(W0, room_a.width());    
    EXPECT_EQ(H0, room_a.height());
    
    for (unsigned yi = 0; yi < H0; ++yi) {
        for (unsigned xi = 0; xi < W0; ++xi) {
            EXPECT_EQ(V0, room_a.at(xi, yi));
        }
    }

    room room_c{std::move(room_a)};

    EXPECT_EQ(W0, room_c.width());    
    EXPECT_EQ(H0, room_c.height());
    
    for (unsigned yi = 0; yi < H0; ++yi) {
        for (unsigned xi = 0; xi < W0; ++xi) {
            EXPECT_EQ(V0, room_c.at(xi, yi));
        }
    }
}
