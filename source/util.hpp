#pragma once

#include <limits>
#include <functional>

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
template <typename T = int>
struct random_wrapper {
    typedef T result_type;
     
    template <typename U>
    random_wrapper(U& gen)
        : gen_([&] { return gen(); })
    {
        static_assert(
            std::is_same<result_type, U::result_type>::value,
            "type mismatch"
        );
    }

    T operator()() {
        return gen_();
    }

    static T min() { return std::numeric_limits<T>::min(); }
    static T max() { return std::numeric_limits<T>::max(); }

    std::function<T ()> gen_;
};
