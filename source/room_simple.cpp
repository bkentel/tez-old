#include "pch.hpp"
//#include "room_simple.hpp"
//
//room_simple::room_simple(unsigned w, unsigned h)
//    : room_base(w, h, room_part::floor)
//{
//}
//
//room_simple room_simple::generate_(
//    std::function<unsigned ()> const generator
//) {
//    auto const w = generator();
//    auto const h = generator();
//
//    room_simple result(w, h);
//
//    for_each_xy(
//        0U, w,
//        [&](unsigned x, unsigned y) {
//            result.set(x, y, result.transform(x, y));
//        },
//        0U, h,
//        [&](unsigned, unsigned) {}
//    );
//
//    return result;
//}
