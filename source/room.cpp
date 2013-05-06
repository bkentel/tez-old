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

    for(;;) {
    unsigned x = (search_dir == direction::west) ? w-1 :
                 (search_dir == direction::east) ? 0 :
                 std::uniform_int_distribution<>(0, w-1)(random);

    unsigned y = (search_dir == direction::north) ? h-1 :
                 (search_dir == direction::south) ? 0 :
                 std::uniform_int_distribution<>(0, h-1)(random);

    unsigned& var = (search_dir == direction::north || search_dir == direction::south) ?
        y : x;

    signed const delta = (search_dir == direction::north || search_dir == direction::west) ?
        -1 : 1;

    auto const is_in_bounds = [&](unsigned x, unsigned y) {
        return x >= 0 && y >= 0 && x < width() && y < height();
    };

    for (; is_in_bounds(x, y); var += delta) {
        if (at(x, y) != tile_category::ceiling) continue;

        var += delta;

        if (at(x, y) == tile_category::floor || (
                at(x, y) == tile_category::wall && search_dir == direction::south
            )
        ) {
            var -= delta;
            return translated_point(x, y);
        }
        
        std::cout << "fail with x, y = " << x << ", " << y << std::endl;
        break;
    }
    }

    return translated_point(0, 0);
}