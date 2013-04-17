#include "pch.hpp"
#include "room_compound.hpp"

room_compound::room_compound(unsigned x, unsigned y, unsigned size, unsigned cell_size)
    : x_(x)
    , y_(y)
    , size_(size)
    , cell_size_(cell_size)
    , cells_(new cell_t[size*size])
{
}

room_compound::room_compound(room_compound const& other)
    : x_(other.x_)
    , y_(other.y_)
    , size_(other.size_)
    , cell_size_(other.cell_size_)
    , cells_(new cell_t[size_*size_])
{
    std::copy_n(other.cells_.get(), size_*size_, cells_.get());
}

room_compound room_compound::generate_(
    unsigned const x,
    unsigned const y,
    unsigned const size,
    unsigned const cell_size,
    unsigned const ox,
    unsigned const oy,
    std::function<unsigned ()> const generator
) {
    room_compound result(x, y, size, cell_size);

    std::vector<std::pair<unsigned, unsigned>> history;
    history.emplace_back(ox, oy);

    while (!history.empty()) {
        auto const where = history.back();
        history.pop_back();

        auto const x = where.first;
        auto const y = where.second;

        bool const gen_north = generator() < 2;
        bool const gen_south = generator() < 2;
        bool const gen_east  = generator() < 2;
        bool const gen_west  = generator() < 2;

        bool const go_north = gen_north && (y > 0)      && result.at_(x,   y-1).empty;
        bool const go_west  = gen_west  && (x > 0)      && result.at_(x-1, y  ).empty;
        bool const go_south = gen_south && (y < size-1) && result.at_(x,   y+1).empty;
        bool const go_east  = gen_east  && (x < size-1) && result.at_(x+1, y  ).empty;

        result.at_(x, y).empty = false;

        if (go_north) history.emplace_back(x,   y-1);
        if (go_west)  history.emplace_back(x-1, y  );
        if (go_south) history.emplace_back(x,   y+1);
        if (go_east)  history.emplace_back(x+1, y  );
    }

    for (unsigned y = 0; y < result.size_; ++y) {
        for (unsigned x = 0; x < result.size_; ++x) {
            std::cout << (result.at_(x, y).empty ? "O" : "X");
        }
        std::cout << std::endl;
    }

    return result;

}

room_part room_compound::get_room_type_(
    unsigned const  x,
    unsigned const  y,
    direction const dx,
    direction const dy
) const {
    if (dx == dy) {
        assert(dx == direction::none);
        assert(dy == direction::none);

        return room_part::floor;
    }
    
    auto const filled = [this](unsigned const xi, unsigned yi) {
        return !at_(xi, yi).empty;
    };

    auto const has_neighbor_n  = [&] {return (y > 0      ) && filled(x,   y-1);};
    auto const has_neighbor_s  = [&] {return (y < size_-1) && filled(x,   y+1);};
    auto const has_neighbor_w  = [&] {return (x > 0      ) && filled(x-1, y  );};
    auto const has_neighbor_e  = [&] {return (x < size_-1) && filled(x+1, y  );};
    auto const has_neighbor_ne = [&] {return (y > 0)       && (x < size_-1) && filled(x+1, y-1);};
    auto const has_neighbor_nw = [&] {return (y > 0)       && (x > 0)       && filled(x-1, y-1);};
    auto const has_neighbor_se = [&] {return (y < size_-1) && (x < size_-1) && filled(x+1, y+1);};
    auto const has_neighbor_sw = [&] {return (y < size_-1) && (x > 0)       && filled(x-1, y+1);};

    static auto const N  = BK_ENUM_VALUE(direction::north);
    static auto const S  = BK_ENUM_VALUE(direction::south);
    static auto const E  = BK_ENUM_VALUE(direction::east);
    static auto const W  = BK_ENUM_VALUE(direction::west);
    static auto const NE = BK_ENUM_VALUE(direction::north_east);
    static auto const NW = BK_ENUM_VALUE(direction::north_west);
    static auto const SE = BK_ENUM_VALUE(direction::south_east);
    static auto const SW = BK_ENUM_VALUE(direction::south_west);

    auto const border_value = get_enum_value(dy) | get_enum_value(dx);

    switch (border_value) {
    case N  : return has_neighbor_n() ? room_part::floor : room_part::h_edge;
    case E  : return has_neighbor_e() ? room_part::floor : room_part::v_edge;
    case W  : return has_neighbor_w() ? room_part::floor : room_part::v_edge;
    case S  : return has_neighbor_s() ? room_part::floor : room_part::h_edge;
    case N | E : {
        auto const neighbor_value =
            (has_neighbor_n()  ? N  : 0) |
            (has_neighbor_e()  ? E  : 0) |
            (has_neighbor_ne() ? NE : 0);

        switch (neighbor_value) {
        case 0          :
        case NE         : return room_part::corner_sw;
        case N          : 
        case N | NE     : return room_part::v_edge;
        case E          : 
        case E | NE     : return room_part::h_edge;
        case N | E      : return room_part::corner_ne;
        case N | E | NE : return room_part::floor;
        }
    }
    case N | W : {
        auto const neighbor_value =
            (has_neighbor_n()  ? N  : 0) |
            (has_neighbor_w()  ? W  : 0) |
            (has_neighbor_nw() ? NW : 0);

        switch (neighbor_value) {
        case 0          :
        case NW         : return room_part::corner_se;
        case N          : 
        case N | NW     : return room_part::v_edge;
        case W          : 
        case W | NW     : return room_part::h_edge;
        case N | W      : return room_part::corner_nw;
        case N | W | NW : return room_part::floor;
        }
    }
    case S | E : {
        auto const neighbor_value =
            (has_neighbor_s()  ? S  : 0) |
            (has_neighbor_e()  ? E  : 0) |
            (has_neighbor_se() ? SE : 0);

        switch (neighbor_value) {
        case 0          :
        case SE         : return room_part::corner_nw;
        case S          : 
        case S | SE     : return room_part::v_edge;
        case E          : 
        case E | SE     : return room_part::h_edge;
        case S | E      : return room_part::corner_se;
        case S | E | SE : return room_part::floor;
        }
    }
    case S | W : {
        auto const neighbor_value =
            (has_neighbor_s()  ? S  : 0) |
            (has_neighbor_w()  ? W  : 0) |
            (has_neighbor_sw() ? SW : 0);

        switch (neighbor_value) {
        case 0          :
        case SW         : return room_part::corner_ne;
        case S          : 
        case S | SW     : return room_part::v_edge;
        case W          : 
        case W | SW     : return room_part::h_edge;
        case S | W      : return room_part::corner_sw;
        case S | W | SE : return room_part::floor;
        }
    }
    }

    return room_part::floor;
}

rect<unsigned> room_compound::get_rect_() const {
    unsigned left   = size_;
    unsigned top    = size_;
    unsigned right  = 0;
    unsigned bottom = 0;

    for_each_xy(
        0U, size_,
        [&](unsigned const x, unsigned const y) {
            if (at_(x, y).empty) return;

            left   = x < left   ? x : left;
            top    = y < top    ? y : top;
            right  = x > right  ? x : right;
            bottom = y > bottom ? y : bottom;
        },
        0U, size_,
        [](unsigned, unsigned) {}
    );

    BK_ASSERT(right  >= left);
    BK_ASSERT(bottom >= top);

    rect<unsigned> result = {left, top, right, bottom};
    return result;
}

void room_compound::write(write_f out) const {
    auto rect = get_rect_();

    auto const fill_f = [&](unsigned const x, unsigned const y) {
        auto const x_off = x*cell_size_;
        auto const y_off = y*cell_size_;

        direction side_x = direction::none;
        direction side_y = direction::none;

        for_each_xy(
            0U, cell_size_,
            [&](unsigned const xi, unsigned const yi) {
                side_x =
                    (xi == 0)            ? direction::west :
                    (xi == cell_size_-1) ? direction::east :
                                           direction::none;

                out(
                    xi + x_off,
                    yi + y_off,
                    get_room_type_(
                        rect.left + x,
                        rect.top  + y,
                        side_x,
                        side_y
                    )
                );
            },
            0U, cell_size_,
            [&](unsigned, unsigned const yi) {
                side_y =
                    (yi == 0)            ? direction::north :
                    (yi == cell_size_-1) ? direction::south :
                                           direction::none;
            }
        );
    };

    for_each_xy(
        rect.left, rect.right + 1,
        [&](unsigned const xi, unsigned const yi) {
            if (!at_(xi, yi).empty) {
                fill_f(xi - rect.left, yi - rect.top);
            }
        },
        rect.top, rect.bottom + 1,
        [](unsigned, unsigned) {}
    );
}
