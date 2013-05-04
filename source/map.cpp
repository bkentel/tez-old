#include "pch.hpp"
#include "map.hpp"


map::map(unsigned width, unsigned height)
    : data_(width, height)
{
}

void map::add_room(room const& r, signed dx, signed dy) {
    grid_copy_transform(
        r,
        0, 0,
        r.width(), r.height(),
        data_,
        r.left() + dx, r.top() + dy,
        [](tile_category const& src_cat, tile_data& dest_data) {
            dest_data.type = src_cat;
        }
    );
}

std::ostream& operator<<(std::ostream& out, map const& m) {
    out << "map";

    auto const h = m.height();
    auto const w = m.width();

    for (unsigned y = 0; y < h; ++y) {
        std::cout << std::endl;

        for (unsigned x = 0; x < w; ++x) {
            std::cout << static_cast<char>(m.data_.at(x, y).type);
        }
    }
    
    std::cout << std::endl;

    return out;
}
