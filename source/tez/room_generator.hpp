#pragma once

#include "bklib/util.hpp"
#include "bklib/geometry.hpp"

#include "tile_category.hpp"
#include "grid2d.hpp"
#include "room.hpp"

#include <vector>

namespace tez {

class generator {
public:
    typedef bklib::random_wrapper<unsigned> random_t;
    typedef grid2d<tile_category>           grid_t;
    typedef bklib::point2d<unsigned>        connection_point;

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

    static connection_point find_connection_point(direction side, random_t random);
};

//==============================================================================
//! Generate a room randomly shaped by a drunken walk.
//==============================================================================
class compound_room_generator : public generator {
public:
    typedef bklib::point2d<signed> point_t;

    compound_room_generator(random_t random);

    room generate();

    static connection_point find_connection_point(direction side, random_t random);
private:
    
    
    grid_t make_compound_room_base_();
    
    std::vector<point_t> points_; //list of occupied points
};


} //namespace tez
