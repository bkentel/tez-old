#pragma once

#include "bklib/util.hpp"
#include "bklib/geometry.hpp"

#include "room.hpp"
#include "map.hpp"

#include <vector>
#include <queue>

namespace tez {

//==============================================================================
//! Maintains a layout of a variable number of rooms such that no rooms
//! intersect each other.
//==============================================================================
class map_layout {
public:
    typedef bklib::random_wrapper<>      random_t;
    typedef std::vector<room>            room_list;
    typedef bklib::rect<signed>          rect_t;
    typedef std::pair<direction, rect_t> candidate_t;
    
    map_layout(random_t random)
        : random_(random)
        , extent_x_(0)
        , extent_y_(0)    
    {
    }

    //--------------------------------------------------------------------------
    //! Add a room to the layout and take ownership.
    //--------------------------------------------------------------------------    
    void add_room(room r);

    unsigned width()  const { return extent_x_.distance(); }
    unsigned height() const { return extent_y_.distance(); }

    //--------------------------------------------------------------------------
    //! Create a map from the layout
    //--------------------------------------------------------------------------
    map make_map();

    //--------------------------------------------------------------------------
    //! Adjust the layout such that all rooms lie in the positive quadrant.
    //--------------------------------------------------------------------------
    void normalize();
private:
    random_t random_;

    bklib::min_max<> extent_x_; //! x range that the rooms occupy.
    bklib::min_max<> extent_y_; //! y range that the rooms occypy.
    
    room_list rooms_; //! The rooms.

    //! Possible locations to attempt to place a new room relative to.
    std::queue<candidate_t> candidates_;
};

} //namespace tez
