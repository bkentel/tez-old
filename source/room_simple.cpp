#include "pch.hpp"
#include "room_simple.hpp"

namespace {
    typedef std::uniform_int_distribution<unsigned> dist_t;

    static auto const MIN_W = 3;
    static auto const MAX_W = 10;
    static auto const MIN_H = 3;
    static auto const MAX_H = 10;

    
}

simple_room_generator::simple_room_generator(random_wrapper<unsigned> random)
    : random_(random)
    , width_(dist_t(MIN_W, MAX_W)(random_))
    , height_(dist_t(MIN_H, MAX_H)(random_))
{
}

void simple_room_generator::generate(grid2d<tile_category>& out) {
    static auto const fallback = tile_category::empty;

    std::for_each(
        out.block_begin(fallback),
        out.block_end(fallback),
        [&](block<tile_category> const& b) {
            auto const x = b.x;
            auto const y = b.y;

            if ((x == 0) || (y == 0) ||
                (x == width_ - 1) || (y == height_ - 1)
            ) {
                out.set(x, y, tile_category::ceiling);
            } else if(b.n == tile_category::ceiling) {
                out.set(x, y, tile_category::wall);
            }
        }
    );
}
