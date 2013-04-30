#include "pch.hpp"
#include "tile_map.hpp"

tile_grid::tile_grid(unsigned width, unsigned height)
    : data_(width, height)
{
}

void tile_grid::write(room const& r) {
    data_.transfer(r.data_, 0, 0, r.width(), r.height(), r.left(), r.top(),
        [](tile_category c) {
            tile_data result;
            result.type = c;
            return result;
        }
    );
}
