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

static unsigned const NUM_CARDINAL_DIR = 4u;
static unsigned const NUM_PLANAR_DIR   = 8u;
//==============================================================================
//! Declare arrays defining vectors for each direction.
//==============================================================================
#define BK_DECLARE_DIRECTION_ARRAYS(xname, yname) \
    signed const xname[::tez::NUM_PLANAR_DIR] = { 0, 0, 1, -1,  1, -1, -1, 1}; \
    signed const yname[::tez::NUM_PLANAR_DIR] = {-1, 1, 0,  0, -1,  1, -1, 1}
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

inline direction next_cardinal_direction(direction const d) {
    auto const i = static_cast<size_t>(d);
    return static_cast<direction>((i + 1) % NUM_CARDINAL_DIR);
}

inline direction prev_cardinal_direction(direction const d) {
    auto const i = static_cast<size_t>(d);
    return static_cast<direction>((i - 1) % NUM_CARDINAL_DIR);
}

//==============================================================================
//! Return a uniformly distributed random direction NSEW.
//==============================================================================
template <typename T>
inline direction random_cardinal_direction(T& random) {
    return static_cast<direction>(
        std::uniform_int_distribution<>(0, 3)(random)
    );
}

} // namespace tez
