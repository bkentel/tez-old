#pragma once

#include <type_traits>

namespace tez {
//==============================================================================
//! Cardinal directions
//==============================================================================
enum class direction {
    north,
    south,
    east,
    west,
    north_east,
    south_west,
    north_west,
    south_east,
    up,
    down,
    here,
    none,
};
//==============================================================================
//! Declare arrays defining vectors for each direction.
//==============================================================================
#define BK_DECLARE_DIRECTION_ARRAYS(xname, yname) \
    signed const xname[] = { 0, 0, 1, -1,  1, -1, -1, 1}; \
    signed const yname[] = {-1, 1, 0,  0, -1,  1, -1, 1}
//==============================================================================
//! Return the direction opposite d.
//==============================================================================
inline direction opposite_direction(direction const d) {
    typedef std::underlying_type<direction>::type type;

    return static_cast<direction>(
        static_cast<type>(d) ^ static_cast<type>(1)
    );
}
//==============================================================================
//! Return a direction vector for d.
//==============================================================================
inline std::pair<signed, signed> direction_vector(direction const d) {
    typedef std::underlying_type<direction>::type type;

    auto const i = static_cast<type>(d);

    BK_ASSERT(i < static_cast<type>(direction::up));

    BK_DECLARE_DIRECTION_ARRAYS(xd, yd);
    return std::make_pair(xd[i], yd[i]);
}

} // namespace tez
