#pragma once

#include <Windows.h>

#include <d2d1.h>

#include <memory>
#include <functional>
#include <random>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <tuple>
#include <stack>

#include <boost/exception/all.hpp>

#include "bklib/assert.hpp"

template <typename T>
inline T clone(T const value) {
    return value;
}

namespace std {

template <typename T>
auto inline cbegin(T& container) -> decltype(container.cbegin())
{	// get beginning of sequence
    return container.cbegin();
}

template <typename T>
auto inline cbegin(T const& container) -> decltype(container.cbegin())
{	// get beginning of sequence
    return container.cbegin();
}

template <typename T>
auto inline cend(T& container) -> decltype(container.cend())
{	// get end of sequence
    return container.cend();
}

template <typename T>
auto inline cend(T const& container) -> decltype(container.cend())
{	// get end of sequence
    return container.cend();
}


} // std