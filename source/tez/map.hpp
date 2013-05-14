#pragma once

#include "grid2d.hpp"
#include "tile.hpp"
#include "room.hpp"

namespace tez {

//==============================================================================
// A 2D grid of tiles.
//==============================================================================
class map {
public:
    typedef grid2d<tile_data>   grid_t;
    typedef bklib::point2d<unsigned>   position;
    typedef grid_t::block       block;
    typedef grid_t::const_block const_block;

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

    tile_data const& at(position p) const {
        return data_.at(p.x, p.y);
    }

    tile_data& at(position p) {
        return data_.at(p.x, p.y);
    }

    const_block block_at(unsigned x, unsigned y) const {
        return data_.block_at(x, y);
    }

    const_block block_at(position p) const {
        return data_.block_at(p.x, p.y);
    }
    //--------------------------------------------------------------------------
    void add_room(room const& r, signed dx = 0, signed dy = 0);
    //--------------------------------------------------------------------------
    
    bool is_valid_position(unsigned x, unsigned y) const {
        return data_.is_valid_position(x, y);
    }

    bool is_valid_position(position p) const {
        return data_.is_valid_position(p.x, p.y);
    }

    friend std::ostream& operator<<(std::ostream& out, map const& m);
private:
    map(map const&)           BK_DELETE;
    map operator=(map const&) BK_DELETE;

    grid_t data_;
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

    point_t start_point() const {
        BK_ASSERT(path_.size() >= 2);
        return path_.front();
    }

    point_t end_point() const {
        BK_ASSERT(path_.size() >= 2);
        return path_.back();
    }
private:   
    distribution_t path_dist_n_;
    distribution_t path_dist_s_;
    distribution_t path_dist_e_;
    distribution_t path_dist_w_;

    std::vector<point_t> path_;
    random_t random_;
};

} //namespace tez
