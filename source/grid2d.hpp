#pragma once

#include "config.hpp"
#include "assert.hpp"
#include "direction.hpp"
#include "geometry.hpp"

#include <memory>
#include <utility>

template <typename T> class block;
template <typename T> class grid2d;
template <typename T> class grid2d_iterator;
template <typename T> class grid2d_block_iterator;

//==============================================================================
//! Iterator for element by element access.
//==============================================================================
template <typename T>
class grid2d_iterator
    : public std::iterator<std::random_access_iterator_tag, T>
{
    template <typename U> friend class grid2d;

    template <typename A, typename B>
    friend bool operator==(grid2d_iterator<A> const& a, grid2d_iterator<B> const& b);

    template <typename A, typename B>
    friend bool operator!=(grid2d_iterator<A> const& a, grid2d_iterator<B> const& b);
public:
    template <typename U>
    grid2d_iterator(grid2d_iterator<U> const& rhs)
        : data_(rhs.data_)
        , width_(rhs.width_)
        , height_(rhs.height_)
        , offset_(rhs.offset_)
    {
    }
   
    template <typename U>
    grid2d_iterator& operator=(grid2d_iterator<U> const& rhs)
    {
        data_   = rhs.data_;
        width_  = rhs.width_;
        height_ = rhs.height_;
        offset_ = rhs.offset_;
    
        return *this;   
    }

    grid2d_iterator& operator++() {
        return *this += 1;
    }

    grid2d_iterator& operator++(int) {
        return ++(*this);
    }

    grid2d_iterator& operator--() {
        return *this -= 1;
    }

    grid2d_iterator& operator--(int) {
        return --(*this);
    }

    grid2d_iterator operator+(difference_type n) const {
        return room_iterator(*this) += n;
    }

    grid2d_iterator operator-(difference_type n) const {
        return room_iterator(*this) -= n;
    }

    template <typename U>
    difference_type operator-(grid2d_iterator<U> const& rhs) const {
        BK_ASSERT(data_ == rhs.data_);
        return offset_ - rhs.offset_;
    }

    grid2d_iterator& operator+=(difference_type n) {
        BK_ASSERT(n >= 0);
        BK_ASSERT(offset_ + n <= size_());

        offset_ += n;
        return *this;
    }

    grid2d_iterator& operator-=(difference_type n) {
        BK_ASSERT(n >= 0);
        BK_ASSERT(offset_ >= static_cast<size_t>(n));
    
        offset_ -= n;
        return *this;
    }

    reference operator*() const {
        return *at_(offset_);
    }

    pointer operator->() const {
        return at_(offset_);
    }

    reference operator[](difference_type n) const {
        return *at_(offset_ + n);
    }

    point2d<unsigned> position() const {
        return point2d<unsigned>(offset_ % width_, offset_ / width_);
    }
private:
    grid2d_iterator(pointer data, size_t width, size_t height, size_t offset)
        : data_(data)
        , width_(width)
        , height_(height)
        , offset_(offset)
    {
        BK_ASSERT(offset_ <= size_());
    }

    grid2d_iterator(grid2d_iterator& other, size_t offset)
        : grid2d_iterator(other.data_, other.width_, other.height_, offset)
    {
    }

    size_t size_() const {
        return width_ * height_;
    }
    
    pointer at_(size_t const offset) const {
        BK_ASSERT(offset < size_());

        return data_ + offset;
    }

    pointer data_;
    size_t  width_, height_;
    size_t  offset_;
};

template <typename A, typename B>
inline bool operator<(grid2d_iterator<A> const& a, grid2d_iterator<B> const& b) {
    BK_ASSERT(a.data_ == b.data_);
    return a.offset_ < b.offset_;
}

template <typename A, typename B>
inline bool operator==(grid2d_iterator<A> const& a, grid2d_iterator<B> const& b) {
    return (a.data_   == b.data_)   && 
           (a.width_  == b.width_)  &&
           (a.height_ == b.height_) &&
           (a.offset_ == b.offset_);
}

template <typename A, typename B>
inline bool operator!=(grid2d_iterator<A> const& a, grid2d_iterator<B> const& b) {
    return !(a == b);
}

//==============================================================================
//! Iterator for block by block access.
//==============================================================================
template <typename T>
class grid2d_block_iterator
    : public std::iterator<std::random_access_iterator_tag, block<T>>
{
    template <typename U> friend class grid2d;

    template <typename A, typename B>
    friend bool operator==(grid2d_block_iterator<A> const& a, grid2d_block_iterator<B> const& b);

    template <typename A, typename B>
    friend bool operator!=(grid2d_block_iterator<A> const& a, grid2d_block_iterator<B> const& b);

    template <typename A, typename B>
    friend bool operator<(grid2d_block_iterator<A> const& a, grid2d_block_iterator<B> const& b);
public:
    template <typename U>
    grid2d_block_iterator(grid2d_block_iterator<U> const& rhs)
        : data_(rhs.data_)
        , offset_(rhs.offset_)
        , fallback_(rhs.fallback_)
        , block_(rhs.block_)
    {
    }
   
    template <typename U>
    grid2d_block_iterator& operator=(grid2d_block_iterator<U> const& rhs)
    {
        data_     = rhs.data_;
        offset_   = rhs.offset_;
        fallback_ = rhs.fallback_;
        block_    = rhs.block_;
    
        return *this;   
    }

    grid2d_block_iterator& operator++() {
        return *this += 1;
    }

    grid2d_block_iterator& operator++(int) {
        return ++(*this);
    }

    grid2d_block_iterator& operator--() {
        return *this -= 1;
    }

    grid2d_block_iterator& operator--(int) {
        return --(*this);
    }

    grid2d_block_iterator operator+(difference_type n) const {
        return room_iterator(*this) += n;
    }

    grid2d_block_iterator operator-(difference_type n) const {
        return room_iterator(*this) -= n;
    }

    template <typename U>
    difference_type operator-(grid2d_block_iterator<U> const& rhs) const {
        BK_ASSERT(data_ == rhs.data_);
        return offset_ - rhs.offset_;
    }

    grid2d_block_iterator& operator+=(difference_type n) {
        BK_ASSERT(n >= 0);
        BK_ASSERT(offset_ + n <= size_());

        offset_ += n;
        return *this;
    }

    grid2d_block_iterator& operator-=(difference_type n) {
        BK_ASSERT(n >= 0);
        BK_ASSERT(offset_ >= static_cast<size_t>(n));
    
        offset_ -= n;
        return *this;
    }

    reference operator*() const {
        return *at_(offset_);
    }

    pointer operator->() const {
        return at_(offset_);
    }

    reference operator[](difference_type n) const {
        return *at_(offset_ + n);
    }

    point2d<unsigned> position() const {
        return point2d<unsigned>(offset_ % data_->width(), offset_ / data_->width());
    }
private:
    grid2d_block_iterator(grid2d<T>* data, size_t offset, T fallback)
        : data_(data)
        , offset_(offset)
        , fallback_(fallback)
        , block_(*data_, x_(offset), y_(offset), fallback)
    {
        BK_ASSERT(offset_ <= size_());
    }

    grid2d_block_iterator(grid2d_block_iterator& other, size_t offset)
        : grid2d_block_iterator(other.data_, offset)
    {
    }

    size_t size_() const {
        return data_->width() * data_->height();
    }
    
    unsigned x_(size_t offset) const {
        return static_cast<unsigned>(offset % data_->width());
    }

    unsigned y_(size_t offset) const {
        return static_cast<unsigned>(offset / data_->width());
    }

    pointer at_(size_t const offset) const {
        BK_ASSERT(offset < size_());

        auto const xi = x_(offset);
        auto const yi = y_(offset);

        //if (offset_ != offset) {
            block_ = block<T>(*data_, xi, yi, fallback_);
        //}

        return &block_;
    }

    grid2d<T>*       data_;
    size_t           offset_;
    T                fallback_;
    mutable block<T> block_;    // updated only as needed
};

template <typename A, typename B>
inline bool operator<(grid2d_block_iterator<A> const& a, grid2d_block_iterator<B> const& b) {
    BK_ASSERT(a.data_ == b.data_);
    return a.offset_ < b.offset_;
}

template <typename A, typename B>
inline bool operator==(grid2d_block_iterator<A> const& a, grid2d_block_iterator<B> const& b) {
    return (a.data_ == b.data_) && (a.offset_ == b.offset_);
}

template <typename A, typename B>
inline bool operator!=(grid2d_block_iterator<A> const& a, grid2d_block_iterator<B> const& b) {
    return !(a == b);
}

//==============================================================================
//! 2D grid of Ts.
//! Implicily movable.
//! Not implicitly copyable.
//==============================================================================
template <typename T>
class grid2d {
public:    
    typedef std::unique_ptr<T[]> data_t;

    typedef grid2d_iterator<T>       iterator;
    typedef grid2d_block_iterator<T> block_iterator;

    block_iterator block_begin(T fallback) {
        return block_iterator(this, 0, fallback);
    }

    block_iterator block_end(T fallback) {
        return block_iterator(this, size_(), fallback);
    }

    iterator begin() {
        return iterator(data_.get(), width_, height_, 0);
    }

    iterator end() {
        return iterator(data_.get(), width_, height_, size_());
    }

    grid2d(unsigned w, unsigned h, T default)
        : grid2d(w, h, data_t(new T[w*h]))
    {
        std::fill_n(data_.get(), w*h, default);
    }

    grid2d(unsigned w, unsigned h)
        : grid2d(w, h, data_t(new T[w*h]))
    {
        //std::fill_n(data_.get(), w*h, default);
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

    block<T> get_block(unsigned x, unsigned y, T fallback) const {
        return block<T>(*this, x, y, fallback);
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

    size_t size_() const {
        return width_ * height_;
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
    #define BK_GN(DX, DY) get_(data, (x + DX), (y + DY), fallback)

    block(grid2d<T> const& data, unsigned x, unsigned y, T fallback)
        : nw(BK_GN(-1, -1)), n   (BK_GN(0, -1)), ne(BK_GN(1, -1))
        , w (BK_GN(-1,  0)), here(BK_GN(0,  0)),  e(BK_GN(1,  0))
        , sw(BK_GN(-1,  1)), s   (BK_GN(0,  1)), se(BK_GN(1,  1))
        , x(x)
        , y(y)
    {
    }
    
    #undef BK_GN  

    template <direction Dir> T get() const;

    template <> T get<direction::north_west>() const { return nw; }
    template <> T get<direction::north>()      const { return n; }
    template <> T get<direction::north_east>() const { return ne; }
    template <> T get<direction::west>()       const { return w; }
    template <> T get<direction::east>()       const { return e; }
    template <> T get<direction::south_west>() const { return sw; }
    template <> T get<direction::south>()      const { return s; }
    template <> T get<direction::south_east>() const { return se; }

    T nw, n,    ne,
      w,  here, e,
      sw, s,    se;

    unsigned x, y;
private:
    static T get_(grid2d<T> const& data, unsigned x, unsigned y, T fallback) {
        return data.is_valid_index(x, y) ? data.at(x, y) : fallback;
    }
};

template <typename T, typename U>
inline bool operator==(block<T> const& a, block<U> const& b) {
    return
        (a.nw == b.nw) && (a.n == b.n)       && (a.ne == b.ne) &&
        (a.w  == b.w)  && (a.here == b.here) && (a.e  == b.e)  &&
        (a.sw == b.sw) && (a.s == b.s)       && (a.se == b.se);
}

template <typename T, typename U>
inline bool operator!=(block<T> const& a, block<U> const& b) {
    return !(a == b);
}
