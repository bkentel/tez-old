#include "pch.hpp"
#include "room_compound.hpp"

room_compound::room_compound(unsigned size, unsigned cell_size)
    : room_base(size*cell_size, size*cell_size)
    , cell_size_(cell_size)
    , rect_(0, 0, w_, h_)
{
    BK_ASSERT(size >= MIN_CELLS);
    BK_ASSERT(size <= MAX_CELLS);
    
    BK_ASSERT(cell_size >= MIN_CELL_DIM);
    BK_ASSERT(cell_size <= MAX_CELL_DIM);
}

room_compound room_compound::generate_(
    unsigned const size,
    unsigned const cell_size,
    unsigned const ox,
    unsigned const oy,
    std::function<unsigned ()> const generator
) {
    room_compound result(size, cell_size);

    std::vector<std::pair<unsigned, unsigned>> history;
    history.emplace_back(ox, oy);

    auto const count = size*cell_size;

    unsigned min_x = count;
    unsigned min_y = count;
    unsigned max_x = 0;
    unsigned max_y = 0;

    unsigned room_count = 1;

    while (!history.empty()) {
        auto const where = history.back();
        history.pop_back();

        auto const x = where.first;
        auto const y = where.second;

        auto const left   = x;
        auto const right  = x + cell_size;
        auto const top    = y;
        auto const bottom = y + cell_size;

        min_x = x < min_x ? x : min_x;
        min_y = y < min_y ? y : min_y;
        max_x = x + cell_size > max_x ? x + cell_size : max_x;
        max_y = y + cell_size > max_y ? y + cell_size : max_y;

        for_each_xy(
            left, right,
            [&](unsigned x, unsigned y) {
                result.set(x, y, room_part::floor);
            },
            top, bottom,
            [](unsigned, unsigned){}
        );

        bool const gen_north = generator() < 100 - 2*room_count ? (++room_count, true) : false;
        bool const gen_south = generator() < 100 - 2*room_count ? (++room_count, true) : false;
        bool const gen_east  = generator() < 100 - 2*room_count ? (++room_count, true) : false;
        bool const gen_west  = generator() < 100 - 2*room_count ? (++room_count, true) : false;

        bool const go_north = gen_north && (top    >= cell_size)     && result.at(left,     top - 1) == room_part::empty;
        bool const go_west  = gen_west  && (left   >= cell_size)     && result.at(left - 1, top)     == room_part::empty;
        bool const go_south = gen_south && (bottom <  count - cell_size) && result.at(left,     bottom)  == room_part::empty;
        bool const go_east  = gen_east  && (right  <  count - cell_size) && result.at(right,    top)     == room_part::empty;

        if (go_north) history.emplace_back(left,             top - cell_size);
        if (go_west)  history.emplace_back(left - cell_size, top  );
        if (go_south) history.emplace_back(left,             bottom);
        if (go_east)  history.emplace_back(right,            top  );
    }

    BK_ASSERT(max_x >= min_x);
    BK_ASSERT(max_y >= min_y);
    
    result.rect_ = urect(min_x, min_y, max_x, max_y);

    for_each_xy(
        result.left(), result.right(),
        [&](unsigned x, unsigned y) {
            result.set(x, y, result.transform(x, y));
        },
        result.top(), result.bottom(),
        [&](unsigned, unsigned) {}
    );

    return result;
}
