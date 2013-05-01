#pragma once

#include "util.hpp"

#include <vector>
#include <memory>
#include <queue>

#include "room.hpp"

//==============================================================================
//! 
//==============================================================================
class map_layout {
public:
    typedef std::unique_ptr<room>    room_ptr;
    typedef std::vector<room_ptr>    room_list;
    typedef random_wrapper<unsigned> random_t;
    
    map_layout() {
        extent_x_(0);
        extent_y_(0);
    }

    void add_room(room&& r, random_t random) {
        add_room(
            room_ptr(new room(std::move(r))),
            random
        );
    }

    void add_room(room_ptr room, random_t random);

    unsigned width() const {
        return extent_x_.distance();
    }

    unsigned height() const {
        return extent_y_.distance();
    }

    map make_map() const;

    //move the origin to (0, 0)
    void normalize();
private:
    min_max<signed>       extent_x_;
    min_max<signed>       extent_y_;
    std::vector<room_ptr> rooms_;

    std::queue<
        std::pair<
            direction,
            rect<signed>
        >
    > candidates_;
};
