#pragma once

#include <limits>
#include <functional>
#include <type_traits>

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

template <typename T>
T min(T const head) {
    return head;
}

template <typename T, typename... Ts>
T min(T const head, Ts... tail) {
    T const tail_min = min(tail...);
    return head <= tail_min ? head : tail_min;
}

//==============================================================================
//! Accumulates the minimum and maximum value pass to operator().
//! @tparam T a numeric type.
//==============================================================================
template <typename T>
struct min_max {
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

    typedef typename std::make_unsigned<T>::type distance_t;

    distance_t distance() const {
        return static_cast<distance_t>(max - min);
    }

    T min;
    T max;
};

//==============================================================================
//! Wraps a random number generator to abstract away the differences.
//! @tparam T a numeric type.
//==============================================================================
template <typename T>
class random_wrapper {
public:
    typedef T result_type;
     

    template <typename U>
    random_wrapper(U&& gen,
        typename std::enable_if<
            std::is_convertible<typename std::result_of<U()>::type, T>::value &&
            !std::is_same<
                typename std::remove_reference<U>::type,
                random_wrapper
            >::value
        >::type* = nullptr
    )
        : gen_([&] { return gen(); })
    {
    }

    T operator()() {
        return gen_();
    }

    static T min() { return std::numeric_limits<T>::min(); }
    static T max() { return std::numeric_limits<T>::max(); }
private:
    std::function<T ()> gen_;
};

template <typename T>
inline auto make_random_wrapper(T& random)
    -> random_wrapper<typename std::result_of<T()>::type>
{
    return random_wrapper<typename T::result_type>(random);
}
