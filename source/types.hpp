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
