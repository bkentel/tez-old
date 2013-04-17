#pragma once

#include "types.hpp"

class entity;

////////////////////////////////////////////////////////////////////////////////
class level {
public:
    level() : w_(100), h_(100) {}

    bool can_relocate_to(entity const& ent, coord_t x, coord_t y) const;
    bool can_move_to(entity const& ent, direction dir) const;
private:
    coord_t w_, h_;
};
