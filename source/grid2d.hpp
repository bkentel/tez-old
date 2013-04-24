#pragma once

#include "config.hpp"
#include "assert.hpp"
#include "direction.hpp"

#include <memory>
#include <utility>

//==============================================================================
//! 2D grid of Ts.
//! Implicily movable.
//! Not implicitly copyable.
//==============================================================================
template <typename T>
class grid2d {
public:    
    typedef std::unique_ptr<T[]> data_t;

    grid2d(unsigned w, unsigned h, T default)
        : grid2d(w, h, data_t(new T[w*h]))
    {
        std::fill_n(data_.get(), w*h, default);
    }

    grid2d(grid2d&& other)
        : grid2d(other.width_, other.height_, std::move(other.data_))
    {
    }

    grid2d& operator=(grid2d&& rhs) {
        rhs.swap(*this);
        return *this;
    }

    void swap(grid2d& other) {
        std::swap(width_,  other.width_);
        std::swap(height_, other.height_);
        std::swap(data_,   other.data_);
    }

    grid2d clone() const {
        auto const size = width()*height();
        data_t copy(new T[size]);
        
        std::copy_n(data_.get(), size, copy.get());

        return grid2d(width_, height_, std::move(copy));
    }

    bool is_valid_index(unsigned x, unsigned y) const {
        return x < width_ && y < height_;
    }

    T at(unsigned x, unsigned y) const {
        BK_ASSERT(is_valid_index(x, y));
        return *(data_.get() + to_index_(x, y));
    }

    void set(unsigned x, unsigned y, T value) {
        BK_ASSERT(is_valid_index(x, y));
        *(data_.get() + to_index_(x, y)) = value;
    }

    unsigned width()  const { return width_; }
    unsigned height() const { return height_; }
protected:
    grid2d()                         BK_DELETE;
    grid2d(grid2d const&)            BK_DELETE;
    grid2d& operator=(grid2d const&) BK_DELETE;

    grid2d(unsigned w, unsigned h, data_t data)
        : width_(w)
        , height_(h)
        , data_(std::move(data))        
    {
    }

    size_t to_index_(unsigned x, unsigned y) const {
        return x + y*width_;
    }

    unsigned width_;
    unsigned height_;
    data_t   data_;
};

template <typename T>
inline void swap(grid2d<T>& a, grid2d<T>& b) {
    a.swap(b);
}

//==============================================================================
//! Set of all points adjactent to a given tile in 8 cardinal directions.
//==============================================================================
template <typename T>
class block {
public:
    #define BK_GET_NEIGHBOR(DX, DY) get_(data, (x + DX), (y + DY), fallback)

    block(grid2d<T> const& data, unsigned x, unsigned y, T fallback)
        : nw(BK_GET_NEIGHBOR(-1, -1)), n(BK_GET_NEIGHBOR(0, -1)), ne(BK_GET_NEIGHBOR(1, -1))
        , w (BK_GET_NEIGHBOR(-1,  0)),                            e (BK_GET_NEIGHBOR(1,  0))
        , sw(BK_GET_NEIGHBOR(-1,  1)), s(BK_GET_NEIGHBOR(0,  1)), se(BK_GET_NEIGHBOR(1,  1))
    {
    }

    #undef BK_GET_NEIGHBOR

    template <direction Dir> T get() const;

    template <> T get<direction::north_west>() const { return nw; }
    template <> T get<direction::north>()      const { return n; }
    template <> T get<direction::north_east>() const { return ne; }
    template <> T get<direction::west>()       const { return w; }
    template <> T get<direction::east>()       const { return e; }
    template <> T get<direction::south_west>() const { return sw; }
    template <> T get<direction::south>()      const { return s; }
    template <> T get<direction::south_east>() const { return se; }

    T nw, n, ne,
      w,     e,
      sw, s, se;
private:
    static T get_(grid2d<T> const& data, unsigned x, unsigned y, T fallback) {
        return data.is_valid_index(x, y) ? data.at(x, y) : fallback;
    }
};

//==============================================================================
//! For each cell in grid do function(x, y, block(x, y, default)).
//==============================================================================
template <typename F, typename T>
void for_each_block(
    grid2d<T>& grid,
    unsigned   xmin,
    unsigned   xmax,
    unsigned   ymin,
    unsigned   ymax,
    T          default,
    F&&        function
) {
    for (unsigned yi = ymin; yi < ymax; ++yi) {
        for (unsigned xi = xmin; xi < xmax; ++xi) {
            function(xi, yi, block<T>(grid, xi, yi, default));
        }
    }
}
