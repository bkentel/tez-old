#pragma once

#include "bklib/geometry.hpp"
#include "bklib/util.hpp"

#include "types.hpp"
#include "grid2d.hpp"
#include "tile_category.hpp"
#include "direction.hpp"

#include <functional>

namespace tez {

//==============================================================================
//! Room.
//==============================================================================
class room {
public:
    typedef unsigned index_t;
    typedef signed   location_t;
    
    typedef bklib::rect<location_t> rect_t;
    typedef bklib::point2d<index_t> connection_point;
    typedef bklib::point2d<location_t> point_t;

    typedef bklib::random_wrapper<> random_t;
    typedef grid2d<tile_category>   grid_t;

    typedef std::function<connection_point (
        room const& room, direction side, random_t random
    )> connection_finder_f;

    typedef grid_t::iterator       iterator;
    typedef grid_t::const_iterator const_iterator;
    //--------------------------------------------------------------------------
    room(grid_t grid, connection_finder_f finder)
        : data_(std::move(grid))
        , rect_(0, 0, data_.width(), data_.height())
        , finder_(std::move(finder))
    {
        BK_ASSERT(rect_);
    }
    
    room(room&& other)
        : data_(std::move(other.data_)) 
        , rect_(other.rect_)
        , finder_(other.finder_)
    {
    }
    //--------------------------------------------------------------------------
    block_iterator_adapter<grid_t> block_iterator() {
        return block_iterator_adapter<grid_t>(data_);
    }

    block_iterator_adapter<grid_t const> block_iterator() const {
        return block_iterator_adapter<grid_t const>(data_);
    }
    //--------------------------------------------------------------------------
    iterator begin() { return data_.begin(); }
    iterator end()   { return data_.end(); }

    const_iterator begin() const { return data_.begin(); }
    const_iterator end()   const { return data_.end(); }
    //--------------------------------------------------------------------------
    room& operator=(room&& rhs) {
        swap(rhs);
        return *this;
    }

    void swap(room& other) {
        using std::swap;

        swap(data_,   other.data_);
        swap(rect_,   other.rect_);
        swap(finder_, other.finder_);
    }
    //--------------------------------------------------------------------------
    unsigned width()  const { return rect_.width(); }
    unsigned height() const { return rect_.height(); }

    signed left()   const { return rect_.left; }
    signed right()  const { return rect_.right; }
    signed top()    const { return rect_.top; }
    signed bottom() const { return rect_.bottom; }

    rect_t const& bounds() const {
        return rect_;
    }
    //--------------------------------------------------------------------------
    void translate_by(signed dx, signed dy) {
        rect_.translate_by(dx, dy);
    }

    void translate_to(signed x, signed y) {
        rect_.translate_to(x, y);
    }
    //--------------------------------------------------------------------------
    tile_category at(unsigned x, unsigned y) const {
        return data_.at(x, y);
    }
    
    tile_category& at(unsigned x, unsigned y) {
        return data_.at(x, y);
    }

    bool contains(point_t const p) const {
        return bounds().intersects(p);
    }
    
    bool contains(signed x, signed y) const {
        return contains(bklib::make_point(x, y));
    }

    grid_block<tile_category, true> block_at(unsigned x, unsigned y) const {
        return data_.block_at(x, y);
    }

    connection_point find_connection_point(
        direction const side,
        random_t        random
    ) const {
        BK_ASSERT(finder_);
        return finder_(*this, side, random);
    }
private:
    room(room const&)            BK_DELETE;
    room& operator=(room const&) BK_DELETE;

    grid_t              data_;   //<! Tile data.
    rect_t              rect_;   //<! Bounds.
    connection_finder_f finder_; //<! Algorithm to find an edge.
};

inline void swap(room& a, room& b) {
    a.swap(b);
}

} //namespace tez
