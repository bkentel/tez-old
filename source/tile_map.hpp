#pragma once

#include "types.hpp"
#include "assert.hpp"
#include "grid2d.hpp"

#include "room.hpp" //tile_category

#include <memory>

struct tile_info {
};

class tile_data {
public:
    tile_data()
        : texture_id(0)
        , type(tile_category::empty)
        , info(nullptr)
    {
    }

    tile_data(tile_data&& other)
        : texture_id(other.texture_id)
        , type(other.type)
        , info(std::move(other.info))
    {
    }

    tile_data& operator=(tile_data&& rhs) {
        swap(rhs);
        return *this;
    }

    void swap(tile_data& other) {
        using std::swap;

        swap(texture_id, other.texture_id);
        swap(type, other.type);
        swap(info, other.info);
    }

    uint32_t                   texture_id;
    tile_category              type;
    std::unique_ptr<tile_info> info;
private:
    tile_data(tile_data const&)            BK_DELETE;
    tile_data& operator=(tile_data const&) BK_DELETE;
};

inline void swap(tile_data& a, tile_data& b) {
    a.swap(b);
}

//==============================================================================
// A 2D grid of tiles.
//==============================================================================
class tile_grid {
public:
    tile_grid(unsigned width, unsigned height);

    void write(room const& r);

    unsigned width() const {
        return data_.width();
    }

    unsigned height() const {
        return data_.height();
    }

    void print() {
        std::cout << "===================================" << std::endl;

        for (unsigned y = 0; y < height(); ++y) {
            for (unsigned x = 0; x < width(); ++x) {
                std::cout << static_cast<char>(data_.at(x, y).type);
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
private:   
    grid2d<tile_data> data_;
};

//////////////////////////////////////////////////////////////////////////////////
////! Random access iterator over a 2D sub-region of a tile_map.
////! @tparam Is_const
////!     true when the data iterated over is const; false otherwise.
//////////////////////////////////////////////////////////////////////////////////
//template <bool Is_const>
//class tile_iterator
//    : public std::iterator<
//        std::random_access_iterator_tag,
//        typename std::conditional<Is_const, const tile_data, tile_data>::type
//    >
//{
//    template <bool B> friend class tile_iterator;
//    template <bool B> friend class tile_region;
//
//    template <bool B1, bool B2>
//    friend bool operator==(tile_iterator<B1> const& a, tile_iterator<B2> const& b);
//
//    template <bool B1, bool B2>
//    friend bool operator!=(tile_iterator<B1> const& a, tile_iterator<B2> const& b);
//public:
//    template <bool B>
//    tile_iterator(tile_iterator<B> const& rhs)
//        : stride_(rhs.stride_)
//        , width_(rhs.width_)
//        , height_(rhs.height_)
//        , offset_(rhs.offset_)
//        , start_(rhs.start_)
//    {
//    }
//   
//    template <bool B>
//    tile_iterator& operator=(tile_iterator<B> const& rhs)
//    {
//        stride_ = rhs.stride_;
//        width_  = rhs.width_;
//        height_ = rhs.height_;
//        offset_ = rhs.offset_;
//        start_  = rhs.start_;
//    
//        return *this;   
//    }
//
//    tile_iterator& operator++() {
//        return *this += 1;
//    }
//
//    tile_iterator& operator++(int) {
//        return ++(*this);
//    }
//
//    tile_iterator& operator--() {
//        return *this -= 1;
//    }
//
//    tile_iterator& operator--(int) {
//        return --(*this);
//    }
//
//    tile_iterator operator+(difference_type n) const {
//        return tile_iterator(*this) += n;
//    }
//
//    tile_iterator operator-(difference_type n) const {
//        return tile_iterator(*this) -= n;
//    }
//
//    template <bool B>
//    difference_type operator-(tile_iterator<B> const& rhs) const {
//        BK_ASSERT(start_ == rhs.start_);
//        return offset_ - rhs.offset_;
//    }
//
//    tile_iterator& operator+=(difference_type n) {
//        BK_ASSERT(n >= 0);
//        BK_ASSERT(offset_ + n <= size_());
//
//        offset_ += n;
//        return *this;
//    }
//
//    tile_iterator& operator-=(difference_type n) {
//        BK_ASSERT(n >= 0);
//        BK_ASSERT(offset_ >= n);
//    
//        offset_ -= n;
//        return *this;
//    }
//
//    reference operator*() const {
//        BK_ASSERT(offset_ < size_());
//        return *at_(offset_);
//    }
//
//    pointer operator->() const {
//        BK_ASSERT(offset_ < size_());
//        return at_(offset_);
//    }
//
//    reference operator[](difference_type n) const {
//        BK_ASSERT(offset_ + n < size_());
//        return *at_(offset_ + n);
//    }
//private:
//    tile_iterator(pointer start, size_t w, size_t h, size_t stride, size_t offset = 0)
//        : stride_(stride)
//        , width_(w)
//        , height_(h)
//        , offset_(offset)
//        , start_(start)
//    {
//        BK_ASSERT(start != nullptr);
//        BK_ASSERT(w > 0);
//        BK_ASSERT(h > 0);
//    }
//
//    tile_iterator(tile_iterator const& other, size_t offset)
//        : stride_(other.stride_)
//        , width_(other.width_)
//        , height_(other.height_)
//        , offset_(offset)        
//        , start_(other.start_)
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
//        size_t const xi = offset % width_;
//        size_t const yi = offset / width_;
//
//        return start_ + yi * stride_ + xi;
//    }
//
//    size_t  stride_;
//    size_t  width_;
//    size_t  height_;
//    size_t  offset_;
//    pointer start_;
//};
//
//template <bool B1, bool B2>
//bool operator==(tile_iterator<B1> const& a, tile_iterator<B2> const& b) {
//    return (a.start_  == b.start_)  &&
//           (a.offset_ == b.offset_) &&
//           (a.width_  == b.width_)  &&
//           (a.stride_ == b.stride_);
//}
//
//template <bool B1, bool B2>
//bool operator!=(tile_iterator<B1> const& a, tile_iterator<B2> const& b) {
//    return !(a == b);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////
//// A 2D sub-region of a tile_map.
//////////////////////////////////////////////////////////////////////////////////
//template <bool Is_const>
//class tile_region {
//    friend tile_grid;
//
//    template <bool B> friend class tile_region;
//    
//    template <bool B1, bool B2>
//    friend bool operator==(tile_region<B1> const&, tile_region<B2> const&);
//    
//    template <bool B1, bool B2>
//    friend bool operator!=(tile_region<B1> const&, tile_region<B2> const&);
//public:
//    typedef typename std::conditional<Is_const, tile_data const, tile_data>::type value_type;
//    typedef value_type* pointer;
//    typedef value_type& reference;
//    
//    typedef tile_iterator<true>              const_iterator;
//    typedef tile_iterator<false || Is_const> iterator;
//
//    template <bool B, typename std::enable_if<!(!Is_const && B)>::type* = nullptr>
//    tile_region(tile_region<B> const& other)
//        : width_(other.width_)
//        , height_(other.height_)
//        , stride_(other.stride_)
//        , where_(other.where_)    
//    {
//    }
//
//    template <bool B, typename std::enable_if<!(!Is_const && B)>::type* = nullptr>
//    tile_region& operator=(tile_region<B> const& rhs) {
//        width_  = rhs.width_;
//        height_ = rhs.height_;
//        stride_ = rhs.stride_;
//        where_  = rhs.where_;    
//
//        return *this;
//    }
//
//    iterator begin() const {
//        return iterator(where_, width_, height_, stride_, 0);
//    }
//
//    iterator end() const {
//        return iterator(where_, width_, height_, stride_, width_ * height_);
//    }
//
//    const_iterator cbegin() const {
//        return begin();
//    }
//
//    const_iterator cend() const {
//        return end();
//    }
//
//    reference operator()(size_t x, size_t y) const {
//        BK_ASSERT(x < width_);
//        BK_ASSERT(y < height_);
//
//        auto const i = y * stride_ + x;
//
//        return *(where_ + i);
//    }
//private:
//    tile_region(pointer where, size_t w, size_t h, size_t stride)
//        : width_(w)
//        , height_(h)
//        , stride_(stride)
//        , where_(where)
//    {
//        BK_ASSERT(where != nullptr);
//        BK_ASSERT(w > 0);
//        BK_ASSERT(h > 0);
//    }
//
//    size_t  width_;
//    size_t  height_;
//    size_t  stride_;
//    pointer where_;
//};
//
//template <bool B1, bool B2>
//bool operator==(tile_region<B1> const& a, tile_region<B2> const& b) {
//    return (a.width_  == b.width_)  &&
//           (a.height_ == b.height_) &&
//           (a.stride_ == b.stride_) &&
//           (a.where_  == b.where_);
//}
//
//template <bool B1, bool B2>
//bool operator!=(tile_region<B1> const& a, tile_region<B2> const& b) {
//    return !(a == b);
//}