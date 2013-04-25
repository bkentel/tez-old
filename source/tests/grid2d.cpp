#include "pch.hpp"
#include "grid2d.hpp"

#include <gtest/gtest.h>

namespace {

class Grid2DTest : public ::testing::Test {
public :
    static const unsigned WIDTH  = 5;
    static const unsigned HEIGHT = 10;
    static const unsigned VALUE  = 0xABC;

    Grid2DTest()
        : test_grid(WIDTH, HEIGHT, VALUE)
    {
    }

    grid2d<unsigned> test_grid;
};

}

TEST_F(Grid2DTest, Constructor) {
    EXPECT_EQ(WIDTH,  test_grid.width());
    EXPECT_EQ(HEIGHT, test_grid.height());

    for (unsigned y = 0; y < HEIGHT; ++y) {
        for (unsigned x = 0; x < WIDTH; ++x) {
            EXPECT_EQ(VALUE, test_grid.at(x, y));
        }
    }
}

TEST_F(Grid2DTest, Swap) {
    // create and grid with height and width reversed
    grid2d<unsigned> grid(HEIGHT, WIDTH, VALUE + 1);

    swap(test_grid, grid);

    EXPECT_EQ(HEIGHT, test_grid.width());
    EXPECT_EQ(WIDTH,  test_grid.height());

    for (unsigned y = 0; y < WIDTH; ++y) {
        for (unsigned x = 0; x < HEIGHT; ++x) {
            EXPECT_EQ(VALUE + 1, test_grid.at(x, y));
        }
    }

    EXPECT_EQ(WIDTH,  grid.width());
    EXPECT_EQ(HEIGHT, grid.height());

    for (unsigned y = 0; y < HEIGHT; ++y) {
        for (unsigned x = 0; x < WIDTH; ++x) {
            EXPECT_EQ(VALUE, grid.at(x, y));
        }
    }
}

TEST_F(Grid2DTest, Clone) {
    grid2d<unsigned> grid = test_grid.clone();

    EXPECT_EQ(WIDTH,  grid.width());
    EXPECT_EQ(HEIGHT, grid.height());

    for (unsigned y = 0; y < HEIGHT; ++y) {
        for (unsigned x = 0; x < WIDTH; ++x) {
            EXPECT_EQ(test_grid.at(x, y), grid.at(x, y));
        }
    }
}


TEST_F(Grid2DTest, IsValidIndex) {
    EXPECT_FALSE(test_grid.is_valid_index(WIDTH, 0));
    EXPECT_FALSE(test_grid.is_valid_index(0, HEIGHT));
    EXPECT_FALSE(test_grid.is_valid_index(WIDTH, HEIGHT));

    for (unsigned y = 0; y < HEIGHT; ++y) {
        for (unsigned x = 0; x < WIDTH; ++x) {
            EXPECT_TRUE(test_grid.is_valid_index(x, y));
        }
    }
}

TEST_F(Grid2DTest, MoveConstructor) {
    grid2d<unsigned> grid = std::move(test_grid);

    EXPECT_EQ(WIDTH,  grid.width());
    EXPECT_EQ(HEIGHT, grid.height());

    for (unsigned y = 0; y < HEIGHT; ++y) {
        for (unsigned x = 0; x < WIDTH; ++x) {
            EXPECT_EQ(VALUE, grid.at(x, y));
        }
    }
}

TEST(Block, Center) {
    static const unsigned SIZE = 3;
        
    grid2d<unsigned> grid(SIZE, SIZE, 0);
    
    for (unsigned y = 0; y < SIZE; ++y) {
        for (unsigned x = 0; x < SIZE; ++x) {
            grid.set(x, y, x + y*SIZE);
        }
    }

    block<unsigned> center(grid, 1, 1, 0);

    EXPECT_EQ(center.nw, center.get<direction::north_west>());
    EXPECT_EQ(center.n,  center.get<direction::north>());
    EXPECT_EQ(center.ne, center.get<direction::north_east>());
    EXPECT_EQ(center.w,  center.get<direction::west>());
    EXPECT_EQ(center.e,  center.get<direction::east>());
    EXPECT_EQ(center.sw, center.get<direction::south_west>());
    EXPECT_EQ(center.s,  center.get<direction::south>());
    EXPECT_EQ(center.se, center.get<direction::south_east>());

    EXPECT_EQ(0, center.get<direction::north_west>());
    EXPECT_EQ(1, center.get<direction::north>());
    EXPECT_EQ(2, center.get<direction::north_east>());
    EXPECT_EQ(3, center.get<direction::west>());
    EXPECT_EQ(5, center.get<direction::east>());
    EXPECT_EQ(6, center.get<direction::south_west>());
    EXPECT_EQ(7, center.get<direction::south>());
    EXPECT_EQ(8, center.get<direction::south_east>());
}

TEST(Block, Corner) {
    static const unsigned SIZE = 3;
        
    grid2d<unsigned> grid(SIZE, SIZE, 0);
    
    for (unsigned y = 0; y < SIZE; ++y) {
        for (unsigned x = 0; x < SIZE; ++x) {
            grid.set(x, y, x + y*SIZE);
        }
    }

    block<unsigned> center(grid, 0, 0, 0xFF);

    EXPECT_EQ(center.nw, center.get<direction::north_west>());
    EXPECT_EQ(center.n,  center.get<direction::north>());
    EXPECT_EQ(center.ne, center.get<direction::north_east>());
    EXPECT_EQ(center.w,  center.get<direction::west>());
    EXPECT_EQ(center.e,  center.get<direction::east>());
    EXPECT_EQ(center.sw, center.get<direction::south_west>());
    EXPECT_EQ(center.s,  center.get<direction::south>());
    EXPECT_EQ(center.se, center.get<direction::south_east>());

    EXPECT_EQ(0xFF, center.get<direction::north_west>());
    EXPECT_EQ(0xFF, center.get<direction::north>());
    EXPECT_EQ(0xFF, center.get<direction::north_east>());
    EXPECT_EQ(0xFF, center.get<direction::west>());
    EXPECT_EQ(5, center.get<direction::east>());
    EXPECT_EQ(0xFF, center.get<direction::south_west>());
    EXPECT_EQ(7, center.get<direction::south>());
    EXPECT_EQ(8, center.get<direction::south_east>());
}
