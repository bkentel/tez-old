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

    switch (search_dir) {
    case direction::north : {
        unsigned const x = std::uniform_int_distribution<>(0, w-1)(random);

        for (unsigned y = h; y != 0; --y) {
            if (at(x, y-1) != tile_category::empty) return point_t(x, y-1);
        }
    } break;
    case direction::south : {
        unsigned const x = std::uniform_int_distribution<>(0, w-1)(random);

        for (unsigned y = 0; y != h; ++y) {
            if (at(x, y) != tile_category::empty) return point_t(x, y);
        }
    } break;
    case direction::east : {
        unsigned const y = std::uniform_int_distribution<>(0, h-1)(random);

        for (unsigned x = 0; x != w; ++x) {
            if (at(x, y) != tile_category::empty) return point_t(x, y);
        }
    } break;
    case direction::west : {
        unsigned const y = std::uniform_int_distribution<>(0, h-1)(random);

        for (unsigned x = w; x != 0; --x) {
            if (at(x-1, y) != tile_category::empty) return point_t(x-1, y);
        }
    } break;
    default :
        BK_ASSERT(false);
    }

    BK_ASSERT(false);
    return point_t(0, 0);
}