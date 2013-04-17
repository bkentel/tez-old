#pragma once

#include "types.hpp"

class level;

class entity {
    friend level;
public:
    entity(coord_t x, coord_t y, level* level = nullptr);

    void attach(level& level);

    void detach(level& level);

    bool relocate_to(coord_t x, coord_t y);

    bool move_to(direction dir);

    location_t location() const;
protected:
    void set_position_(coord_t x, coord_t y);
    void move_to_(direction dir);

    level*  level_;
    coord_t x_, y_;
};
