#include "pch.hpp"
#include "room.hpp"

#include <gtest/gtest.h>

#include "room_compound.hpp"

class RoomTest : public ::testing::Test {
public :
    struct test_generator {
        test_generator(tile_category value, unsigned w, unsigned h)
            : value(value), w(w), h(h)
        {
        }

        unsigned width()  const { return w; }
        unsigned height() const { return h; }
        
        tile_category default() const { return value; }

        void generate(grid2d<tile_category>& grid) {
            EXPECT_EQ(w, grid.width());
            EXPECT_EQ(h, grid.height());
        }

        tile_category value;
        unsigned w, h;
    };
};

TEST(RoomTest, Compound) {
    std::default_random_engine random(1984);

    while(1) {
        auto test_room = room(compound_room_generator(random));
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
