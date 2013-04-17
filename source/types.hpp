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

template <typename T>
struct rect {
    T left, top, right, bottom;

    T width()  const { return right - left; }
    T height() const { return bottom - top; }
};

enum class direction : uint16_t {
    here       = 0,
    none       = 0,
    north      = 1 << 0,
    south      = 1 << 1,
    east       = 1 << 2,
    west       = 1 << 3,
    north_east = 1 << 4,
    north_west = 1 << 5,
    south_east = 1 << 6,
    south_west = 1 << 7,
    up         = 1 << 8,
    down       = 1 << 9,
};

static inline delta_t get_delta(direction const dir) {
    signed dx = 0;
    signed dy = 0;
    signed dz = 0;
    
    switch (dir) {
    case direction::here :
        break;
    case direction::north_east :
        dx = 1;
    case direction::north :
        dy = -1;
        break;
    case direction::south_east :
        dy = 1;
    case direction::east :
        dx = 1;
        break;
    case direction::south_west :
        dx = -1;
    case direction::south :
        dy = 1;
        break;
    case direction::north_west :
        dy = -1;
    case direction::west :
        dx = -1;
        break;
    case direction::up :
        dz = -1;
        break;
    case direction::down :
        dz = 1;
        break;
    }

    delta_t const result = {dx, dy, dz};

    return result;
}
