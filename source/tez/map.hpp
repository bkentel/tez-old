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

    grid2d<tile_data>::const_block block_at(unsigned x, unsigned y) const {
        return data_.block_at(x, y);
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

class path_generator {
public:
    typedef bklib::random_wrapper<unsigned> random_t;
    typedef bklib::point2d<unsigned> point_t;
    typedef std::discrete_distribution<unsigned> distribution_t;

    explicit path_generator(random_t random);

    bool generate(room const& origin, map const& m, direction dir);

    void write_path(map& out);
private:   
    distribution_t path_dist_n_;
    distribution_t path_dist_s_;
    distribution_t path_dist_e_;
    distribution_t path_dist_w_;

    std::vector<point_t> path_;
    random_t random_;
};
