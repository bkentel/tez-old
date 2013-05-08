#pragma once

#include "util.hpp"
#include "tile_category.hpp"
#include "grid2d.hpp"
#include "geometry.hpp"
#include "room.hpp"

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

    room generate();
};

//==============================================================================
//! Generate a room randomly shaped by a drunken walk.
//==============================================================================
class compound_room_generator : public generator {
public:
    compound_room_generator(random_t random);

    room generate();
private:
    typedef point2d<signed> point_t;
    
    grid_t make_compound_room_base_();
    
    std::vector<point_t> points_; //list of occupied points
};

class path_generator {
public:
    explicit path_generator(random_wrapper<unsigned> random);

    bool generate(room const& origin, map const& m, direction dir);

    void write_path(map& out);
private:
    typedef std::discrete_distribution<unsigned> distribution_t;
    
    distribution_t path_dist_n_;
    distribution_t path_dist_s_;
    distribution_t path_dist_e_;
    distribution_t path_dist_w_;

    typedef point2d<unsigned> point_t;

    std::vector<point_t> path_;
    random_wrapper<unsigned> random_;
};
