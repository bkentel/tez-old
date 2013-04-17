#include "pch.hpp"
#include "room_compound.hpp"

room_compound::room_compound(unsigned x, unsigned y, unsigned size, unsigned cell_size)
    : x_(x)
    , y_(y)
    , size_(size)
    , cell_size_(cell_size)
    , rect_(0, 0, 0, 0)
    , cells_(new room_part[size*cell_size*size*cell_size])
{
    BK_ASSERT(size >= MIN_CELLS);
    BK_ASSERT(size <= MAX_CELLS);
    
    BK_ASSERT(cell_size >= MIN_CELL_DIM);
    BK_ASSERT(cell_size <= MAX_CELL_DIM);

    std::fill_n(cells_.get(), size*cell_size*size*cell_size, room_part::empty);
}

room_compound::room_compound(room_compound const& other)
    : x_(other.x_)
    , y_(other.y_)
    , size_(other.size_)
    , cell_size_(other.cell_size_)
    , rect_(other.rect_)
    , cells_(new room_part[size_*size_])
{
    std::copy_n(other.cells_.get(), size_*size_*cell_size_, cells_.get());
}

room_compound::room_compound(room_compound&& other)
    : x_(other.x_)
    , y_(other.y_)
    , size_(other.size_)
    , cell_size_(other.cell_size_)
    , rect_(other.rect_)
    , cells_(std::move(other.cells_))
{
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

    unsigned min_x = size;
    unsigned min_y = size;
    unsigned max_x = 0;
    unsigned max_y = 0;

    while (!history.empty()) {
        auto const where = history.back();
        history.pop_back();

        auto const x = where.first;
        auto const y = where.second;

        min_x = x < min_x ? x : min_x;
        min_y = y < min_y ? y : min_y;
        max_x = x > max_x ? x : max_x;
        max_y = y > max_y ? y : max_y;

        for_each_xy(
            x*cell_size, (x+1)*cell_size,
            [&](unsigned x, unsigned y) {
                result.set(x, y, room_part::floor);
            },
            y*cell_size, (y+1)*cell_size,
            [](unsigned, unsigned){}
        );

        bool const gen_north = generator() < 2;
        bool const gen_south = generator() < 2;
        bool const gen_east  = generator() < 2;
        bool const gen_west  = generator() < 2;

        bool const go_north = gen_north && (y > 0)      && result.at(x,   y-1) == room_part::empty;
        bool const go_west  = gen_west  && (x > 0)      && result.at(x-1, y  ) == room_part::empty;
        bool const go_south = gen_south && (y < size-1) && result.at(x,   y+1) == room_part::empty;
        bool const go_east  = gen_east  && (x < size-1) && result.at(x+1, y  ) == room_part::empty;

        if (go_north) history.emplace_back(x,   y-1);
        if (go_west)  history.emplace_back(x-1, y  );
        if (go_south) history.emplace_back(x,   y+1);
        if (go_east)  history.emplace_back(x+1, y  );
    }

    BK_ASSERT(max_x >= min_x);
    BK_ASSERT(max_y >= min_y);
    
    auto const k = cell_size;

    result.rect_ = urect(min_x*k, min_y*k, (max_x+1)*k, (max_y+1)*k);

    for_each_xy(
        result.rect_.left, result.rect_.right,
        [&](unsigned x, unsigned y) {
            result.set(x, y, result.transform(x, y));
        },
        result.rect_.top, result.rect_.bottom,
        [&](unsigned, unsigned) {}
    );

    return result;
}

void room_compound::write(write_f out) const {
    for_each_xy(
        rect_.left, rect_.right,
        [&](unsigned x, unsigned y) {
            out(x, y, at(x, y));
        },
        rect_.top, rect_.bottom,
        [&](unsigned, unsigned) {}
    );
}

//room_part room_compound::get_room_type_(
//    unsigned const  x,
//    unsigned const  y,
//    direction const dx,
//    direction const dy
//) const {
//    if (dx == dy) {
//        assert(dx == direction::none);
//        assert(dy == direction::none);
//
//        return room_part::floor;
//    }
//    
//    auto const filled = [this](unsigned const xi, unsigned yi) {
//        return !at_(xi, yi).empty;
//    };
//
//    auto const has_neighbor_n  = [&] {return (y > 0      ) && filled(x,   y-1);};
//    auto const has_neighbor_s  = [&] {return (y < size_-1) && filled(x,   y+1);};
//    auto const has_neighbor_w  = [&] {return (x > 0      ) && filled(x-1, y  );};
//    auto const has_neighbor_e  = [&] {return (x < size_-1) && filled(x+1, y  );};
//    auto const has_neighbor_ne = [&] {return (y > 0)       && (x < size_-1) && filled(x+1, y-1);};
//    auto const has_neighbor_nw = [&] {return (y > 0)       && (x > 0)       && filled(x-1, y-1);};
//    auto const has_neighbor_se = [&] {return (y < size_-1) && (x < size_-1) && filled(x+1, y+1);};
//    auto const has_neighbor_sw = [&] {return (y < size_-1) && (x > 0)       && filled(x-1, y+1);};
//
//    static auto const N  = BK_ENUM_VALUE(direction::north);
//    static auto const S  = BK_ENUM_VALUE(direction::south);
//    static auto const E  = BK_ENUM_VALUE(direction::east);
//    static auto const W  = BK_ENUM_VALUE(direction::west);
//    static auto const NE = BK_ENUM_VALUE(direction::north_east);
//    static auto const NW = BK_ENUM_VALUE(direction::north_west);
//    static auto const SE = BK_ENUM_VALUE(direction::south_east);
//    static auto const SW = BK_ENUM_VALUE(direction::south_west);
//
//    auto const border_value = get_enum_value(dy) | get_enum_value(dx);
//
//    switch (border_value) {
//    case N  : return has_neighbor_n() ? room_part::floor : room_part::h_edge;
//    case E  : return has_neighbor_e() ? room_part::floor : room_part::v_edge;
//    case W  : return has_neighbor_w() ? room_part::floor : room_part::v_edge;
//    case S  : return has_neighbor_s() ? room_part::floor : room_part::h_edge;
//    case N | E : {
//        auto const neighbor_value =
//            (has_neighbor_n()  ? N  : 0) |
//            (has_neighbor_e()  ? E  : 0) |
//            (has_neighbor_ne() ? NE : 0);
//
//        switch (neighbor_value) {
//        case 0          :
//        case NE         : return room_part::corner_sw;
//        case N          : 
//        case N | NE     : return room_part::v_edge;
//        case E          : 
//        case E | NE     : return room_part::h_edge;
//        case N | E      : return room_part::corner_ne;
//        case N | E | NE : return room_part::floor;
//        }
//    }
//    case N | W : {
//        auto const neighbor_value =
//            (has_neighbor_n()  ? N  : 0) |
//            (has_neighbor_w()  ? W  : 0) |
//            (has_neighbor_nw() ? NW : 0);
//
//        switch (neighbor_value) {
//        case 0          :
//        case NW         : return room_part::corner_se;
//        case N          : 
//        case N | NW     : return room_part::v_edge;
//        case W          : 
//        case W | NW     : return room_part::h_edge;
//        case N | W      : return room_part::corner_nw;
//        case N | W | NW : return room_part::floor;
//        }
//    }
//    case S | E : {
//        auto const neighbor_value =
//            (has_neighbor_s()  ? S  : 0) |
//            (has_neighbor_e()  ? E  : 0) |
//            (has_neighbor_se() ? SE : 0);
//
//        switch (neighbor_value) {
//        case 0          :
//        case SE         : return room_part::corner_nw;
//        case S          : 
//        case S | SE     : return room_part::v_edge;
//        case E          : 
//        case E | SE     : return room_part::h_edge;
//        case S | E      : return room_part::corner_se;
//        case S | E | SE : return room_part::floor;
//        }
//    }
//    case S | W : {
//        auto const neighbor_value =
//            (has_neighbor_s()  ? S  : 0) |
//            (has_neighbor_w()  ? W  : 0) |
//            (has_neighbor_sw() ? SW : 0);
//
//        switch (neighbor_value) {
//        case 0          :
//        case SW         : return room_part::corner_ne;
//        case S          : 
//        case S | SW     : return room_part::v_edge;
//        case W          : 
//        case W | SW     : return room_part::h_edge;
//        case S | W      : return room_part::corner_sw;
//        case S | W | SE : return room_part::floor;
//        }
//    }
//    }
//
//    return room_part::floor;
//}
//
//rect<unsigned> room_compound::get_rect_() const {
//    unsigned left   = size_;
//    unsigned top    = size_;
//    unsigned right  = 0;
//    unsigned bottom = 0;
//
//    for_each_xy(
//        0U, size_,
//        [&](unsigned const x, unsigned const y) {
//            if (at_(x, y).empty) return;
//
//            left   = x < left   ? x : left;
//            top    = y < top    ? y : top;
//            right  = x > right  ? x : right;
//            bottom = y > bottom ? y : bottom;
//        },
//        0U, size_,
//        [](unsigned, unsigned) {}
//    );
//
//    BK_ASSERT(right  >= left);
//    BK_ASSERT(bottom >= top);
//
//    rect<unsigned> result = {left, top, right, bottom};
//    return result;
//}
//
//void room_compound::write(write_f out) const {
//    auto rect = get_rect_();
//
//    auto const fill_f = [&](unsigned const x, unsigned const y) {
//        auto const x_off = x*cell_size_;
//        auto const y_off = y*cell_size_;
//
//        direction side_x = direction::none;
//        direction side_y = direction::none;
//
//        for_each_xy(
//            0U, cell_size_,
//            [&](unsigned const xi, unsigned const yi) {
//                side_x =
//                    (xi == 0)            ? direction::west :
//                    (xi == cell_size_-1) ? direction::east :
//                                           direction::none;
//
//                out(
//                    xi + x_off,
//                    yi + y_off,
//                    get_room_type_(
//                        rect.left + x,
//                        rect.top  + y,
//                        side_x,
//                        side_y
//                    )
//                );
//            },
//            0U, cell_size_,
//            [&](unsigned, unsigned const yi) {
//                side_y =
//                    (yi == 0)            ? direction::north :
//                    (yi == cell_size_-1) ? direction::south :
//                                           direction::none;
//            }
//        );
//    };
//
//    for_each_xy(
//        rect.left, rect.right + 1,
//        [&](unsigned const xi, unsigned const yi) {
//            if (!at_(xi, yi).empty) {
//                fill_f(xi - rect.left, yi - rect.top);
//            }
//        },
//        rect.top, rect.bottom + 1,
//        [](unsigned, unsigned) {}
//    );
//}
