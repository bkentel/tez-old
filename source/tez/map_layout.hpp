#pragma once

#include "util.hpp"

#include <vector>
#include <memory>
#include <queue>

#include "room.hpp"

//==============================================================================
//! Maintains a layout of a variable number of rooms such that no rooms
//! intersect each other.
//==============================================================================
class map_layout {
public:
    typedef std::unique_ptr<room>    room_ptr;
    typedef std::vector<room   >    room_list;
    typedef random_wrapper<unsigned> random_t;
    
    map_layout()
        : extent_x_(0)
        , extent_y_(0)    
    {
    }

    void add_room(room r, random_t random);

    unsigned width() const {
        return extent_x_.distance();
    }

    unsigned height() const {
        return extent_y_.distance();
    }

    //! return a map representing the layout.
    map make_map() const;

    //!move the origin to (0, 0)
    void normalize();
private:
    min_max<signed> extent_x_; //!x range that the rooms occupy.
    min_max<signed> extent_y_; //!y range that the rooms occypy.
    
    room_list rooms_; //! the rooms.

    std::queue<
        std::pair<
            direction,
            rect<signed>
        >
    > candidates_; //possible locations to attempt to place a new room.
};
