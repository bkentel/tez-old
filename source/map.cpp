#include "pch.hpp"
#include "map.hpp"


map::map(unsigned width, unsigned height)
    : data_(width, height)
{
}

void map::write(room const& r, signed dx, signed dy) {
    data_.transfer(r.data_, 0, 0, r.width(), r.height(), r.left() + dx, r.top() + dy,
        [](tile_category c) {
            tile_data result;
            result.type = c;
            return result;
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
