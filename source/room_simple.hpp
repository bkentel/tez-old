#pragma once

#include "room.hpp"
#include "util.hpp"

//==============================================================================
//! Generate a simple rectangular room.
//==============================================================================
class simple_room_generator {
public:
    simple_room_generator(random_wrapper<unsigned> random);

    template <typename T>
    simple_room_generator(T& random)
        : simple_room_generator(random_wrapper<unsigned>(random))
    {
    }

    unsigned      width()   const { return width_; }
    unsigned      height()  const { return height_; }
    tile_category default() const { return tile_category::floor; }

    void generate(grid2d<tile_category>& out);
private:
    random_wrapper<unsigned> random_;
    unsigned width_;
    unsigned height_;
};
