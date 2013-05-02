#pragma once

#include "config.hpp"
#include "assert.hpp"

#include <memory>
#include <utility>
#include <vector>
#include <iterator>

#include <boost/iterator/iterator_facade.hpp>

template <typename>       class grid2d;
template <typename, bool> class grid_block;
template <typename, bool> class block_iterator;
template <typename>       class block_iterator_adapter;

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
    
    typedef typename T      value_type;

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

    position to_position(size_t const offset) const {
        return to_position_(offset);
    }

    pointer       data()       { return data_.data(); }
    const_pointer data() const { return data_.data(); }

    reference       at(unsigned x, unsigned y)       { return data_[to_index_(x, y)]; }
    const_reference at(unsigned x, unsigned y) const { return data_[to_index_(x, y)]; }

    reference       at(position p)       { return at(p.first, p.second); }
    const_reference at(position p) const { return at(p.first, p.second); }

    reference at_or(unsigned x, unsigned y, reference value) {
        return is_valid_position(x, y) ? at(x, y) : value;
    }

    const_reference at_or(unsigned x, unsigned y, const_reference value) const {
        return is_valid_position(x, y) ? at(x, y) : value;
    }
    //--------------------------------------------------------------------------
    unsigned width()  const { return width_; }
    unsigned height() const { return height_; }
    size_t   size()   const { return data_.size(); }
    //--------------------------------------------------------------------------
    bool is_valid_position(unsigned x, unsigned y) const {
        return x < width() && y < height();
    }

    bool is_valid_position(position p) const {
        return is_valid_position(p.x, p.y);
    }
private:
    grid2d(grid2d const&)            BK_DELETE;
    grid2d& operator=(grid2d const&) BK_DELETE;

    size_t to_index_(unsigned const x, unsigned const y) const {
        BK_ASSERT(is_valid_position(x, y));
        return x + y*width_;
    }

    position to_position_(size_t const i) const {
        BK_ASSERT(i < size());

        return std::make_pair(
            static_cast<unsigned>(width_ ? i % width_ : 0),
            static_cast<unsigned>(width_ ? i / width_ : 0)
        );
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
template <typename T, bool Const>
class grid_block {
    template <typename, bool> friend class grid_block;
public:
    template <typename U>
    struct other_t {
        typedef typename std::conditional<
            Const,
            grid2d<U> const,
            grid2d<U>
        >::type type;
    };

    typedef typename other_t<T>::type         grid_type;
    typedef typename grid_type::pointer       pointer;
    typedef typename grid_type::const_pointer const_pointer;

    grid_block()
        : grid_(nullptr)
        , x(0)
        , y(0)
    {
    }

    grid_block(
        grid_type* grid, unsigned x, unsigned y 
    )
        : grid_(BK_CHECK_PTR(grid))
        , x(x)
        , y(y)
    {
        BK_ASSERT(grid_->is_valid_position(x, y));
    }

    grid_block(
        grid_type* grid, size_t offset
    )
        : grid_(BK_CHECK_PTR(grid))
        , x(grid->to_position(offset).first)
        , y(grid->to_position(offset).second)
    {
    }

    template <typename U, bool C>
    bool operator==(grid_block<U, C> const& rhs) const {
        return (grid_ == rhs.grid_) && (x_ == rhs.x_) && (y_ == rhs.y_);
    }

    pointer here()       { return grid_ ? &grid_->at(x, y) : nullptr; }
    pointer north()      { return get_( 0, -1); }
    pointer south()      { return get_( 0,  1); }
    pointer east()       { return get_( 1,  0); }
    pointer west()       { return get_(-1,  0); }
    pointer north_east() { return get_( 1, -1); }
    pointer north_west() { return get_(-1, -1); }
    pointer south_east() { return get_( 1,  1); }
    pointer south_west() { return get_(-1,  1); }

    const_pointer here()       const { return grid_ ? &grid_->at(x, y) : nullptr; }
    const_pointer north()      const { return get_( 0, -1); }
    const_pointer south()      const { return get_( 0,  1); }
    const_pointer east()       const { return get_( 1,  0); }
    const_pointer west()       const { return get_(-1,  0); }
    const_pointer north_east() const { return get_( 1, -1); }
    const_pointer north_west() const { return get_(-1, -1); }
    const_pointer south_east() const { return get_( 1,  1); }
    const_pointer south_west() const { return get_(-1,  1); }

private :
    grid_type* grid_;
public:
    unsigned x, y;
private:
    pointer get_(signed dx, signed dy) {
        auto const ix = x + dx; // allow overflow
        auto const iy = y + dy; // allow overflow

        return grid_ && grid_->is_valid_position(ix, iy)
            ? &grid_->at(ix, iy)
            : nullptr;        
    }

    const_pointer get_(signed dx, signed dy) const {
        auto const ix = x + dx; // allow overflow
        auto const iy = y + dy; // allow overflow

        return grid_ && grid_->is_valid_position(ix, iy)
            ? &grid_->at(ix, iy)
            : nullptr;  
    }
}; //class grid_block

//==============================================================================
//! Iterator for block by block access.
//==============================================================================
template <typename T, bool Const>
struct block_iterator_base {
    typedef typename std::conditional<
        Const,
        grid2d<T> const,
        grid2d<T>
    >::type grid_type;
    
    block_iterator_base(grid_type* data = nullptr, ptrdiff_t  offset = 0)
        : data_(data), offset_(offset)
    {
    }

    grid_type*                   data_;
    ptrdiff_t                    offset_;
    mutable grid_block<T, Const> block_;
};

template <typename T, bool Const>
class block_iterator
    : public boost::iterator_facade<
        block_iterator<T, Const>,
        grid_block<T, Const>,
        boost::random_access_traversal_tag
      >
    , public block_iterator_base<T, Const>
{
public:
    block_iterator()
    {
    }

    block_iterator(grid_type* data, difference_type offset)
        : block_iterator_base(BK_CHECK_PTR(data), offset)
    {
    }

    template <typename U, bool C>
    block_iterator(block_iterator<U, C> const& other,
        typename std::enable_if<
            std::is_convertible<U*, T*>::value
        >::type* = nullptr
    )
        : block_iterator_base(other.data_, other.offset_)
    {
    }
private:
    friend class boost::iterator_core_access;
    template <typename, bool> friend class block_iterator;

    template <typename U, bool C>
    bool equal(block_iterator<U, C> const& other) const {
        return (data_ == other.data_) && (offset_ == other.offset_);
    }

    void increment() {
        ++offset_;
    }

    void decrement() {
        --offset_;
    }

    void advance(difference_type n) {
        offset_ += n;
    }

    reference dereference() const {
        BK_ASSERT(offset_ >= 0);
        return (block_ = grid_block<T, Const>(data_, static_cast<size_t>(offset_)));
    }

    template <typename U, bool C>
    difference_type distance_to(block_iterator<U, C> const& other) const {
        BK_ASSERT(data_ == other.data_);
        return other.offset_ - offset_;
    }
};

//==============================================================================
//! Adapter for block_iterator to work with STL algorithms.
//==============================================================================
template <typename T>
class block_iterator_adapter {
public:
    typedef typename T::value_type value_type;
    typedef block_iterator<value_type, std::is_const<T>::value> iterator;

    block_iterator_adapter(T& grid)
        : grid_(&grid)
    {
    }

    iterator begin() {
        return iterator(grid_, 0);
    }

    iterator end() {
        return iterator(grid_, static_cast<ptrdiff_t>(grid_->size()));
    }
private:
    T* grid_;
};

//==============================================================================
//! Helper function to create a block_iterator_adapter.
//==============================================================================
template <typename T>
auto make_block_iterator_adapter(T&& grid)
    -> block_iterator_adapter<typename std::remove_reference<T>::type>
{
    return block_iterator_adapter<typename std::remove_reference<T>::type>(grid);
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
