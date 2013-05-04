#pragma once

#include "grid2d.hpp"
#include "tile.hpp"
#include "room.hpp"

//==============================================================================
// A 2D grid of tiles.
//==============================================================================
class map {
public:
    map(unsigned width, unsigned height);

    map(map&& other)
        : data_(std::move(other.data_))
    {
    }

    map& operator=(map&& rhs) {
        swap(rhs);
        return *this;
    }
    //--------------------------------------------------------------------------
    void swap(map& other) {
        using std::swap;
        swap(data_, other.data_);
    }
    //--------------------------------------------------------------------------
    unsigned width()  const { return data_.width();  }
    unsigned height() const { return data_.height(); }
    //--------------------------------------------------------------------------
    tile_data const& at(unsigned x, unsigned y) const {
        return data_.at(x, y);
    }

    tile_data& at(unsigned x, unsigned y) {
        return data_.at(x, y);
    }
    //--------------------------------------------------------------------------
    void add_room(room const& r, signed dx = 0, signed dy = 0);
    //--------------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& out, map const& m);
private:
    map(map const&)           BK_DELETE;
    map operator=(map const&) BK_DELETE;

    grid2d<tile_data> data_;
};

inline void swap(map& a, map& b) {
    a.swap(b);
}
