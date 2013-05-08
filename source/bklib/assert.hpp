#pragma once

#include "config.hpp"
#include "scope_exit.hpp"

#if defined(BK_TEST_BUILD)
#   include <boost/exception/all.hpp>

    struct assertion_failure : virtual boost::exception, std::exception {};
    extern bool BK_TEST_BREAK_ON_ASSERT;

#   define BK_TEST_DEBUG_BREAK \
    do { \
        if (BK_TEST_BREAK_ON_ASSERT) __debugbreak(); \
    } while (false)

#   define BK_ASSERT_IMPL BOOST_THROW_EXCEPTION(assertion_failure())
#else
#   define BK_TEST_DEBUG_BREAK ((void)0)
#   define BK_ASSERT_IMPL BK_UNREACHABLE
#endif

#define BK_ASSERT(c) \
    do { \
        if (!(c)) { \
            BK_TEST_DEBUG_BREAK; \
            BK_ASSERT_IMPL; \
        } \
    } while (false)
    
#define BK_ASSERT_VALUE(x, cond) \
    [&] {                        \
        BK_ASSERT(cond);         \
        return (x);              \
    }()

#define BK_CHECK_PTR(p) BK_ASSERT_VALUE(p, (p) != nullptr)

#define BK_TEST_FAILURES \
    BK_TEST_BREAK_ON_ASSERT = false; \
    BK_ON_SCOPE_EXIT({ \
        BK_TEST_BREAK_ON_ASSERT = true; \
    });