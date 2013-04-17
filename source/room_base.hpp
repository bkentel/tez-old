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

        bool const N = (y != 0  ) && (at(x,   y-1) != room_part::empty);
        bool const S = (y <  h-1) && (at(x,   y+1) != room_part::empty);
        bool const E = (x <  w-1) && (at(x+1, y  ) != room_part::empty);
        bool const W = (x != 0  ) && (at(x-1, y  ) != room_part::empty);

        auto const count =
            (N ? 1 : 0) +
            (S ? 1 : 0) +
            (E ? 1 : 0) +
            (W ? 1 : 0);

        switch (count) {
        case 2 :
            if      (N && E) return room_part::corner_sw;
            else if (N && W) return room_part::corner_se;
            else if (S && E) return room_part::corner_nw;
            else if (S && W) return room_part::corner_ne;
            else             BK_ASSERT(false);
            break;
        case 3 :
            return (N ^ S) ? room_part::h_edge
                           : room_part::v_edge;
        case 0 :
        case 1 :
        case 4 :
            return room_part::floor;
        }

        return room_part::empty;
    }

};
