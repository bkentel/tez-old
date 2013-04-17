#pragma once

#include "room_base.hpp"

struct room_compound : public room_base {
    static unsigned const MIN_CELL_DIM = 2;
    static unsigned const MAX_CELL_DIM = 5;

    static unsigned const MIN_CELLS = 10;
    static unsigned const MAX_CELLS = 15;
   
    unsigned left()   const override { return x_; }
    unsigned top()    const override { return y_; }
    unsigned right()  const override { return x_ + width(); }
    unsigned bottom() const override { return y_ + height(); }
    
    unsigned width() const override {
        return get_rect_().width();
    }
    
    unsigned height() const override {
        return get_rect_().height();
    }

    room_compound(unsigned x, unsigned y, unsigned size, unsigned cell_size);
    room_compound(room_compound const& other);

    static room_compound generate_(
        unsigned x, unsigned y,
        unsigned size, unsigned cell_size,
        unsigned ox, unsigned oy,
        std::function<unsigned ()> generator
    );

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

    void write(write_f out) const override;
private:
    struct cell_t {
        cell_t() : empty(true) {}
        bool empty;
    };

    room_part get_room_type_(
        unsigned x, unsigned y,
        direction dx, direction dy
    ) const;

    rect<unsigned> get_rect_() const;

    cell_t& at_(unsigned x, unsigned y) {
        BK_ASSERT(x < size_);
        BK_ASSERT(y < size_);

        return *(cells_.get() + x + y*size_);
    }

    cell_t const& at_(unsigned x, unsigned y) const {
        return const_cast<room_compound*>(this)->at_(x, y);     
    }

    unsigned x_, y_;
    unsigned size_; // number of cells; W x H
    unsigned cell_size_;  // size of each cell
    std::unique_ptr<cell_t[]> cells_;
};