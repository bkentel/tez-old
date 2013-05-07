#pragma once

#include <type_traits>

//==============================================================================
//! Cardinal directions
//==============================================================================
enum class direction {
    here       = 0,
    none       = 0,
    north      = 1 << 0,
    south      = 1 << 1,
    east       = 1 << 2,
    west       = 1 << 3,
    north_east = 1 << 4,
    north_west = 1 << 5,
    south_east = 1 << 6,
    south_west = 1 << 7,
    up         = 1 << 8,
    down       = 1 << 9,
};

//==============================================================================
struct axis {};
struct x_axis : public axis {};
struct y_axis : public axis {};
struct z_axis : public axis {};

//==============================================================================
namespace detail {
    template <typename T, direction Dir, signed Val = 0>
    struct get_axis_vector_base {
        static_assert(std::is_base_of<axis, T>::value, "T must be an axis type");
        static_assert(Val == 0 || Val == 1 || Val == -1, "Val must be (-1, 0, 1)");

        static signed const value = Val;
    };
} //namespace detail

template <direction Dir> struct get_x_axis_vector
    : public detail::get_axis_vector_base<x_axis, Dir> {};
template <direction Dir> struct get_y_axis_vector
    : public detail::get_axis_vector_base<y_axis, Dir> {};
template <direction Dir> struct get_z_axis_vector
    : public detail::get_axis_vector_base<z_axis, Dir> {};

#define BK_SPECIALIZE_AXIS(XYZ, D, V) \
template <> struct get_##XYZ##_axis_vector<D> \
    : public detail::get_axis_vector_base<##XYZ##_axis, D, V> {}

BK_SPECIALIZE_AXIS(x, direction::east,        1);
BK_SPECIALIZE_AXIS(x, direction::north_east,  1);
BK_SPECIALIZE_AXIS(x, direction::south_east,  1);
BK_SPECIALIZE_AXIS(x, direction::west,       -1);
BK_SPECIALIZE_AXIS(x, direction::north_west, -1);
BK_SPECIALIZE_AXIS(x, direction::south_west, -1);

BK_SPECIALIZE_AXIS(y, direction::south,       1);
BK_SPECIALIZE_AXIS(y, direction::south_east,  1);
BK_SPECIALIZE_AXIS(y, direction::south_west,  1);
BK_SPECIALIZE_AXIS(y, direction::north,      -1);
BK_SPECIALIZE_AXIS(y, direction::north_east, -1);
BK_SPECIALIZE_AXIS(y, direction::north_west, -1);

BK_SPECIALIZE_AXIS(z, direction::up,         -1);
BK_SPECIALIZE_AXIS(z, direction::down,        1);

#undef BK_SPECIALIZE_AXIS

//==============================================================================
//! Return the directions opposite [dir].
//==============================================================================
inline direction opposite(direction const dir) {
    switch (dir) {
    case direction::south :      return direction::north;
    case direction::north :      return direction::south;
    case direction::east :       return direction::west;
    case direction::north_east : return direction::south_west;
    case direction::south_east : return direction::north_west;
    case direction::west :       return direction::east;
    case direction::north_west : return direction::south_east;
    case direction::south_west : return direction::north_east;
    case direction::here :       return direction::here;
    }

    return direction::none;
}
//==============================================================================
//! Returns a (unit) direction vector along the x-axis.
//==============================================================================
inline signed get_x_vector(direction const dir) {
    switch (dir) {
    case direction::east :
    case direction::north_east :
    case direction::south_east :
        return 1;
    case direction::west :
    case direction::north_west :
    case direction::south_west :
        return -1;
    default :
        return 0;
    }
}
//==============================================================================
//! Returns a (unit) direction vector along the y-axis.
//==============================================================================
inline signed get_y_vector(direction const dir) {
    switch (dir) {
    case direction::south :
    case direction::south_east :
    case direction::south_west :
        return 1;
    case direction::north :
    case direction::north_east :
    case direction::north_west :
        return -1;
    default :
        return 0;
    }
}
//==============================================================================
//! Returns a (unit) direction vector along the z-axis.
//==============================================================================
inline signed get_z_vector(direction const dir) {
    switch (dir) {
    case direction::down :
        return 1;
    case direction::up :
        return -1;
    default :
        return 0;
    }
}
