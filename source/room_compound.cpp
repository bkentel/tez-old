#include "pch.hpp"
#include "room_compound.hpp"

namespace {
    typedef std::uniform_int_distribution<unsigned> dist_t;

    static auto const CELL_SIZE_MIN = 4;
    static auto const CELL_SIZE_MAX = 6;

    static auto const COUNT_MIN = 10;
    static auto const COUNT_MAX = 20;
}

compound_room_generator::compound_room_generator(random_wrapper<unsigned> random)
    : random_(random)
    , cell_size_(dist_t(CELL_SIZE_MIN, CELL_SIZE_MAX)(random_))
    , cell_count_(dist_t(COUNT_MIN, COUNT_MAX)(random_))
{
    //current position.
    point_t p(0, 0);
    
    //uniform distribution over [0, 3].
    dist_t dist(0, 3);

    //true if the position given by p has already been used.
    auto const is_occupied = [&](point_t const p) {
        return std::find(
            std::begin(points_), std::end(points_), p
        ) != std::end(points_);
    };
    
    //get a point offset in a cardinal direction from p
    auto const get_point = [](point_t const p, unsigned const index) -> point_t {
        switch (index) {
        case 0 : return point_t(p.x + 1, p.y + 0);
        case 1 : return point_t(p.x - 1, p.y + 0);
        case 2 : return point_t(p.x + 0, p.y + 1);
        case 3 : return point_t(p.x + 0, p.y - 1);
        }

        return p;
    };

    while (points_.size() < cell_count_) {
        auto const i = dist(random_);

        auto const p0 = get_point(p, (i + 0) % 4);
        auto const p1 = get_point(p, (i + 1) % 4);
        auto const p2 = get_point(p, (i + 2) % 4);
        auto const p3 = get_point(p, (i + 3) % 4);

        auto const q = 
            !is_occupied(p0) ? p0 :
            !is_occupied(p1) ? p1 :
            !is_occupied(p2) ? p2 :
            !is_occupied(p3) ? p3 : p;

        //no move was possible -- stop looping
        if (q == p) {
            break;
        }
            
        points_.push_back(p = q);
        range_x_(p.x);
        range_y_(p.y);
    }
}

void compound_room_generator::generate(grid2d<tile_category>& out) {
    for (auto const p : points_) {
        for (auto yi = 0U; yi < cell_size_; ++yi) {
            for (auto xi = 0U; xi < cell_size_; ++xi) {
                out.set(
                    xi + (p.x - range_x_.min) * cell_size_,
                    yi + (p.y - range_y_.min) * cell_size_,
                    tile_category::floor
                );
            }
        }
    }

    typedef grid2d<tile_category>::block_iterator it;
    static auto const fallback = tile_category::empty;

    std::cout << std::endl;

    auto const ceiling_rule = [](block<tile_category> const& b) {       
        if (b.n  == tile_category::empty) return true;
        if (b.s  == tile_category::empty) return true;
        if (b.e  == tile_category::empty) return true;
        if (b.w  == tile_category::empty) return true;

        //all of nsew are not empty
        if (b.nw == tile_category::empty) return true;
        if (b.ne == tile_category::empty) return true;
        if (b.sw == tile_category::empty) return true;
        if (b.se == tile_category::empty) return true;

        return false;
    };

    std::for_each(out.block_begin(fallback), out.block_end(fallback), [&](block<tile_category> const& b) {
        auto const x = b.x;
        auto const y = b.y;

        if (b.here == tile_category::empty) {
        } else if (ceiling_rule(b)) {
            out.set(x, y, tile_category::ceiling);
        } else if (b.n == tile_category::ceiling) {
            out.set(x, y, tile_category::wall);
        }

        if (x == 0) std::cout << "\n";
        std::cout << static_cast<char>(out.at(x, y));
    });

    std::cout << std::endl;
}
