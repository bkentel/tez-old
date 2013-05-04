#pragma once

#include "util.hpp"
#include "tile_category.hpp"
#include "grid2d.hpp"
#include "geometry.hpp"

class generator {
public:
    typedef random_wrapper<unsigned> random_t;
    typedef grid2d<tile_category>    grid_t;

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

    grid_t generate();
};

//==============================================================================
//! Generate a room randomly shaped by a drunken walk.
//==============================================================================
class compound_room_generator : public generator {
public:
    compound_room_generator(random_t random);

    grid_t generate();
private:

};
