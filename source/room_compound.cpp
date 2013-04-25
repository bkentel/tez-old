#include "pch.hpp"
//#include "room_compound.hpp"
//
//room_compound::room_compound(unsigned size, unsigned cell_size)
//    : room_base(size*cell_size, size*cell_size)
//    , cell_size_(cell_size)
//    , rect_(0, 0, w_, h_)
//{
//    BK_ASSERT(size >= MIN_CELLS);
//    BK_ASSERT(size <= MAX_CELLS);
//    
//    BK_ASSERT(cell_size >= MIN_CELL_DIM);
//    BK_ASSERT(cell_size <= MAX_CELL_DIM);
//}
//
//room_compound room_compound::generate_(
//    unsigned const size,
//    unsigned const cell_size,
//    unsigned const ox,
//    unsigned const oy,
//    std::function<unsigned ()> const generator
//) {
//    room_compound result(size, cell_size);
//
//    std::vector<std::pair<unsigned, unsigned>> history;
//    history.emplace_back(ox, oy);
//
//    auto const count = size*cell_size;
//
//    unsigned min_x = count;
//    unsigned min_y = count;
//    unsigned max_x = 0;
//    unsigned max_y = 0;
//
//    unsigned room_count = 1;
//
//    while (!history.empty()) {
//        auto const where = history.back();
//        history.pop_back();
//
//        auto const left   = where.first;
//        auto const right  = left + cell_size;
//        auto const top    = where.second;
//        auto const bottom = top + cell_size;
//
//        min_x = left   < min_x ? left   : min_x;
//        min_y = top    < min_y ? top    : min_y;
//        max_x = right  > max_x ? right  : max_x;
//        max_y = bottom > max_y ? bottom : max_y;
//
//        for_each_xy(
//            left, right,
//            [&](unsigned const x, unsigned const y) {
//                result.set(x, y, room_part::floor);
//            },
//            top, bottom,
//            [](unsigned, unsigned){}
//        );
//
//        bool const gen_n = generator() < 100 - 2*room_count;
//        bool const gen_s = generator() < 100 - 2*room_count;
//        bool const gen_e = generator() < 100 - 2*room_count;
//        bool const gen_w = generator() < 100 - 2*room_count;
//
//        bool const in_n = (top    >= cell_size);
//        bool const in_w = (left   >= cell_size);
//        bool const in_s = (bottom <  count - cell_size);
//        bool const in_e = (right  <  count - cell_size);
//
//        bool const go_n = gen_n && in_n && result.at(left,     top - 1) == room_part::empty;
//        bool const go_w = gen_w && in_w && result.at(left - 1, top    ) == room_part::empty;
//        bool const go_s = gen_s && in_s && result.at(left,     bottom ) == room_part::empty;
//        bool const go_e = gen_e && in_e && result.at(right,    top    ) == room_part::empty;
//
//        if (go_n) history.emplace_back(left,             top - cell_size), ++room_count;
//        if (go_w) history.emplace_back(left - cell_size, top            ), ++room_count;
//        if (go_s) history.emplace_back(left,             bottom         ), ++room_count;
//        if (go_e) history.emplace_back(right,            top            ), ++room_count;
//    }
//
//    BK_ASSERT(max_x >= min_x);
//    BK_ASSERT(max_y >= min_y);
//    
//    result.rect_ = urect(min_x, min_y, max_x, max_y);
//
//    for_each_xy(
//        result.left(), result.right(),
//        [&](unsigned x, unsigned y) {
//            result.set(x, y, result.transform(x, y));
//        },
//        result.top(), result.bottom(),
//        [&](unsigned, unsigned) {}
//    );
//
//    return result;
//}
