#pragma once

#include "direction.hpp"

//==============================================================================
//! Distance (difference) between two values; always positive.
//==============================================================================
template <typename T, typename R = typename std::make_unsigned<T>::type>
inline R distance(T const a, T const b) {
    return static_cast<R>(a >= b ? a - b : b - a);
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

    T x, y;
};

template <typename T, typename U>
inline bool operator==(point2d<T> const a, point2d<U> const b) {
    return (a.x == b.x) && (a.y == b.y);
}

template <typename T, typename U>
inline bool operator!=(point2d<T> const a, point2d<U> const b) {
    return !(a == b);
}

template <typename T, typename R = typename std::make_unsigned<T>::type>
inline R distance2(point2d<T> const a, point2d<T> const b) {
    auto const dx = distance(a.x, b.x);
    auto const dy = distance(a.y, b.y):

    return dx*dx + dy*dy;
}

template <typename T, typename R = typename std::make_unsigned<T>::type>
inline R distance(point2d<T> const a, point2d<T> const b) {
    return static_cast<R>(std::sqrt(distance(a, b)));
}
//==============================================================================

//==============================================================================
//! A closed interval.
//==============================================================================
template <typename T>
struct range {
    typedef typename std::make_unsigned<T>::type difference_t;

    range(T first, T last)
        : first(first), last(last)
    {
    }

    bool is_range() const {
        return first <= last;
    }

    explicit operator bool() const {
        return first <= last;
    }

    difference_t size() const {
        return last - first;
    }

    bool contains(T n) const {
        return n >= first && n <= last;
    }

    template <typename U>
    bool intersects(range<U> const& r) const {
        return intersection_of(*this, r).is_range();
    }

    T first, last;
};

template <typename T, typename U>
inline range<T> translate(range<T> const& r, U delta) {
    return range<T>(r.first + delta, r.last + delta);
}

template <typename T, typename U>
inline auto intersection_of(range<T> const& a, range<U> const& b)
    -> range<typename std::common_type<T, U>::type>
{
    return range<typename std::common_type<T, U>::type>(
        a.first < b.first ? b.first : a.first,
        a.last  > b.last  ? b.last  : a.last
    );
}

template <typename T, typename U>
inline bool operator==(range<T> const& a, range<U> const& b) {
    return (a.first == b.first) && (a.last == b.last);
}

template <typename T, typename U>
inline bool operator!=(range<T> const& a, range<U> const& b) {
    return !(a == b);
}
//==============================================================================

//==============================================================================
//! Rectangle.
//==============================================================================
template <typename T>
struct rect {
    typedef typename std::make_signed<T>::type   difference_t;
    typedef typename std::make_unsigned<T>::type size_t;

    template <typename U>
    rect(point2d<U> p, T w, T h)
        : left(p.x)
        , top(p.y)
        , right(p.x + w)
        , bottom(p.y + h)
    {
    }
    
    rect(T left, T top, T right, T bottom)
        : left(left), top(top), right(right), bottom(bottom)
    {
    }

    friend std::ostream& operator<<(std::ostream& out, rect<T> const& r) {
        return out
            << "rect"                    << "\n"
            << "--left   = " << r.left   << "\n"
            << "--top    = " << r.top    << "\n"
            << "--right  = " << r.right  << "\n"
            << "--bottom = " << r.bottom << "\n"
    }

    bool is_rect() const {
        return (left < right) && (top < bottom);
    }

    T area() const {
        return width() * height();
    }

    void move_to(T x, T y) {
        auto const w = width();
        auto const h = height();

        left   = x;
        right  = x + w;
        top    = y;
        bottom = y + h;
    }

    void move_by(difference_t dx, difference_t dy) {
        left   += dx;
        right  += dx;
        top    += dy;
        bottom += dy;
    }

    size_t width()  const { return right - left; }
    size_t height() const { return bottom - top; }

    template <typename U>
    bool contains(point2d<U> p) const {
        return contains(p.x, p.y);
    }

    template <typename U, typename V>
    bool contains(U x, V y) const {
        return x >= left && x < right &&
               y >= top  && y < bottom; 
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

    T left, top, right, bottom;
};

template <typename T, typename U, typename V>
inline rect<T> translate(rect<T> const& r, U dx, V dy) {
    return rect<T>(
        r.left   + dx,
        r.top    + dy,
        r.right  + dx,
        r.bottom + dy
    );
}

template <typename T, typename U>
inline auto intersection_of(rect<T> const& a, rect<U> const& b)
    -> rect<typename std::common_type<T, U>::type>
{
    range<T> const ax(a.left, a.right);
    range<T> const ay(a.top,  a.bottom);
    range<U> const bx(b.left, b.right);
    range<U> const by(b.top,  b.bottom);

    auto const ix = intersection_of(ax, bx);
    auto const iy = intersection_of(ay, by);

    return rect<typename std::common_type<T, U>::type>(
        ix.first, iy.first, ix.last, iy.last
    );
}

template <typename T, typename U>
inline bool operator==(rect<T> const& a, rect<U> const& b) {
    return
        (a.left   == b.left)   &&
        (a.top    == b.top)    &&
        (a.right  == b.right)  &&
        (a.bottom == b.bottom);
}

template <typename T, typename U>
inline bool operator!=(rect<T> const& a, rect<U> const& b) {
    return !(a == b);
}

//==============================================================================
//! Compile time version of #separate_rects_toward.
//! @tparam D #direction.
//==============================================================================
template <direction D> struct separate_rects;

#define BK_SPECIALIZE_SEPARATE_RECTS(dir, dx, dy)                              \
    template <> struct separate_rects<direction::dir> {                        \
        template <typename T>                                                  \
        static rect<T> get(                                                    \
            rect<T> const source,                                              \
            rect<T> const reference,                                           \
            T       const padding = 0                                          \
        ) {                                                                    \
            return translate(                                                  \
                source,                                                        \
                get_x_axis_vector<direction::dir>::value * (dx),               \
                get_y_axis_vector<direction::dir>::value * (dy)                \
            );                                                                 \
        }                                                                      \
    }

BK_SPECIALIZE_SEPARATE_RECTS(north, 0, padding + source.bottom - reference.top);
BK_SPECIALIZE_SEPARATE_RECTS(south, 0, padding + reference.bottom - source.top);
BK_SPECIALIZE_SEPARATE_RECTS(west,  padding + source.right - reference.left, 0);
BK_SPECIALIZE_SEPARATE_RECTS(east,  padding + reference.right - source.left, 0);

#undef BK_SPECIALIZE_SEPARATE_RECTS

//==============================================================================
//! @param dir The #direction in which to translate @c source.
//! @param source The #rect to translate.
//! @param reference The #rect to translate relative to.
//! @param padding The amount of extra space to leave between @c source and
//!     @c reference.
//! @returns A @c source translated in #direction @c dir such that @source and
//!     @c reference do not intersect.
//!
//! @pre @c dir is a cardinal direction (NSEW).
//==============================================================================
template <typename T>
rect<T> separate_rects_toward(
    direction const dir,
    rect<T>   const source,
    rect<T>   const reference,
    T         const padding = 0
) {
    switch (dir) {
    case direction::north :
        return separate_rects<direction::north>::get(source, reference, padding);
    case direction::south :
        return separate_rects<direction::south>::get(source, reference, padding);
    case direction::west :
        return separate_rects<direction::west>::get(source, reference, padding);
    case direction::east :
        return separate_rects<direction::east>::get(source, reference, padding);
    }

    BK_ASSERT(false);

    return source;
}
//==============================================================================