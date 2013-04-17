#pragma once

#include <boost/exception/all.hpp>

struct assertion_failure : virtual boost::exception, std::exception {};

#if defined(BK_TEST_BUILD)
#   define BK_ASSERT_IMPL BOOST_THROW_EXCEPTION(assertion_failure())
#elif !defined(NDEBUG)
#   define BK_ASSERT_IMPL __debugbreak()
#else
#   define BK_ASSERT_IMPL (void)0
#endif

#define BK_ASSERT(test)    \
[](bool const condition) { \
    if (!condition) {      \
        BK_ASSERT_IMPL;    \
    }                      \
}(test)
