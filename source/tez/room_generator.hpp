#pragma once

#include "bklib/util.hpp"
#include "bklib/geometry.hpp"

#include "tile_category.hpp"
#include "room.hpp"

#include <vector>

namespace tez {

class generator {
public:
    typedef bklib::random_wrapper<> random_t;
    typedef grid2d<tile_category>   grid_t;
    typedef room::connection_point  connection_point;

    generator(random_t random) : random_(random) {}
protected:
    random_t random_;
};

//==============================================================================
//! Generate a simple rectangular room.
//==============================================================================
class simple_room_generator : public generator {
public:
    simple_room_generator(random_t random);

    room generate();

    static connection_point find_connection_point(
        room const& room, direction side, random_t random
    );
};

//==============================================================================
//! Generate a room randomly shaped by a drunken walk.
//==============================================================================
class compound_room_generator : public generator {
public:
    compound_room_generator(random_t random);

    room generate();

    static connection_point find_connection_point(
        room const& room, direction side, random_t random
    );
private:    
    typedef bklib::point2d<signed> point_t;
    std::vector<point_t> points_; //list of occupied points
};


} //namespace tez
