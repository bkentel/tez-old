#pragma once

#include "types.hpp"
#include <functional>

enum class room_part : uint8_t {
    empty,
    floor,
    v_edge,
    h_edge,
    corner_ne,
    corner_nw,
    corner_se,
    corner_sw,
};

template <typename T, T t>
struct enum_value {
    typedef typename std::underlying_type<T>::type type;
    static type const value = static_cast<type>(t);
};

#define BK_ENUM_VALUE(x) enum_value<decltype(x), x>::value

template <typename T>
typename std::underlying_type<T>::type
inline get_enum_value(T value) {
    typedef typename std::underlying_type<T>::type U;
    return static_cast<U>(value);
}

template <typename T, typename X, typename Y>
void for_each_xy(
    T xmin, T xmax, X&& xf,
    T ymin, T ymax, Y&& yf
) {
    for (T yi = ymin; yi < ymax; ++yi) {
        yf(0, yi);
        for (T xi = xmin; xi < xmax; ++xi) {
            xf(xi, yi);
        }
    }
}

struct room_base {
    typedef std::function<void (unsigned x, unsigned y, room_part part)> write_f;

    virtual ~room_base() {}

    virtual unsigned left()   const = 0;
    virtual unsigned top()    const = 0;
    virtual unsigned right()  const = 0;
    virtual unsigned bottom() const = 0;
    
    virtual unsigned width()  const = 0;
    virtual unsigned height() const = 0;

    virtual room_part at(unsigned x, unsigned y) const = 0;
    virtual void set(unsigned x, unsigned y, room_part part) = 0;

    virtual void write(write_f out) const = 0;

    room_part transform(unsigned x, unsigned y) const {
        auto const w = width();
        auto const h = height();

        if (
            x > w || y > h || at(x, y) == room_part::empty
        ) {
            return room_part::empty;
        }

        bool const in_n = (y != 0  );
        bool const in_s = (y <  h-1);
        bool const in_e = (x <  w-1);
        bool const in_w = (x != 0  );

        bool const has_n  = in_n &&         (at(x,   y-1) != room_part::empty);
        bool const has_s  = in_s &&         (at(x,   y+1) != room_part::empty);
        bool const has_e  = in_e &&         (at(x+1, y  ) != room_part::empty);
        bool const has_w  = in_w &&         (at(x-1, y  ) != room_part::empty);
        bool const has_ne = in_n && in_e && (at(x+1, y-1) != room_part::empty);
        bool const has_se = in_s && in_e && (at(x+1, y+1) != room_part::empty);
        bool const has_nw = in_n && in_w && (at(x-1, y-1) != room_part::empty);
        bool const has_sw = in_s && in_w && (at(x-1, y+1) != room_part::empty);

        static uint8_t const N  = 1 << 0;
        static uint8_t const S  = 1 << 1;
        static uint8_t const E  = 1 << 2;
        static uint8_t const W  = 1 << 3;
        static uint8_t const NE = 1 << 4;
        static uint8_t const NW = 1 << 5;
        static uint8_t const SE = 1 << 6;
        static uint8_t const SW = 1 << 7;

        uint8_t const value =
            (has_n  ? N  : 0) |
            (has_s  ? S  : 0) |
            (has_e  ? E  : 0) |
            (has_w  ? W  : 0) |
            (has_ne ? NE : 0) |
            (has_nw ? NW : 0) |
            (has_se ? SE : 0) |
            (has_sw ? SW : 0);

        switch (value) {
        case 0xFF & ~NE :
        case 0xFF &  (N | E | NE) : return room_part::corner_ne;
        case 0xFF & ~NW :
        case 0xFF &  (N | W | NW) : return room_part::corner_nw;
        case 0xFF & ~SE :
        case 0xFF &  (S | E | SE) : return room_part::corner_se;
        case 0xFF & ~SW :
        case 0xFF &  (S | W | SW) : return room_part::corner_sw;
        case 0xFF & ~(E | NE) :
        case 0xFF & ~(E | SE) :
        case 0xFF & ~(W | NW) :
        case 0xFF & ~(W | SW) :
        case 0xFF & ~(E | NE | SE) :
        case 0xFF & ~(W | NW | SW) : return room_part::v_edge;
        case 0xFF & ~(N | NE) :
        case 0xFF & ~(N | NW) :
        case 0xFF & ~(S | SE) :
        case 0xFF & ~(S | SW) :
        case 0xFF & ~(N | NE | NW) :
        case 0xFF & ~(S | SE | SW) : return room_part::h_edge;
        }

        return room_part::floor;
    }

};
