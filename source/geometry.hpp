#pragma once

template <typename T>
struct point2d {
    point2d(T x, T y)
        : x(x)
        , y(y)
    {
    }

    T x, y;
};

////////////////////////////////////////////////////////////////////////////////
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

    difference_t size() const {
        return last - first;
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

//template <typename T, typename U>
//inline auto union_of(range<T> const& a, range<U> const& b)
//    -> range<typename std::common_type<T, U>::type>
//{
//    return range<typename std::common_type<T, U>::type>(
//        a.first < b.first ? a.first : b.first,
//        a.last  > b.last  ? a.last  : b.last
//    );
//}

template <typename T, typename U>
inline bool operator==(range<T> const& a, range<U> const& b) {
    return (a.first == b.first) && (a.last == b.last);
}

template <typename T, typename U>
inline bool operator!=(range<T> const& a, range<U> const& b) {
    return !(a == b);
}
////////////////////////////////////////////////////////////////////////////////
// struct range;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct rect {
    typedef typename std::make_unsigned<T>::type difference_t;

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

    bool is_rect() const {
        return (left <= right) && (top <= bottom);
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

    difference_t width()  const { return right - left; }
    difference_t height() const { return bottom - top; }

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

//template <typename T, typename U>
//inline auto union_of(rect<T> const& a, rect<U> const& b)
//    -> rect<typename std::common_type<T, U>::type>
//{
//    range<T> const ax(a.left, a.right);
//    range<T> const ay(a.top,  a.bottom);
//    range<U> const bx(b.left, b.right);
//    range<U> const by(b.top,  b.bottom);
//
//    auto const ux = union_of(ax, bx);
//    auto const uy = union_of(ay, by);
//
//    return rect<typename std::common_type<T, U>::type>(
//        ux.first, uy.first, ux.last, uy.last
//    );
//}

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
////////////////////////////////////////////////////////////////////////////////
// struct rect;
////////////////////////////////////////////////////////////////////////////////