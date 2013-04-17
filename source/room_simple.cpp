#include "pch.hpp"
#include "room_simple.hpp"

room_simple::room_simple(unsigned x, unsigned y, unsigned w, unsigned h)
    : x_(x)
    , y_(y)
    , w_(w)
    , h_(h)
    , data_(new room_part[w*h])
{
    std::fill_n(data_.get(), w*h, room_part::floor);
}

room_simple::room_simple(room_simple const& other)
    : x_(other.x_)
    , y_(other.y_)
    , w_(other.w_)
    , h_(other.h_)
    , data_(new room_part[w_*h_])
{
    std::copy_n(other.data_.get(), w_*h_, data_.get());
}

room_simple::room_simple(room_simple&& other)
    : x_(other.x_)
    , y_(other.y_)
    , w_(other.w_)
    , h_(other.h_)
    , data_(std::move(other.data_))
{
}

room_part room_simple::at(unsigned const x, unsigned const y) const {
    BK_ASSERT(x < width());
    BK_ASSERT(y < height());

    return *(data_.get() + x + y*width());
}

void room_simple::set(unsigned const x, unsigned const y, room_part const part) {
    BK_ASSERT(x < width());
    BK_ASSERT(y < height());

    *(data_.get() + x + y*width()) = part;
}

room_simple room_simple::generate_(
    unsigned const x,
    unsigned const y, 
    std::function<unsigned ()> const generator
) {
    auto const w = generator();
    auto const h = generator();

    room_simple result(x, y, w, h);

    for_each_xy(
        0U, w,
        [&](unsigned x, unsigned y) {
            result.set(x, y, result.transform(x, y));
        },
        0U, h,
        [&](unsigned, unsigned) {}
    );

    return result;
}

void room_simple::write(write_f out) const {   
    for_each_xy(
        left(), right(),
        [&](unsigned x, unsigned y) {
            out(x, y, at(x, y));
        },
        top(), bottom(),
        [&](unsigned, unsigned) {}
    );
}
