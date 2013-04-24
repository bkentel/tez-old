#pragma once

#include "room_base.hpp"

enum class tile_category : uint8_t {
    empty   = ' ',
    wall    = '|',
    ceiling = '#',
    floor   = '.',
};

#define BK_DELETE


//class simple_room {
//public:
//    typedef std::uniform_int_distribution<unsigned> dist_t;
//    typedef std::function<unsigned ()>              random_t;
//
//    template <typename T>
//    simple_room(T& gen,
//        unsigned min_w, unsigned max_w,
//        unsigned min_h, unsigned max_h
//    )
//        : distribution_(0, 100)
//        , random_(
//            [&] {
//                return distribution_(gen);
//            }
//        )
//        , data_(
//            dist_t(min_w, max_w)(gen),
//            dist_t(min_h, max_h)(gen),
//            tile_category::floor
//        )
//    {
//    }
//
//    unsigned width()  const { return data_.width(); }
//    unsigned height() const { return data_.height(); }
//
//    void copy(grid2d<tile_category>& dest, unsigned x = 0, unsigned y = 0) const {
//        for (unsigned yi = 0; yi < height(); ++yi) {
//            for (unsigned xi = 0; xi < width(); ++xi) {
//                dest.set(xi + x, yi + y, data_.at(xi, yi));
//            }
//        }
//    }
//private:
//    void transform_() {
//        for_each_block(data_, 0, width(), 0, height(), tile_category::empty,
//        [&](unsigned x, unsigned y, block<tile_category> b) {
//            if (
//                b.n == tile_category::empty ||
//                b.w == tile_category::empty ||
//                b.e == tile_category::empty ||
//                b.s == tile_category::empty
//            ) {
//                data_.set(x, y, tile_category::ceiling);
//            } else if (b.e == tile_category::ceiling) {
//                data_.set(x, y, tile_category::wall);
//            }
//        });
//    }
//
//    dist_t distribution_;
//    std::function<unsigned ()> random_;
//    grid2d<tile_category> data_;
//};

class room_simple : public room_base {
public:
    static unsigned const MIN_DIM = 5;
    static unsigned const MAX_DIM = 10;

    room_simple(unsigned w, unsigned h);
    virtual ~room_simple() {}
    
    template <typename T>
    static room_simple generate(T&& generator) {
        std::uniform_int_distribution<unsigned> dist(MIN_DIM, MAX_DIM);
        
        return generate_([&] {
            return dist(generator);
        });
    }
private:
    static room_simple generate_(std::function<unsigned ()> generator);
};
