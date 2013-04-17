#pragma once

#include "room_base.hpp"

class room_simple : public room_base {
public:
    static unsigned const MIN_DIM = 3;
    static unsigned const MAX_DIM = 5;

    room_simple(unsigned x, unsigned y, unsigned w, unsigned h);
    room_simple(room_simple const& other);
    room_simple(room_simple&& other);

    unsigned left()   const override { return x_; }
    unsigned top()    const override { return y_; }
    unsigned right()  const override { return x_ + width(); }
    unsigned bottom() const override { return y_ + height(); }
    
    unsigned width()  const override { return w_; }
    unsigned height() const override { return h_; }

    room_part at(unsigned x, unsigned y) const override;
    void set(unsigned x, unsigned y, room_part part) override;

    template <typename T>
    static room_simple generate(unsigned x, unsigned y, T&& generator) {
        std::uniform_int_distribution<unsigned> dist(MIN_DIM, MAX_DIM);
        
        return generate_(x, y, [&] {
            return dist(generator);
        });
    }

    void write(write_f out) const override;

private:
    static room_simple generate_(unsigned x, unsigned y, std::function<unsigned ()> generator);

    unsigned x_, y_, w_, h_;
    std::unique_ptr<room_part[]> data_;
};
