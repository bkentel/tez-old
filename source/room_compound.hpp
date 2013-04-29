#pragma once

#include "room.hpp"
#include "geometry.hpp"
#include "util.hpp"

#include <vector>

//==============================================================================
//! Generate a room randomly shaped by a drunken walk.
//==============================================================================
class compound_room_generator {
public:
    compound_room_generator(random_wrapper<unsigned> random);

    template <typename T>
    compound_room_generator(T& random)
        : compound_room_generator(random_wrapper<unsigned>(random))
    {
    }

    unsigned      width()   const { return cell_size_*(range_x_.distance()+1); }
    unsigned      height()  const { return cell_size_*(range_y_.distance()+1); }
    tile_category default() const { return tile_category::empty; }

    void generate(grid2d<tile_category>& out);   
private:
    typedef point2d<signed> point_t;

    random_wrapper<unsigned> random_;
    unsigned             cell_size_;
    unsigned             cell_count_;
    min_max<signed>      range_x_;
    min_max<signed>      range_y_;
    std::vector<point_t> points_;
};
