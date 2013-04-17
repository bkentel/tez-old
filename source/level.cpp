#include "pch.hpp"
#include "level.hpp"

#include "entity.hpp"

bool level::can_relocate_to(entity const& ent, coord_t const x, coord_t const y) const {
    return true;
}

bool level::can_move_to(entity const& ent, direction const dir) const {   
    auto const loc   = ent.location();
    auto const delta = get_delta(dir);

    BK_ASSERT(std::abs(delta.dx) <= 1);
    BK_ASSERT(std::abs(delta.dy) <= 1);
    BK_ASSERT(std::abs(delta.dz) <= 1);

    auto const check_move = [](coord_t value, coord_t min, coord_t max, difference_t change) {
        switch (change) {
        case  1 : return value < max;
        case  0 : return true;
        case -1 : return value > min;
        default : BK_ASSERT(false);
        }

        return false;
    };

    return check_move(loc.x, 0, w_, delta.dx) &&
           check_move(loc.y, 0, h_, delta.dy) &&
           check_move(loc.z, 0, 0, delta.dz);
}
