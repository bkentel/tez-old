#pragma once

#include <string>
#include <cstdint>

typedef std::string utf8str;
typedef unsigned    coord_t;
typedef signed      difference_t;

struct location_t {
    coord_t x, y, z;
};

struct delta_t {
    difference_t dx, dy, dz;
};

//enum class direction {
//    here       = 0,
//    none       = 0,
//    north      = 1 << 0,
//    south      = 1 << 1,
//    east       = 1 << 2,
//    west       = 1 << 3,
//    north_east = 1 << 4,
//    north_west = 1 << 5,
//    south_east = 1 << 6,
//    south_west = 1 << 7,
//    up         = 1 << 8,
//    down       = 1 << 9,
//};

