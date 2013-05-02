#include "pch.hpp"
#include "grid2d.hpp"

#include "scope_exit.hpp"

#include <gtest/gtest.h>

namespace {

class Grid2DTest : public ::testing::Test {
public :
    static const unsigned WIDTH  = 5;
    static const unsigned HEIGHT = 10;
    static const unsigned VALUE  = 0xABC;

    typedef grid2d<int>      grid_t;
    typedef grid_t::position position_t;
};

} //namespace

TEST_F(Grid2DTest, BlockIterators) {
    typedef std::vector<int>::value_type t1;
    typedef std::vector<int const>::value_type t2;

    static_assert(!std::is_same<t1, t2>::value, "hmmm");

    t1 a = 0;
    t2 b = 1;

    a = b;
    b = a;

    grid_t       grid_a;
    auto&        grid_b = grid_a;
    auto const&  grid_c = grid_a;
    grid_t const grid_d;

    typedef grid2d<int const> cgrid_t;

    grid2d<int const>       grid_e;
    auto&         grid_f = grid_e;
    auto const&   grid_g = grid_e;
    cgrid_t const grid_h;

    for (auto const& i : make_block_iterator_adapter(grid_a)) { BK_UNUSED(i); }
    for (auto const& i : make_block_iterator_adapter(grid_b)) { BK_UNUSED(i); }
    for (auto const& i : make_block_iterator_adapter(grid_c)) { BK_UNUSED(i); }
    for (auto const& i : make_block_iterator_adapter(grid_d)) { BK_UNUSED(i); }
    for (auto const& i : make_block_iterator_adapter(grid_e)) { BK_UNUSED(i); }
    for (auto const& i : make_block_iterator_adapter(grid_f)) { BK_UNUSED(i); }
    for (auto const& i : make_block_iterator_adapter(grid_g)) { BK_UNUSED(i); }
    for (auto const& i : make_block_iterator_adapter(grid_h)) { BK_UNUSED(i); }

}

TEST_F(Grid2DTest, DefaultConstructor) {
    grid_t const grid;

    EXPECT_EQ(0, grid.width());
    EXPECT_EQ(0, grid.height());
    EXPECT_EQ(0, grid.size());

    EXPECT_EQ(nullptr, grid.data());

    EXPECT_FALSE(grid.is_valid_position(0, 0));

    ::BK_TEST_BREAK_ON_ASSERT = false;
    {
        BK_ON_SCOPE_EXIT({
            ::BK_TEST_BREAK_ON_ASSERT = true;
        });

        EXPECT_THROW(
            grid.at(position_t(0, 0)),
            assertion_failure
        );
    }

    for (auto const& block : make_block_iterator_adapter(grid)) {
        BK_UNUSED(block);
    }

    for (auto const& i : grid) {
        BK_UNUSED(i);
    }
}

TEST_F(Grid2DTest, BlockIterator) {
    auto const grid = grid_t(WIDTH, HEIGHT, VALUE);

    for (auto const& block : make_block_iterator_adapter(grid)) {
        EXPECT_NE(nullptr, block.here());
        EXPECT_EQ(VALUE, *block.here());

        if (block.x == 0) {
            EXPECT_EQ(nullptr, block.west());
            EXPECT_EQ(nullptr, block.north_west());
            EXPECT_EQ(nullptr, block.south_west());
        } else if (block.x == WIDTH - 1) {
            EXPECT_EQ(nullptr, block.east());
            EXPECT_EQ(nullptr, block.north_east());
            EXPECT_EQ(nullptr, block.south_east());
        }

        if (block.y == 0) {
            EXPECT_EQ(nullptr, block.north());
            EXPECT_EQ(nullptr, block.north_west());
            EXPECT_EQ(nullptr, block.north_east());
        } else if (block.y == HEIGHT - 1) {
            EXPECT_EQ(nullptr, block.south());
            EXPECT_EQ(nullptr, block.south_west());
            EXPECT_EQ(nullptr, block.south_east());
        }
    }
}

TEST_F(Grid2DTest, Swap) {
    static auto const VALUE_A = VALUE;
    static auto const VALUE_B = VALUE + 1;

    auto grid_a = grid_t(WIDTH,  HEIGHT, VALUE_A);
    auto grid_b = grid_t(HEIGHT, WIDTH,  VALUE_B);

    swap(grid_a, grid_b);

    EXPECT_EQ(HEIGHT, grid_a.width());
    EXPECT_EQ(WIDTH,  grid_a.height());

    EXPECT_EQ(WIDTH,  grid_b.width());
    EXPECT_EQ(HEIGHT, grid_b.height());

    for (auto const& i : grid_a) {
        EXPECT_EQ(VALUE_B, i);
    }

    for (auto const& i : grid_b) {
        EXPECT_EQ(VALUE_A, i);
    }
}

TEST_F(Grid2DTest, Clone) {
    auto const grid_a = grid_t(WIDTH, HEIGHT, VALUE);
    auto const grid_b = clone(grid_a);

    EXPECT_EQ(WIDTH, grid_a.width());
    EXPECT_EQ(WIDTH, grid_b.width());

    EXPECT_EQ(HEIGHT, grid_a.height());
    EXPECT_EQ(HEIGHT, grid_b.height());

    for (auto const& i : grid_a) {
        EXPECT_EQ(VALUE, i);
    }

    for (auto const& i : grid_b) {
        EXPECT_EQ(VALUE, i);
    }
}

TEST_F(Grid2DTest, Bounds) {
    auto const grid = grid_t(WIDTH, HEIGHT, VALUE);

    ::BK_TEST_BREAK_ON_ASSERT = false;
    {
        BK_ON_SCOPE_EXIT({
            ::BK_TEST_BREAK_ON_ASSERT = true;
        });

        EXPECT_FALSE(grid.is_valid_position(WIDTH, 0));
        EXPECT_THROW(grid.at(WIDTH, 0), assertion_failure);

        EXPECT_FALSE(grid.is_valid_position(WIDTH, 0));
        EXPECT_THROW(grid.at(0, HEIGHT), assertion_failure);
    }    
    
    for (unsigned y = 0; y < HEIGHT; ++y) {
        for (unsigned x = 0; x < WIDTH; ++x) {
            EXPECT_TRUE(grid.is_valid_position(x, y));
        }
    }
}

TEST_F(Grid2DTest, FillConstructor) {
    auto const gen = [](unsigned x, unsigned y) {
        return x*x + 2*y;
    };

    auto grid = grid_t(WIDTH, HEIGHT, gen);

    EXPECT_EQ(WIDTH,  grid.width());
    EXPECT_EQ(HEIGHT, grid.height());

    for (unsigned y = 0; y < HEIGHT; ++y) {
        for (unsigned x = 0; x < WIDTH; ++x) {
            EXPECT_EQ(gen(x, y), grid.at(x, y));
        }
    }
}

TEST_F(Grid2DTest, MoveConstructor) {
    auto       grid_a = grid_t(WIDTH, HEIGHT, VALUE);
    auto const grid_b = grid_t(std::move(grid_a));

    EXPECT_EQ(0, grid_a.width());
    EXPECT_EQ(0, grid_a.height());

    EXPECT_EQ(WIDTH,  grid_b.width());
    EXPECT_EQ(HEIGHT, grid_b.height());

    for (auto const& i : grid_b) {
        EXPECT_EQ(VALUE, i);
    }
}
