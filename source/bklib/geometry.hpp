#pragma once

#include <type_traits>
#include "util.hpp"

namespace bklib {
//==============================================================================
//! Distance (difference) between two values; always positive.
//==============================================================================
template <
    typename T,
    typename R = typename make_distance_type<T>::type
>
inline R distance(T const a, T const b,
    typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr
) {
    return static_cast<R>(a >= b ? a - b : b - a);
}
//==============================================================================
//! Magnitude; always positive.
//==============================================================================
template <
    typename T,
    typename R = typename make_distance_type<T>::type
>
inline R magnitude(T const n) {
    return static_cast<R>(std::abs(n));
}

//==============================================================================
//! A 2 dimensional point type.
//==============================================================================
template <typename T>
struct point2d {
    point2d(T x, T y)
        : x(x)
        , y(y)
    {
    }

    template <typename U, typename V>
    point2d& translate_by(U const dx, V const dy) {
        x += dx;
        y += dy;

        return *this;
    }

    template <typename U, typename V>
    point2d& translate_to(U const x, V const y) {
        this->x = x;
        this->y = y;

        return *this;
    }

    template <typename U>
    bool operator==(point2d<U> const rhs) const {
        return (x == rhs.x) && (y == rhs.y);
    }

    template <typename U>
    bool operator!=(point2d<U> const rhs) const {
        return !(*this == rhs);
    }

    template <typename U>
    explicit operator point2d<U>() const {
        return point2d<U>(
            static_cast<U>(x), static_cast<U>(y)
        );
    }

    T x, y;
}; // struct point2d

template <
    typename T,
    typename U,
    typename R = typename std::common_type<T, U>::type
>
inline point2d<R> make_point(T const x, U const y) {
    return point2d<R>(x, y);
}

template <
    typename T,
    typename U,
    typename R = typename make_distance_type<
        typename std::common_type<T, U>::type
    >::type
>
inline R distance2(point2d<T> const a, point2d<U> const b)
{
    auto const dx = distance(a.x, b.x);
    auto const dy = distance(a.y, b.y);

    return static_cast<R>(dx*dx + dy*dy);
}

template <typename T, typename U>
inline double distance(point2d<T> const a, point2d<U> const b) {
    return std::sqrt(distance2(a, b));
}

template <typename T, typename U, typename V>
point2d<T> translate_by(point2d<T> p, U dx, V dy) {
    return p.translate_by(dx, dy);
}

template <typename T, typename U>
point2d<T> translate_by(point2d<T> p, point2d<U> d) {
    return translate_by(p, d.x, d.y);
}

//==============================================================================
//! A closed interval.
//==============================================================================
template <typename T>
struct range {
    range(T first, T last)
        : first(first), last(last)
    {
    }

    explicit operator bool() const {
        return first <= last;
    }

    template <
        typename U,
        typename R = typename std::common_type<T, U>::type
    >
    range<R> intersection_with(range<U> const& b) const {
        auto const& a = *this;

        return range<R>(
            a.first < b.first ? b.first : a.first,
            a.last  > b.last  ? b.last  : a.last
        );
    }

    template <typename U>
    bool intersects(range<U> const& b) const {
        return static_cast<bool>(intersection_with(b));
    }

    template <typename U>
    bool intersects(U const n) const {
        return n >= first && n <= last;
    }

    typename make_distance_type<T>::type magnitude() const {
        return distance(last, first);
    }

    template <typename U>
    range& translate_by(U const delta) {
        first += delta;
        last  += delta;

        return *this;
    }

    template <typename U>
    range& translate_to(U const where) {
        auto const dist = magnitude();

        first = where;
        last  = first + dist;

        return *this;
    }

    template <typename U>
    bool operator==(range<U> const& rhs) const {
        return first == rhs.first && last == rhs.last;
    }

    template <typename U>
    bool operator!=(range<U> const& rhs) const {
        return !(*this == rhs);
    }

    template <typename U>
    bool operator<(range<U> const& rhs) const {
        return first < rhs.first;
    }

    T first, last;
}; //struct range

//==============================================================================
//! Rectangle.
//==============================================================================
template <typename T>
struct rect {
    typedef typename make_distance_type<T>::type size_type;

    template <typename U, typename V = size_type>
    rect(point2d<U> const p, V const w, V const h)
        : left(p.x)
        , top(p.y)
        , right(p.x + w)
        , bottom(p.y + h)
    {
        BK_ASSERT(w > static_cast<T>(0));
        BK_ASSERT(h > static_cast<T>(0));
    }
    
    rect(T left, T top, T right, T bottom)
        : left(left), top(top), right(right), bottom(bottom)
    {
    }

    explicit operator bool() const {
        return (left < right) && (top < bottom);
    }

    template <typename U>
    bool operator==(rect<U> const& rhs) const {
        auto const& a = *this;
        auto const& b = rhs;

        return (a.left   == b.left)   &&
               (a.top    == b.top)    &&
               (a.right  == b.right)  &&
               (a.bottom == b.bottom);
    }

    template <typename U>
    inline bool operator!=(rect<U> const& rhs) const {
        return !(*this == rhs);
    }

    rect& translate_to(T const x, T const y) {
        auto const w = width();
        auto const h = height();

        left   = x;
        right  = x + w;
        top    = y;
        bottom = y + h;

        return *this;
    }

    template <typename U, typename V>
    rect& translate_by(U const dx, V const dy) {
        left   += dx;
        right  += dx;
        top    += dy;
        bottom += dy;

        return *this;
    }

    template <typename U>
    explicit operator rect<U>() const {
        return rect<U>(left, top, right, bottom);
    }

    size_type width()  const { return distance(right, left); }
    size_type height() const { return distance(bottom, top); }

    template <typename U>
    std::pair<bool, point2d<U>> intersection_with(point2d<U> const p) const {
        return std::make_pair(intersects(p), p);
    }

    template <typename U, typename R = typename std::common_type<T, U>::type>
    rect<R> intersection_with(rect<U> const r) const {
        auto const& a = *this;
        auto const& b = r;

        range<T> const ax(a.left, a.right);
        range<T> const ay(a.top,  a.bottom);
        range<U> const bx(b.left, b.right);
        range<U> const by(b.top,  b.bottom);

        auto const ix = ax.intersection_with(bx);
        auto const iy = ay.intersection_with(by);

        return rect<R>(
            ix.first, iy.first, ix.last, iy.last
        );
    }

    template <typename U>
    bool intersects(point2d<U> const& p) const {
        return range<T>(left, right).intersects(p.x) &&
               range<T>(top, bottom).intersects(p.y);
    }

    template <typename U>
    bool intersects(rect<U> const& r) const {
        return !(
            (r.right  < left)   ||
            (r.left   > right)  ||
            (r.top    > bottom) ||
            (r.bottom < top)    
        );
    }

    friend std::ostream& operator<<(std::ostream& out, rect<T> const& r) {
        return out
            << "rect"                    << "\n"
            << "--left   = " << r.left   << "\n"
            << "--top    = " << r.top    << "\n"
            << "--right  = " << r.right  << "\n"
            << "--bottom = " << r.bottom << "\n";
    }

    T left, top, right, bottom;
}; //rect

template <typename T, typename U>
inline bool intersects(rect<T> const a, rect<U> const b) {
    return a.intersects(b);
}

template <typename T, typename U>
inline bool intersects(rect<T> const a, point2d<U> const b) {
    return a.intersects(b);
}

template <typename T, typename U>
inline bool intersects(point2d<T> const a, rect<U> const b) {
    return b.intersects(a);
}

template <typename T, typename U, typename V>
rect<T> translate_by(rect<T> r, U dx, V dy) {
    return r.translate_by(dx, dy);
}

template <typename T>
rect<T> translate_to(rect<T> r, T x, T y) {
    return r.translate_to(x, y);
}

} //namespace bklib
