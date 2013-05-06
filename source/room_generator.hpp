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

    bool generate(room const& origin, map const& m);

    void write_path(map& out);
private:
    typedef std::discrete_distribution<unsigned> distribution_t;
    
    distribution_t path_dist_n_;
    distribution_t path_dist_s_;
    distribution_t path_dist_e_;
    distribution_t path_dist_w_;

    std::vector<point2d<unsigned>> path_;
    random_wrapper<unsigned> random_;

    //typedef std::discrete_distribution<unsigned> dist_t;

    //explicit path_generator(direction const dir) {
    //    #define BK_MAKE_DIST(dir) \
    //    case direction::dir : { \
    //        auto gen = path_generator_dist<direction::dir>(); \
    //        dist_ = dist_t(4, 0, 4, gen); \
    //    } break

    //    switch (dir) {
    //    BK_MAKE_DIST(north);
    //    BK_MAKE_DIST(south);
    //    BK_MAKE_DIST(east);
    //    BK_MAKE_DIST(west);
    //    }

    //    #undef BK_MAKE_DIST
    //};

    //static direction to_direction(unsigned const i) {
    //    switch (i) {
    //    case 0 : return direction::north;
    //    case 1 : return direction::south;
    //    case 2 : return direction::east;
    //    case 3 : return direction::west;
    //    }

    //    BK_ASSERT(false);

    //    return direction::none;
    //}

    //template <typename T>
    //direction operator()(T& random) {
    //    return to_direction(dist_(random));
    //}

    //dist_t dist_;
};
