#pragma once

#include <vector>
#include <memory>
#include <type_traits>

template <size_t A, size_t B>
struct max_of {
    static size_t const value = A >= B ? A : B;
};

class quad_tree_node_base {
public:
    static unsigned const SPLIT_COUNT = 10;
    static unsigned const MAX_DEPTH   = 5;
    static unsigned const MIN_DIM     = 2;

    quad_tree_node_base(unsigned x, unsigned y, unsigned w, unsigned h)
        : is_leaf_(true)
        , x_(x), y_(y), w_(w), h_(h)
    {
    }

    bool contains(unsigned x, unsigned y) const {
        return (x >= x_) && (x < x_ + w_) &&
               (y >= y_) && (y < y_ + h_); 
    }

    bool is_leaf() const {
        return is_leaf_;
    }
protected:
    unsigned get_index_from_loc_(unsigned x, unsigned y) const {
        BK_ASSERT(contains(x, y));

        auto const xi = (x - x_) > w_ / 2 ? 1 : 0;
        auto const yi = (y - y_) > h_ / 2 ? 1 : 0;
        auto const i  = yi * 2 + xi;

        BK_ASSERT(i >= 0 && i <= 4);

        return i;
    }

    bool      is_leaf_;
    unsigned  x_, y_, w_, h_;
};

template <typename T>
class quad_tree_node : public quad_tree_node_base {
public:
    typedef std::unique_ptr<quad_tree_node> child_t;
    typedef child_t                         children_t[4];    
    typedef std::vector<T>                  objects_t;

    typedef typename std::aligned_union<
        max_of<
            sizeof(child_t)*4,
            sizeof(objects_t)
        >::value,
        children_t,
        objects_t
    >::type storage_t;

    quad_tree_node(unsigned x, unsigned y, unsigned w, unsigned h)
        : quad_tree_node_base(x, y, w, h)
    {
        new (&storage_) objects_t();
    }

    ~quad_tree_node() {
        if (is_leaf_) {
            objects_().~objects_t();
        } else {
            children_()[0].~child_t();
            children_()[1].~child_t();
            children_()[2].~child_t();
            children_()[3].~child_t();
        }
    }

    void insert(T object, unsigned depth = 0) {
        auto const xx = x(object);
        auto const yy = y(object);

        BK_ASSERT(contains(xx, yy));

        if (!is_leaf_) {
            get_child_from_loc_(xx, yy)->insert(object, depth + 1);
            return;
        }

        objects_().push_back(object);

        if (objects_().size() >= SPLIT_COUNT &&
            depth < MAX_DEPTH &&
            w_ / 2 > MIN_DIM &&
            h_ / 2 > MIN_DIM
        ) {
            split_();
        }
    }

    bool contains(T const& object) const {
        auto const xx = x(object);
        auto const yy = y(object);
        
        return contains(xx, yy);
    }

    using quad_tree_node_base::contains;

    objects_t const* find_near(unsigned x, unsigned y) const {
        if (!contains(x, y)) {
            return nullptr;
        }
        
        if (is_leaf_) {
            return &objects_();
        } else {
            return get_child_from_loc_(x, y)->find_near(x, y);
        }
    }
private:
    void split_() {
        BK_ASSERT(w_ / 2 > MIN_DIM && h_ / 2 > MIN_DIM);
        BK_ASSERT(is_leaf_);

        auto& obj = objects_();
        auto temp = std::move(obj);
        obj.~objects_t();

        is_leaf_ = false;

        auto& children = children_();

        auto const w0 =  w_ / 2;
        auto const h0 =  h_ / 2;

        new (&children[0]) child_t(new quad_tree_node(x_,      y_,      w0, h0));
        new (&children[1]) child_t(new quad_tree_node(x_ + w0, y_,      w0, h0));
        new (&children[2]) child_t(new quad_tree_node(x_,      y_ + h0, w0, h0));
        new (&children[3]) child_t(new quad_tree_node(x_ + w0, y_ + h0, w0, h0));

        for (auto& o : temp) {
            get_child_from_loc_(x(o), y(o))->insert(std::move(o));
        }
    }

    child_t& get_child_from_loc_(unsigned x, unsigned y) {
        return children_()[get_index_from_loc_(x, y)];
    }

    child_t const& get_child_from_loc_(unsigned x, unsigned y) const {
        return children_()[get_index_from_loc_(x, y)];
    }

    children_t& children_() {
        BK_ASSERT(!is_leaf_);
        return *reinterpret_cast<children_t*>(&storage_);
    }

    children_t const& children_() const {
        BK_ASSERT(!is_leaf_);
        return *reinterpret_cast<children_t const*>(&storage_);
    }

    objects_t& objects_() {
        BK_ASSERT(is_leaf_);
        return *reinterpret_cast<objects_t*>(&storage_);
    }

    objects_t const& objects_() const {
        BK_ASSERT(is_leaf_);
        return *reinterpret_cast<objects_t const*>(&storage_);
    }

    storage_t storage_;
};
