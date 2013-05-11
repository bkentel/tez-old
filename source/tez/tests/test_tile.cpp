#include "pch.hpp"
#include "tez/tile.hpp"

#include <gtest/gtest.h>

TEST(Tile, Constructor) {
    using namespace tez;

    tile_data tile;

    auto door = tile.get_data<door_data>();

}
