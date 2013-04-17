#pragma once

#include "room_base.hpp"
#include "assert.hpp"

class room_compound : public room_base {
public:
    static unsigned const MIN_CELL_DIM = 2;
    static unsigned const MAX_CELL_DIM = 5;

    static unsigned const MIN_CELLS = 10;
    static unsigned const MAX_CELLS = 15;
   
    unsigned left()   const override { return x_; }
    unsigned top()    const override { return y_; }
    unsigned right()  const override { return x_ + width(); }
    unsigned bottom() const override { return y_ + height(); }
    unsigned width()  const override { return w_; }
    unsigned height() const override { return h_; }

    room_compound(unsigned x, unsigned y, unsigned size, unsigned cell_size);
    room_compound(room_compound const& other);
    room_compound(room_compound&& other);

    template <typename T>
    static room_compound generate(unsigned x, unsigned y, T&& generator) {
        typedef std::uniform_int_distribution<unsigned> dist_t;

        auto const size      = dist_t(MIN_CELLS,    MAX_CELLS)(generator);
        auto const cell_size = dist_t(MIN_CELL_DIM, MAX_CELL_DIM)(generator);
        auto const start_x   = dist_t(0, size - 1)(generator);
        auto const start_y   = dist_t(0, size - 1)(generator);

        dist_t dist(0, 4);
        
        return generate_(x, y, size, cell_size, start_x, start_y, [&] {
            return dist(generator);
        });
    }

    room_part at(unsigned x, unsigned y) const override {
        return at_(x, y);    
    }

    void set(unsigned x, unsigned y, room_part part) override {
        at_(x, y) = part;
    }

    void write(write_f out) const override {
    }
private:
    static room_compound generate_(
        unsigned x, unsigned y,
        unsigned size, unsigned cell_size,
        unsigned ox, unsigned oy,
        std::function<unsigned ()> generator
    );

    room_part& at_(unsigned x, unsigned y) {
        BK_ASSERT(x < size_*cell_size_);
        BK_ASSERT(y < size_*cell_size_);

        return *(cells_.get() + x + y*size_*cell_size_);
    }

    room_part const& at_(unsigned x, unsigned y) const {
        return const_cast<room_compound*>(this)->at_(x, y);
    }

    room_part get_room_type_(
        unsigned x, unsigned y,
        direction dx, direction dy
    ) const;

    rect<unsigned> get_rect_() const;

    unsigned x_, y_, w_, h_;
    unsigned size_; // number of cells; W x H
    unsigned cell_size_;  // size of each cell
    std::unique_ptr<room_part[]> cells_;
};