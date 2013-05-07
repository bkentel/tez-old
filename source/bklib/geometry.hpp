#pragma once

#include <type_traits>

namespace bklib {


template <typename T, bool Integral = std::is_integral<T>::value>
struct make_difference_type {
    typedef typename std::make_unsigned<T>::type type;
};

template <typename T>
struct make_difference_type<T, false> {
    typedef typename std::enable_if<
        std::is_floating_point<T>::value, T
    >::type type;
};

//==============================================================================
//! Distance (difference) between two values; always positive.
//==============================================================================
template <
    typename T,
    typename R = typename make_difference_type<T>::type
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
    typename R = typename make_difference_type<T>::type
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

    T x, y;
}; // struct point2d

template <
    typename T,
    typename U,
    typename R = typename make_difference_type<
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
////==============================================================================
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

        return R(
            a.first < b.first ? b.first : a.first,
            a.last  > b.last  ? b.last  : a.last
        );
    }

    template <typename U>
    bool intersects(range<U> const& b) const {
        return intersection_with(b);
    }

    template <typename U>
    bool intersects(U const n) const {
        return n >= first && n <= last;
    }

    typename make_difference_type<T>::type magnitude() const {
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
    typedef typename make_difference_type<T>::type size_type;

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
    
    template <typename U>
    rect(U left, U top, U right, U bottom)
        : left(left), top(top), right(right), bottom(bottom)
    {
    }

    explicit operator bool() const {
        return (left < right) && (top < bottom);
    }

    template <typename U, typename V>
    rect& translate_to(U const x, V const y) {
        auto const w = width();
        auto const h = height();

        left   = x;
        right  = x + w;
        top    = y;
        bottom = y + h;
    }

    template <typename U, typename V>
    rect& translate_by(U const dx, V const dy) {
        left   += dx;
        right  += dx;
        top    += dy;
        bottom += dy;

        return *this;
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
            << "--bottom = " << r.bottom << "\n"
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

////==============================================================================
////! Compile time version of #separate_rects_toward.
////! @tparam D #direction.
////==============================================================================
//template <direction D> struct separate_rects;
//
//#define BK_SPECIALIZE_SEPARATE_RECTS(dir, dx, dy)                              \
//    template <> struct separate_rects<direction::dir> {                        \
//        template <typename T>                                                  \
//        static rect<T> get(                                                    \
//            rect<T> const source,                                              \
//            rect<T> const reference,                                           \
//            T       const padding = 0                                          \
//        ) {                                                                    \
//            return translate(                                                  \
//                source,                                                        \
//                get_x_axis_vector<direction::dir>::value * (dx),               \
//                get_y_axis_vector<direction::dir>::value * (dy)                \
//            );                                                                 \
//        }                                                                      \
//    }
//
//BK_SPECIALIZE_SEPARATE_RECTS(north, 0, padding + source.bottom - reference.top);
//BK_SPECIALIZE_SEPARATE_RECTS(south, 0, padding + reference.bottom - source.top);
//BK_SPECIALIZE_SEPARATE_RECTS(west,  padding + source.right - reference.left, 0);
//BK_SPECIALIZE_SEPARATE_RECTS(east,  padding + reference.right - source.left, 0);
//
//#undef BK_SPECIALIZE_SEPARATE_RECTS
//
////==============================================================================
////! @param dir The #direction in which to translate @c source.
////! @param source The #rect to translate.
////! @param reference The #rect to translate relative to.
////! @param padding The amount of extra space to leave between @c source and
////!     @c reference.
////! @returns A @c source translated in #direction @c dir such that @source and
////!     @c reference do not intersect.
////!
////! @pre @c dir is a cardinal direction (NSEW).
////==============================================================================
//template <typename T>
//rect<T> separate_rects_toward(
//    direction const dir,
//    rect<T>   const source,
//    rect<T>   const reference,
//    T         const padding = 0
//) {
//    switch (dir) {
//    case direction::north :
//        return separate_rects<direction::north>::get(source, reference, padding);
//    case direction::south :
//        return separate_rects<direction::south>::get(source, reference, padding);
//    case direction::west :
//        return separate_rects<direction::west>::get(source, reference, padding);
//    case direction::east :
//        return separate_rects<direction::east>::get(source, reference, padding);
//    }
//
//    BK_ASSERT(false);
//
//    return source;
//}
////==============================================================================

} //namespace
