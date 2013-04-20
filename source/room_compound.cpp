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
