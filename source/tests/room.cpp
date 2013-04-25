#include "pch.hpp"
#include "room.hpp"

#include <gtest/gtest.h>

TEST(Room, Constructor) {
    static unsigned const W = 10;
    static unsigned const H = 5;
    static tile_category  V = tile_category::empty;

    struct {
        unsigned width()  const { return W; }
        unsigned height() const { return H; }
        
        tile_category default() const { return V; }

        void generate(grid2d<tile_category>& grid) {
            EXPECT_EQ(W, grid.width());
            EXPECT_EQ(H, grid.height());
        }
    } test_generator;

    auto test_room = room(test_generator);

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
