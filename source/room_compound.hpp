#pragma once

#include "room_base.hpp"
#include "assert.hpp"
#include "geometry.hpp"

class room_compound : public room_base {
public:
    typedef rect<unsigned> urect;

    static unsigned const MIN_CELL_DIM = 3;
    static unsigned const MAX_CELL_DIM = 5;

    static unsigned const MIN_CELLS = 10;
    static unsigned const MAX_CELLS = 15;

    room_compound(unsigned size, unsigned cell_size);
    virtual ~room_compound() {}

    virtual unsigned width()  const override { return right() - left(); }
    virtual unsigned height() const override { return bottom() - top(); }

    virtual unsigned left()   const override { return rect_.left; }
    virtual unsigned right()  const override { return rect_.right; }
    virtual unsigned top()    const override { return rect_.top; }
    virtual unsigned bottom() const override { return rect_.bottom; }

    template <typename T>
    static room_compound generate(T&& generator) {
        typedef std::uniform_int_distribution<unsigned> dist_t;

        auto const size      = dist_t(MIN_CELLS,    MAX_CELLS)(generator);
        auto const cell_size = dist_t(MIN_CELL_DIM, MAX_CELL_DIM)(generator);
        auto const start_x   = dist_t(0, (size-1)*cell_size)(generator);
        auto const start_y   = dist_t(0, (size-1)*cell_size)(generator);

        dist_t dist(0, 100);
        
        return generate_(size, cell_size, start_x, start_y, [&] {
            return dist(generator);
        });
    }
private:
    static room_compound generate_(
        unsigned size, unsigned cell_size,
        unsigned ox, unsigned oy,
        std::function<unsigned ()> generator
    );

    unsigned cell_size_;  // size of each cell
    urect    rect_;
};