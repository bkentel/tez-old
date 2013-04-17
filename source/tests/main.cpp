#include "pch.hpp"

#include <gtest/gtest.h>

#include "../tile_map.hpp"
//#include "../room_compound.hpp"
#include "../room_simple.hpp"
#include "../targa.hpp"

TEST(MapGenerator, RoomCompound) {
    image_targa image(100, 100, image_targa::image_type::rgb24);

    std::default_random_engine gen(1984);

    auto room1 = room_simple::generate(0, 0, gen);

    //room1.write([&](unsigned x, unsigned y, room_part part) {
    //    switch (part) {
    //    case room_part::empty :  std::cout << "empty";  image.set(x, y, 0x00, 0x00, 0x00, 0x00); break;
    //    case room_part::floor :  std::cout << "floor";  image.set(x, y, 0xFF, 0x00, 0x00, 0x00); break;
    //    case room_part::v_edge : std::cout << "v_edge"; image.set(x, y, 0x00, 0xFF, 0x00, 0x00); break;
    //    case room_part::h_edge : std::cout << "h_edge"; image.set(x, y, 0x00, 0x00, 0xFF, 0x00); break;
    //    case room_part::corner_nw : std::cout << "corner"; image.set(x, y, 0xFF, 0xFF, 0x00, 0x00); break;
    //    case room_part::corner_ne : std::cout << "corner"; image.set(x, y, 0xFF, 0x00, 0xFF, 0x00); break;
    //    case room_part::corner_sw : std::cout << "corner"; image.set(x, y, 0x00, 0xFF, 0xFF, 0x00); break;
    //    case room_part::corner_se : std::cout << "corner"; image.set(x, y, 0xFF, 0xFF, 0xFF, 0x00); break;
    //    }
    //});

    image.save("room.tga");
}

TEST(TileIterator, IteratorDistance) {
    static auto const w = 10;
    static auto const h = 10;

    tile_grid grid(w, h);
    
    {
        auto const region = grid.csub_region(0, 0, w, h);

        EXPECT_EQ(
            std::distance(region.cbegin(), region.cend()),
            grid.width() * grid.height()
        );
    }

    {
        auto const region = grid.csub_region(1, 1, 1, 1);

        EXPECT_EQ(
            std::distance(region.cbegin(), region.cend()),
            1
        );
    }

}

TEST(TileIterator, Iterator) {
    static auto const w = 10;
    static auto const h = 10;

    tile_grid grid(w, h);

    static auto const xo = 1;
    static auto const yo = 1;
    static auto const ws = 3;
    static auto const hs = 3;

    auto region = grid.sub_region(xo, yo, ws, hs);

    unsigned const values[][hs] = {
        0x100, 0x101, 0x102,
        0x110, 0x111, 0x112,
        0x120, 0x121, 0x122,
    };

    for (unsigned y = 0; y < hs; ++y) {
        for (unsigned x = 0; x < ws; ++x) {
            region(x, y).texture_id = values[x][y];
        }
    }

    // use an iterator to check the values
    auto it = region.cbegin();
    for (unsigned y = 0; y < hs; ++y) {
        for (unsigned x = 0; x < ws; ++x) {
            EXPECT_EQ(values[x][y], it->texture_id);
            ++it;
        }
    }

    // directly index into the tile_grid to check the values
    for (unsigned y = yo; y < yo + hs; ++y) {
        for (unsigned x = xo; x < xo + ws; ++x) {
            EXPECT_EQ(values[x - xo][y - yo], grid(x, y).texture_id);
        }
    }
}

TEST(TileGrid, Dimensions) {
    // Test the dimension functions.

    static auto const w = 10;
    static auto const h = 10;

    tile_grid grid(w, h);

    EXPECT_EQ(w, grid.width());
    EXPECT_EQ(h, grid.height());
}

TEST(TileGrid, Constructor) {
    EXPECT_THROW(tile_grid(0, 0), assertion_failure);
    EXPECT_THROW(tile_grid(0, 1), assertion_failure);
    EXPECT_THROW(tile_grid(1, 0), assertion_failure);
}

TEST(TileGrid, FunctionOperator) {
    static auto const w = 10;
    static auto const h = 10;

    tile_grid grid(w, h);

    EXPECT_THROW(grid(w, h), assertion_failure);
    EXPECT_THROW(grid(w, 0), assertion_failure);
    EXPECT_THROW(grid(0, h), assertion_failure);
}

TEST(TileGrid, SubRegion) {
    static auto const w = 10;
    static auto const h = 10;

    tile_grid grid(w, h);

    EXPECT_THROW(grid.sub_region(w, h, 1,   1), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, h, 1,   1), assertion_failure);
    EXPECT_THROW(grid.sub_region(w, 0, 1,   1), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, 0,   0), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, w,   0), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, 0,   h), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, w+1, h), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, w,   h+1), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, w+1, h+1), assertion_failure);
}

TEST(TileRegion, RegionAssign) {
    // Test the assigment operator.

    tile_grid grid(10, 10);

    auto r1 = grid.sub_region(1, 1, 9, 9);
    auto r2 = grid.sub_region(2, 2, 8, 8);
    auto r3 = grid.csub_region(3, 3, 7, 7);
    auto r4 = grid.csub_region(4, 4, 6, 6);

    // mutable to mutable
    r1 = r2;
    EXPECT_EQ(r1, r2);

    // mutable to const
    r3 = r1;
    EXPECT_EQ(r3, r1);

    // const to const
    r4 = r3;
    EXPECT_EQ(r4, r3);
}

TEST(TileRegion, RegionCopyConstr) {
    // Test the copy constructor.

    tile_grid grid(10, 10);

    // mutable to mutable
    auto r1 = grid.sub_region(1, 1, 9, 9);
    tile_grid::region r2(r1);
    EXPECT_EQ(r1, r2);

    // const to const
    auto r3 = grid.csub_region(3, 3, 7, 7);
    tile_grid::const_region r4(r3);
    EXPECT_EQ(r3, r4);

    // mutable to const
    tile_grid::const_region r5(r1);
    EXPECT_EQ(r1, r5);
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    RUN_ALL_TESTS();

    return 0;
}
