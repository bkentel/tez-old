#pragma once

#pragma warning(push, 3)

#include <Windows.h>

#include <d2d1.h>
#include <wincodec.h>

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
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/iterator/iterator_facade.hpp>

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

#pragma warning(pop)
