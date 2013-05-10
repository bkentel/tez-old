#pragma once

#include "bklib/util.hpp"
#include "bklib/geometry.hpp"

#include "room.hpp"
#include "map.hpp"

#include <vector>
#include <memory>
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
    
    map_layout()
        : extent_x_(0)
        , extent_y_(0)    
    {
    }

    void add_room(room r, random_t random);

    unsigned width()  const { return extent_x_.distance(); }
    unsigned height() const { return extent_y_.distance(); }

    //! return a map representing the layout.
    map make_map() const;

    //!move the origin to (0, 0)
    void normalize();
private:
    bklib::min_max<> extent_x_; //!x range that the rooms occupy.
    bklib::min_max<> extent_y_; //!y range that the rooms occypy.
    
    
    room_list rooms_; //! the rooms.


    //possible locations to attempt to place a new room.
    std::queue<candidate_t> candidates_;
};

} //namespace tez
