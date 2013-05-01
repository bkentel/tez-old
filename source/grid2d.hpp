#pragma once

#include "config.hpp"
#include "assert.hpp"

#include <memory>
#include <utility>
#include <vector>
#include <iterator>

template <typename T> class grid2d;
template <typename T> class grid_block;
template <typename T> class block_iterator;
template <typename T> class block_iterator_adapter;

//==============================================================================
//! A 2D grid of values.
//!
//! @remark Move-only type.
//==============================================================================
template <typename T>
class grid2d {
public:
    //--------------------------------------------------------------------------
    typedef std::vector<T> storage;
    
    typedef typename storage::value_type      value_type;

    typedef typename storage::reference       reference;
    typedef typename storage::const_reference const_reference;

    typedef typename storage::pointer         pointer;
    typedef typename storage::const_pointer   const_pointer;

    typedef typename storage::iterator        iterator;
    typedef typename storage::const_iterator  const_iterator;

    typedef std::pair<unsigned, unsigned>     position;
    typedef size_t                            index;
    //--------------------------------------------------------------------------
    grid2d()
        : width_(0)
        , height_(0)
        , data_()
    {
    }

    grid2d(grid2d&& other)
        : width_(other.width_)
        , height_(other.height_)
        , data_(std::move(other.data_))
    {
        other.width_  = 0;
        other.height_ = 0;
    }

    grid2d(unsigned w, unsigned h)
        : width_(w)
        , height_(h)
        , data_(w*h)
    {
        BK_ASSERT(w > 0);
        BK_ASSERT(h > 0);
    }

    //--------------------------------------------------------------------------
    // Contruct and fill with @c value.
    // @remark Enabled for <tt>is_copy_assignable<T> = true</tt> only.
    //--------------------------------------------------------------------------
    grid2d(unsigned w, unsigned h, T const& value,
        typename std::enable_if<
            std::is_copy_assignable<T>::value
        >::type* = nullptr
    )
        : width_(w)
        , height_(h)
        , data_(w*h, value)
    {    
    }

    //--------------------------------------------------------------------------
    // Contruct and fill with the values return from @c function.
    //--------------------------------------------------------------------------    
    grid2d(
        unsigned w, unsigned h,
        std::function<T (unsigned x, unsigned y)> function
    )
        : width_(w)
        , height_(h)
        , data_()
    {
        BK_ASSERT(w > 0);
        BK_ASSERT(h > 0);

        data_.reserve(w*h);

        for (unsigned y = 0; y < h; ++y) {
            for (unsigned x = 0; x < w; ++x) {
                data_.emplace_back(function(x, y));
            }
        }
    }
    //--------------------------------------------------------------------------
    grid2d& operator=(grid2d&& rhs) {
        rhs.swap(*this);
        return *this;
    }
    
    friend std::ostream& operator<<(std::ostream& out, grid2d const& g) {
        out << "grid2d [" << g.width() << ", " << g.height() << "]";

        unsigned count = 0;
        std::for_each(std::cbegin(g), std::cend(g) [](const_reference value) {
            if (value++ % g.width() == 0) {
                out << '\n';
            }
            
            out << value;
        });

        return std::cout << std::endl;
    }

    void swap(grid2d& other) {
        using std::swap;
        swap(width_,  other.width_);
        swap(height_, other.height_);
        swap(data_,   other.data_);
    }

    grid2d clone() const {
        using ::clone;

        auto result = grid2d();
        result.data_.reserve(size());

        std::transform(cbegin(), cend(), std::back_inserter(result.data_),
            [](const_reference x) {
                return clone(x);
            }
        );
        
        result.width_  = width_;
        result.height_ = height_;

        return result;
    }    
    //--------------------------------------------------------------------------
    iterator begin() { return std::begin(data_); }
    iterator end()   { return std::end(data_); }

    const_iterator begin() const { return cbegin(); }
    const_iterator end()   const { return cend(); }

    const_iterator cbegin() const { return std::cbegin(data_); }
    const_iterator cend()   const { return std::cend(data_); }

    position to_position(iterator const& it) const {
        return to_position_(std::distance(begin(), it));
    }

    position to_position(const_iterator const& it) const {
        return to_position_(std::distance(cbegin(), it));
    }

    pointer       data()       { return data_.data(); }
    const_pointer data() const { return data_.data(); }

    reference       at(unsigned x, unsigned y)       { return data_[to_index_(x, y)]; }
    const_reference at(unsigned x, unsigned y) const { return data_[to_index_(x, y)]; }

    reference       at(position p)       { return at(p.first, p.second); }
    const_reference at(position p) const { return at(p.first, p.second); }
    //--------------------------------------------------------------------------
    unsigned width()  const { return width_; }
    unsigned height() const { return height_; }
    size_t   size()   const { return data_.size(); }
    //--------------------------------------------------------------------------
    bool is_valid_position(unsigned x, unsigned y) const {
        return x < width() && y < height();
    }
private:
    grid2d(grid2d const&)            BK_DELETE;
    grid2d& operator=(grid2d const&) BK_DELETE;

    size_t to_index_(unsigned const x, unsigned const y) const {
        BK_ASSERT(is_valid_position(x, y));
        return x + y*width_;
    }

    position to_position_(size_t const i) const {
        BK_ASSERT(i < size_);
        return std::make_pair(i % width_, i / width_);
    }

    unsigned width_;
    unsigned height_;
    storage  data_;
}; //class grid2d

template <typename T>
inline void swap(grid2d<T>& a, grid2d<T>& b) {
    a.swap(b);
}

template <typename T>
inline grid2d<T> clone(grid2d<T> const& grid) {
    return grid.clone();
}
//==============================================================================

//==============================================================================
//! References a block of Ts from a grid2d<T>.
//!
//! @remark Not default constructible.
//!
//! +------------+------------+------------+
//! | north_west | north      | north_east |
//! +------------+------------+------------+
//! | west       | here       | east       |
//! +------------+------------+------------+
//! | south_west | south      | south_east |
//! +------------+------------+------------+
//!
//! The direction accessors return nullptr when their position would lie
//! outside the grid2d.
//!
//==============================================================================
template <typename T>
class grid_block {
    template <typename U, typename V>
    friend bool operator==(grid_block<U> const& a, grid_block<V> const& b);
public:
    typedef grid2d<T>                         grid_type;
    typedef typename grid2d<T>::pointer       pointer;
    typedef typename grid2d<T>::const_pointer const_pointer;

    grid_block(grid_type& grid, unsigned x, unsigned y)
        : grid_(std::addressof(grid))
        , x(x)
        , y(y)
    {
        BK_ASSERT(x <= grid.width());
        BK_ASSERT(y <= grid.height());
    }
    
    pointer here()       { return &grid_->at(x, y); }
    pointer north()      { return get_( 0, -1); }
    pointer south()      { return get_( 0,  1); }
    pointer east()       { return get_( 1,  0); }
    pointer west()       { return get_(-1,  0); }
    pointer north_east() { return get_( 1, -1); }
    pointer north_west() { return get_(-1, -1); }
    pointer south_east() { return get_( 1,  1); }
    pointer south_west() { return get_(-1,  1); }

    const_pointer here()       const { return &grid_->at(x, y); }
    const_pointer north()      const { return get_( 0, -1); }
    const_pointer south()      const { return get_( 0,  1); }
    const_pointer east()       const { return get_( 1,  0); }
    const_pointer west()       const { return get_(-1,  0); }
    const_pointer north_east() const { return get_( 1, -1); }
    const_pointer north_west() const { return get_(-1, -1); }
    const_pointer south_east() const { return get_( 1,  1); }
    const_pointer south_west() const { return get_(-1,  1); }

    unsigned x, y;
private:
    grid_block() BK_DELETE;

    grid_type* grid_;

    pointer get_(signed dx, signed dy) {
        //allow the overflow
        return grid_->is_valid_position(x+dx, y+dy)
            ? &grid_->at(x+dx, y+dy)
            : nullptr;
    }

    const_pointer get_(signed dx, signed dy) const {
        return const_cast<grid_block*>(this)->get_(dx, dy);
    }
}; //class grid_block

template <typename A, typename B>
inline bool operator==(grid_block<A> const& a, grid_block<B> const& b) {
    return (a.grid_ == b.grid_) && (a.x == b.x) && (a.y == b.y);
}

template <typename A, typename B>
inline bool operator!=(grid_block<A> const& a, grid_block<B> const& b) {
    return !(a == b);
}
//==============================================================================

//==============================================================================
//! Iterator for block by block access.
//==============================================================================
template <typename T>
class block_iterator
    : public std::iterator<std::random_access_iterator_tag, grid_block<T>>
{
    friend class block_iterator_adapter<T>;

    template <typename A, typename B>
    friend bool operator==(block_iterator<A> const& a, block_iterator<B> const& b);

    template <typename A, typename B>
    friend bool operator!=(block_iterator<A> const& a, block_iterator<B> const& b);

    template <typename A, typename B>
    friend bool operator<(block_iterator<A> const& a, block_iterator<B> const& b);
public:
    template <typename U>
    block_iterator(block_iterator<U> const& rhs)
        : data_(rhs.data_)
        , offset_(rhs.offset_)
        , block_(rhs.block_)
    {
    }
   
    template <typename U>
    block_iterator& operator=(block_iterator<U> const& rhs)
    {
        data_     = rhs.data_;
        offset_   = rhs.offset_;
        block_    = rhs.block_;
    
        return *this;   
    }

    block_iterator& operator++() {
        return *this += 1;
    }

    block_iterator& operator++(int) {
        return ++(*this);
    }

    block_iterator& operator--() {
        return *this -= 1;
    }

    block_iterator& operator--(int) {
        return --(*this);
    }

    block_iterator operator+(difference_type n) const {
        return block_iterator(*this) += n;
    }

    block_iterator operator-(difference_type n) const {
        return block_iterator(*this) -= n;
    }

    template <typename U>
    difference_type operator-(block_iterator<U> const& rhs) const {
        BK_ASSERT(data_ == rhs.data_);
        return offset_ - rhs.offset_;
    }

    block_iterator& operator+=(difference_type n) {
        BK_ASSERT(n >= 0);
        BK_ASSERT(offset_ + n <= size_());

        offset_ += n;
        return *this;
    }

    block_iterator& operator-=(difference_type n) {
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

    //point2d<unsigned> position() const {
    //    return point2d<unsigned>(offset_ % data_->width(), offset_ / data_->width());
    //}
private:
    block_iterator(grid2d<T>& data, size_t offset)
        : data_(&data)
        , offset_(offset)
        , block_(*data_, x_(offset), y_(offset))
    {
        BK_ASSERT(offset_ <= size_());
    }

    block_iterator(block_iterator& other, size_t offset)
        : block_iterator(other.data_, offset)
    {
    }

    size_t size_() const {
        return data_->width() * data_->height();
    }
    
    unsigned x_(size_t offset) const {
        return data_->width()
            ? static_cast<unsigned>(offset % data_->width())
            : 0;
    }

    unsigned y_(size_t offset) const {
        return data_->width()
            ? static_cast<unsigned>(offset / data_->width())
            : 0;
    }

    pointer at_(size_t const offset) const {
        BK_ASSERT(offset < size_());

        auto const xi = x_(offset);
        auto const yi = y_(offset);

        block_ = grid_block<T>(*data_, xi, yi);

        return &block_;
    }

    grid2d<T>*            data_;
    size_t                offset_;
    mutable grid_block<T> block_;    // updated only as needed
}; //class block_iterator

template <typename A, typename B>
inline bool operator<(block_iterator<A> const& a, block_iterator<B> const& b) {
    BK_ASSERT(a.data_ == b.data_);
    return a.offset_ < b.offset_;
}

template <typename A, typename B>
inline bool operator==(block_iterator<A> const& a, block_iterator<B> const& b) {
    return (a.data_ == b.data_) && (a.offset_ == b.offset_);
}

template <typename A, typename B>
inline bool operator!=(block_iterator<A> const& a, block_iterator<B> const& b) {
    return !(a == b);
}
//==============================================================================

//==============================================================================
//! Adapter for block_iterator to work with STL algorithms.
//==============================================================================
template <typename T>
class block_iterator_adapter {
public:
    typedef block_iterator<T> iterator;

    block_iterator_adapter(grid2d<T>& grid)
        : grid_(&grid)
    {
    }

    iterator begin() {
        return iterator(*grid_, 0);
    }

    iterator end() {
        return iterator(*grid_, grid_->size());
    }
private:
    grid2d<T>* grid_;
};

//==============================================================================
//! Helper function to create a block_iterator_adapter.
//==============================================================================
template <typename T>
block_iterator_adapter<T> make_block_iterator_adapter(grid2d<T>& grid) {
    return block_iterator_adapter<T>(grid);
}


//template <typename U, typename F>
//inline void transfer(
//    grid2d<U> const& src,
//    unsigned src_x,
//    unsigned src_y,
//    unsigned src_w,
//    unsigned src_h,
//    unsigned dest_x,
//    unsigned dest_y,
//    F&& function
//) {
//    BK_ASSERT(src_x + src_w <= src.width());
//    BK_ASSERT(src_y + src_h <= src.height());
//
//    BK_ASSERT(dest_x + src_w <= width());
//    BK_ASSERT(dest_y + src_h <= height());
//
//    for (unsigned y = 0; y < src_h; ++y) {
//        for (unsigned x = 0; x < src_w; ++x) {
//            set(
//                dest_x + x,
//                dest_y + y, 
//                function(
//                    src.at(src_x + x, src_y + y)
//                )
//            );
//        }
//    }
//}

////==============================================================================
////! Iterator for element by element access.
////==============================================================================
//template <typename T>
//class grid2d_iterator
//    : public std::iterator<std::random_access_iterator_tag, T>
//{
//    template <typename U> friend class grid2d;
//
//    template <typename A, typename B>
//    friend bool operator==(grid2d_iterator<A> const& a, grid2d_iterator<B> const& b);
//
//    template <typename A, typename B>
//    friend bool operator!=(grid2d_iterator<A> const& a, grid2d_iterator<B> const& b);
//public:
//    template <typename U>
//    grid2d_iterator(grid2d_iterator<U> const& rhs)
//        : data_(rhs.data_)
//        , width_(rhs.width_)
//        , height_(rhs.height_)
//        , offset_(rhs.offset_)
//    {
//    }
//   
//    template <typename U>
//    grid2d_iterator& operator=(grid2d_iterator<U> const& rhs)
//    {
//        data_   = rhs.data_;
//        width_  = rhs.width_;
//        height_ = rhs.height_;
//        offset_ = rhs.offset_;
//    
//        return *this;   
//    }
//
//    grid2d_iterator& operator++() {
//        return *this += 1;
//    }
//
//    grid2d_iterator& operator++(int) {
//        return ++(*this);
//    }
//
//    grid2d_iterator& operator--() {
//        return *this -= 1;
//    }
//
//    grid2d_iterator& operator--(int) {
//        return --(*this);
//    }
//
//    grid2d_iterator operator+(difference_type n) const {
//        return room_iterator(*this) += n;
//    }
//
//    grid2d_iterator operator-(difference_type n) const {
//        return room_iterator(*this) -= n;
//    }
//
//    template <typename U>
//    difference_type operator-(grid2d_iterator<U> const& rhs) const {
//        BK_ASSERT(data_ == rhs.data_);
//        return offset_ - rhs.offset_;
//    }
//
//    grid2d_iterator& operator+=(difference_type n) {
//        BK_ASSERT(n >= 0);
//        BK_ASSERT(offset_ + n <= size_());
//
//        offset_ += n;
//        return *this;
//    }
//
//    grid2d_iterator& operator-=(difference_type n) {
//        BK_ASSERT(n >= 0);
//        BK_ASSERT(offset_ >= static_cast<size_t>(n));
//    
//        offset_ -= n;
//        return *this;
//    }
//
//    reference operator*() const {
//        return *at_(offset_);
//    }
//
//    pointer operator->() const {
//        return at_(offset_);
//    }
//
//    reference operator[](difference_type n) const {
//        return *at_(offset_ + n);
//    }
//
//    point2d<unsigned> position() const {
//        return point2d<unsigned>(offset_ % width_, offset_ / width_);
//    }
//private:
//    grid2d_iterator(pointer data, size_t width, size_t height, size_t offset)
//        : data_(data)
//        , width_(width)
//        , height_(height)
//        , offset_(offset)
//    {
//        BK_ASSERT(offset_ <= size_());
//    }
//
//    grid2d_iterator(grid2d_iterator& other, size_t offset)
//        : grid2d_iterator(other.data_, other.width_, other.height_, offset)
//    {
//    }
//
//    size_t size_() const {
//        return width_ * height_;
//    }
//    
//    pointer at_(size_t const offset) const {
//        BK_ASSERT(offset < size_());
//
//        return data_ + offset;
//    }
//
//    pointer data_;
//    size_t  width_, height_;
//    size_t  offset_;
//};
//
//template <typename A, typename B>
//inline bool operator<(grid2d_iterator<A> const& a, grid2d_iterator<B> const& b) {
//    BK_ASSERT(a.data_ == b.data_);
//    return a.offset_ < b.offset_;
//}
//
//template <typename A, typename B>
//inline bool operator==(grid2d_iterator<A> const& a, grid2d_iterator<B> const& b) {
//    return (a.data_   == b.data_)   && 
//           (a.width_  == b.width_)  &&
//           (a.height_ == b.height_) &&
//           (a.offset_ == b.offset_);
//}
//
//template <typename A, typename B>
//inline bool operator!=(grid2d_iterator<A> const& a, grid2d_iterator<B> const& b) {
//    return !(a == b);
//}

//template <typename T>
//class const_grid_block {
//    template <typename U, typename V>
//    friend bool operator==(const_grid_block<U> const& a, const_grid_block<V> const& b);
//
//    template <typename U, typename V>
//    friend bool operator==(const_grid_block<U> const& a, grid_block<V> const& b);
//
//    template <typename U, typename V>
//    friend bool operator==(grid_block<U> const& a, const_grid_block<V> const& b);
//public:
//    typedef grid2d<T> const                   grid_type;
//    typedef typename grid2d<T>::const_pointer pointer;
//
//    const_grid_block(grid_type& grid, unsigned x, unsigned y)
//        : grid_(std::addressof(grid))
//        , x(x)
//        , y(y)
//    {
//        BK_ASSERT(x < grid.width());
//        BK_ASSERT(y < grid.height());
//    }
//    
//    const_grid_block(grid_block& other)
//        : const_grid_block(*other.grid_, other.x, other.y)
//    {
//    }
//
//    pointer here()       const { return grid_->at(x, y); }
//    pointer north()      const { return get( 0, -1); }
//    pointer south()      const { return get( 0,  1); }
//    pointer east()       const { return get( 1,  0); }
//    pointer west()       const { return get(-1,  0); }
//    pointer north_east() const { return get( 1, -1); }
//    pointer north_west() const { return get(-1, -1); }
//    pointer south_east() const { return get( 1,  1); }
//    pointer south_west() const { return get(-1,  1); }
//
//    unsigned x, y;
//private:
//    const_grid_block() BK_DELETE;
//
//    grid_type* grid_;
//
//    pointer get_(signed dx, signed dy) const {
//        //allow the overflow
//        grid_->is_valid_position(x+dx, y+dy)
//            ? grid_->at(x+dx, y+dy);
//            : nullptr;
//    }
//};



//template <typename T, typename U>
//inline bool operator==(const_grid_block<T> const& a, grid_block<U> const& b) {
//    return (a.grid_ == b.grid_) && (a.x == b.x) && (a.y == b.y);
//}
//
//template <typename T, typename U>
//inline bool operator==(grid_block<T> const& a, const_grid_block<U> const& b) {
//    return (a.grid_ == b.grid_) && (a.x == b.x) && (a.y == b.y);
//}
//
//template <typename T, typename U>
//inline bool operator==(const_grid_block<T> const& a, const_grid_block<U> const& b) {
//    return (a.grid_ == b.grid_) && (a.x == b.x) && (a.y == b.y);
//}
//
//template <typename T, typename U>
//inline bool operator!=(grid_block<T> const& a, grid_block<U> const& b) {
//    return !(a == b);
//}
//
//template <typename T, typename U>
//inline bool operator!=(const_grid_block<T> const& a, grid_block<U> const& b) {
//    return !(a == b);
//}
//
//template <typename T, typename U>
//inline bool operator!=(grid_block<T> const& a, const_grid_block<U> const& b) {
//    return !(a == b);
//}
//
//template <typename T, typename U>
//inline bool operator!=(const_grid_block<T> const& a, const_grid_block<U> const& b) {
//    return !(a == b);
//}
