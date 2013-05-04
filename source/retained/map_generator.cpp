#include "pch.hpp"
#include "map_generator.hpp"

#include "targa.hpp"

binary_split_node::binary_split_node(unsigned x, unsigned y, unsigned w, unsigned h)
    : x(x), y(y), w(w), h(h)
{
}

void binary_split_node::split(unsigned depth) {
    if (w / 2 < MIN_DIM && h / 2 < MIN_DIM) {
        return; // can't split
    }

    if (rand() % 1000 >= 1000 - depth*100) {
        //return;
    }

    auto const r = rand();
    bool const is_x_split = w >= h;

    auto const split_dim  = is_x_split ? w : h;
    auto const split_var  = (split_dim - MIN_DIM) / 2;
    auto const split_size = (split_dim / 2) + (r % split_var);

    bool const split_first  = (rand() % 100) < 100 - (depth - 1)*5;
    bool const split_second = (rand() % 100) < 100 - (depth - 1)*5;

    if (is_x_split) {
        auto const x0 = x;
        auto const x1 = x + split_size;

        auto const w0 = split_size;
        auto const w1 = w - split_size;

        if (split_first)  first.reset( new binary_split_node(x0, y, w0, h));
        if (split_second) second.reset(new binary_split_node(x1, y, w1, h));
    } else {
        auto const y0 = y;
        auto const y1 = y + split_size;

        auto const h0 = split_size;
        auto const h1 = h - split_size;

        if (split_first)  first.reset( new binary_split_node(x, y0, w, h0));
        if (split_second) second.reset(new binary_split_node(x, y1, w, h1));
    }

    if (first)  first->split(depth + 1);
    if (second) second->split(depth + 1);
}

void binary_split_node::write(image_targa& img, unsigned& count) {
    if (first) {
        first->write(img, count);
    }

    if (second) {
        second->write(img, count);
    }

    if (first || second) {
        return;
    }

    uint8_t const r = rand() % 255;
    uint8_t const g = rand() % 255;
    uint8_t const b = rand() % 255;

    for (unsigned xx = x; xx < x + w; ++xx) {
        for (unsigned yy = y; yy < y + h; ++yy) {
            if (xx == x || xx == x + w - 1 || yy == y || yy == y + h - 1) {
                img.set(xx, yy, 255, 255, 255, 0);
            } else {
                img.set(xx, yy, r, g, b, 0);
            }
        }
    }
}

binary_split_generator::binary_split_generator(unsigned w, unsigned h)
    : binary_split_node(0, 0, w, h)
{
    BK_ASSERT(w >= MIN_DIM);
    BK_ASSERT(h >= MIN_DIM);
}
