#include "pch.hpp"
#include "room.hpp"
#include "util.hpp"

room::point_t room::find_connectable_point(
    random_wrapper<unsigned> random,
    direction const dir
) const {

    auto const search_dir = opposite(dir);
    
    auto const w = width();
    auto const h = height();

    auto const translated_point = [&](unsigned x, unsigned y) {
        return point_t(x + left(), y + top());
    };

    unsigned x = (search_dir == direction::west) ? w-1 :
                    (search_dir == direction::east) ? 0 :
                    std::uniform_int_distribution<>(0, w-1)(random);

    unsigned y = (search_dir == direction::north) ? h-1 :
                    (search_dir == direction::south) ? 0 :
                    std::uniform_int_distribution<>(0, h-1)(random);

    unsigned& primary   = (search_dir == direction::north || search_dir == direction::south) ?
        y : x;
    unsigned& secondary = (search_dir == direction::north || search_dir == direction::south) ?
        x : y;
    unsigned const secondary_size = (search_dir == direction::north || search_dir == direction::south) ?
        w : h;

    signed const delta = (search_dir == direction::north || search_dir == direction::west) ?
        -1 : 1;

    auto const is_in_bounds = [&](unsigned x, unsigned y) {
        return (delta < 0 && x > 0 && y > 0) || (delta >= 0 && x < w-1 && y < h-1);
    };

    auto const is_valid_pos = [&] {
        if (x >= w || y >= h) return false;
            
        if (at(x, y) != tile_category::ceiling) return false;

        primary += delta;
        auto const c = at(x, y);
        auto const result = (c == tile_category::floor) || (c == tile_category::wall && search_dir == direction::south);
        primary -= delta;

        return result;
    };

    //scan along the primary direction
    for (; is_in_bounds(x, y); primary += delta) {
        if (at(x, y) == tile_category::ceiling) {
            break;
        }
    }
                
    //scan along the secondary direction
    while (!is_valid_pos()) {
        secondary = (secondary + 1) % secondary_size;
    }
            
    auto const value = at(x, y);
    BK_ASSERT(value == tile_category::ceiling);

    return translated_point(x, y);
}