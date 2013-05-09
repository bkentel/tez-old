#include "pch.hpp"
#include "room.hpp"

using tez::room;

////////////////
// Find a location where a door can be placed on the [side] side of the room.
// Rooms must have axis aligned walls.
// Rooms must be at least 3x3.
////////////////
room::point_t room::find_connectable_point(
    random_t             random,
    tez::direction const side
) const {
    static auto const TARGET = tile_category::ceiling;

    auto const search_dir = opposite_direction(side);
    
    auto const w = width()  - 1;
    auto const h = height() - 1;

    unsigned x = (search_dir == direction::west) ? w :
                 (search_dir == direction::east) ? 0 :
                  std::uniform_int_distribution<>(0, w)(random);

    unsigned y = (search_dir == direction::north) ? h :
                 (search_dir == direction::south) ? 0 :
                  std::uniform_int_distribution<>(0, h)(random);

    auto const is_ns =
        (search_dir == direction::north || search_dir == direction::south);

    auto&      primary        = is_ns ? y : x;
    auto&      secondary      = is_ns ? x : y;
    auto const primary_size   = is_ns ? height() : width();
    auto const secondary_size = is_ns ? width()  : height();
    
    auto const delta = is_ns ? direction_vector(search_dir).second :
                               direction_vector(search_dir).first;

    //--------------------------------------------------------------------------
    auto const is_in_bounds = [&](unsigned const x, unsigned const y) {
        return is_ns ? ((y > 0) && (y < h)) : ((x > 0) && (x < w));        
    };
    //--------------------------------------------------------------------------
    auto const is_valid_pos = [&] {
        if (x > w || y > h)     return false;
        if (at(x, y) != TARGET) return false;

        primary += delta;
        
        auto const c = at(x, y);
        auto const result = (c == tile_category::floor) ||
            ((c == tile_category::wall) && (search_dir == direction::south));
        
        primary -= delta;

        return result;
    };
    //--------------------------------------------------------------------------

    //scan along the primary direction
    while (is_in_bounds(x, y) && (at(x, y) == TARGET)) {
        primary += delta;
    }
    BK_ASSERT(at(x, y) == TARGET);
                
    //scan along the secondary direction
    while (!is_valid_pos()) {
        secondary = (secondary + 1) % secondary_size;
    }
    BK_ASSERT(at(x, y) == TARGET);

    return point_t(x + left(), y + top());
}
