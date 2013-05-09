#pragma once

#include <limits>
#include <functional>
#include <type_traits>

namespace bklib {

//==============================================================================
//! Add cv qualifiers conditionally.
//==============================================================================
template <
    typename T,
    bool IsIntegral = std::is_integral<T>::value
>
struct make_distance_type {
    typedef typename std::make_unsigned<T>::type type;
};

template <typename T>
struct make_distance_type<T, false> {
    typedef typename std::enable_if<
        std::is_floating_point<T>::value, T
    >::type type;
};

//==============================================================================
//! Add cv qualifiers conditionally.
//==============================================================================
template <typename T, bool Const = false, bool Volatile = false>
struct make_cv_if {
    typedef T type;
};

template <typename T>
struct make_cv_if<T, true, false> {
    typedef typename std::add_const<T>::type type;
};

template <typename T>
struct make_cv_if<T, false, true> {
    typedef typename std::add_volatile<T>::type type;
};

template <typename T>
struct make_cv_if<T, true, true> {
    typedef typename std::add_cv<T>::type type;
};
//==============================================================================
//! Find the min of a variable number of arguments.
//==============================================================================
template <typename T>
inline T min(T const head) {
    return head;
}

template <typename T, typename... Ts>
inline T min(T const head, Ts const... tail) {
    T const tail_min = min(tail...);
    return head <= tail_min ? head : tail_min;
}
//==============================================================================
//! Find the max of a variable number of arguments.
//==============================================================================
template <typename T>
inline T max(T const head) {
    return head;
}

template <typename T, typename... Ts>
inline T max(T const head, Ts const... tail) {
    T const tail_max = max(tail...);
    return head >= tail_max ? head : tail_max;
}

//==============================================================================
//! Accumulates the minimum and maximum value pass to operator().
//! @tparam T a numeric type.
//==============================================================================
template <typename T = signed>
struct min_max {
    static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");

    typedef typename make_distance_type<T>::type distance_t;

    min_max()
        : min(std::numeric_limits<T>::max())
        , max(std::numeric_limits<T>::lowest())
    {
    }

    explicit min_max(T value)
        : min(value)
        , max(value)
    {
    }

    void operator()(T value) {
        min = min > value ? value : min;
        max = max < value ? value : max;
    }

    distance_t distance() const {
        return static_cast<distance_t>(max - min);
    }

    T min, max;
};

//==============================================================================
//! Wraps a random number generator to abstract away the differences.
//! @tparam T a numeric type.
//==============================================================================
template <typename T>
class random_wrapper {
public:
    typedef T result_type;

    //!TODO cleanup this condition
    template <typename U>
    random_wrapper(U&& gen,
        typename std::enable_if<
            std::is_convertible<
                typename std::result_of<U()>::type,
                result_type
            >::value &&
            !std::is_same<
                typename std::remove_reference<U>::type,
                random_wrapper
            >::value
        >::type* = nullptr
    )
        : gen_([&] { return gen(); })
    {
    }

    result_type operator()() {
        return gen_();
    }

    static result_type min() { return std::numeric_limits<T>::min(); }
    static result_type max() { return std::numeric_limits<T>::max(); }
private:
    std::function<result_type ()> gen_;
};

template <typename T>
inline auto make_random_wrapper(T& random)
    -> random_wrapper<typename std::result_of<T()>::type>
{
    return random_wrapper<typename T::result_type>(random);
}

template <typename T, typename U>
class discriminated_union {
public:
    typedef T a_type;
    typedef U b_type;

    template <typename V, typename... Types>
    static discriminated_union make(Types&&... params) {
        static_assert(
            std::is_same<a_type, V>::value || std::is_same<b_type, V>::value,
            "Not a union type"
        );
        return discriminated_union(
            static_cast<V*>(nullptr),
            std::forward<Types>(params)...
        );
    }

    ~discriminated_union() {
        if (is_a_type_) {
            get_a_().~a_type();
        } else {
            get_b_().~b_type();
        }
    }

    void swap(discriminated_union& other) {
        using std::swap;

        if (is_a_type_ ^ other.is_a_type_) {
            auto& a =  is_a_type_ ? *this : other;
            auto& b = !is_a_type_ ? *this : other;

            swap(a.is_a_type_, b.is_a_type_);
            
            a_type temp = std::move(a.get_a_());
            
            a.get_a_().~a_type();
            new (std::addressof(a.get_b_())) b_type(std::move(b.get_b_()));
            
            b.get_b_().~b_type();
            new (std::addressof(b.get_a_())) a_type(std::move(temp));
        } else if (is_a_type_) {
            a_type& a = get_a_();
            a_type& b = other.get_a_();

            swap(a, b);
        } else {
            b_type& a = get_b_();
            b_type& b = other.get_b_();

            swap(a, b);
        }
    }

    template <typename V>
    bool is_type() const;

    template <>
    bool is_type<a_type>() const { return is_a_type_; }

    template <>
    bool is_type<b_type>() const { return !is_a_type_; }

    explicit operator a_type&() { return get_a_(); }
    explicit operator b_type&() { return get_b_(); }
private:
    template <typename... Types>
    discriminated_union(a_type*, Types&&... params)
        : is_a_type_(true)
    {
        new (std::addressof(get_a_())) a_type(std::forward<Types>(params)...);
    }

    template <typename... Types>
    discriminated_union(b_type*, Types&&... params)
        : is_a_type_(false)
    {
        new (std::addressof(get_b_())) b_type(std::forward<Types>(params)...);
    }

    a_type& get_a_() {
        return reinterpret_cast<a_type&>(storage_);
    }

    a_type const& get_a_() const {
        return const_cast<discriminated_union*>(this)->get_a_();
    }

    b_type& get_b_() {
        return reinterpret_cast<b_type&>(storage_);
    }

    b_type const& get_b_() const {
        return const_cast<discriminated_union*>(this)->get_b_();
    }

    typename std::aligned_union<1, a_type, b_type>::type storage_;
    bool is_a_type_;
};

} //namespace bklib
