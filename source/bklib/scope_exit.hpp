////////////////////////////////////////////////////////////////////////////////
//! @file
//! @brief  Implementation of on_scope_exit.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "config.hpp"
#include <type_traits>

namespace bklib {

//==============================================================================
//! Implementation of RAII "on scope exit".
//==============================================================================
template <typename F>
class on_scope_exit {
public:
    on_scope_exit(on_scope_exit&& other)
        : active_(other.active_), f_(std::move(other.f_))
    {
        other.cancel();
    }

    on_scope_exit(F function, bool active = true)
        : active_(active), f_(std::move(function))
    {
    }

    ~on_scope_exit() {
        if (active_) f_();
    }

    //--------------------------------------------------------------------------
    //! Don't execute the action on scope exit
    //--------------------------------------------------------------------------
    void cancel() {
        active_ = false;
    }
private:
    on_scope_exit() BK_DELETE;
    on_scope_exit(on_scope_exit const&) BK_DELETE;
    on_scope_exit& operator=(on_scope_exit const&) BK_DELETE;

    bool active_;
    F    f_;
};

//==============================================================================
//! stl style make function.
//==============================================================================
template <typename F>
on_scope_exit<F> make_on_scope_exit(F function, bool active = true) {
    return on_scope_exit<F>(std::move(function), active);
}

//==============================================================================
//! Allow a declaration of the form BK_ON_SCOPE_EXIT({ ...code... });
//==============================================================================
#define BK_ON_SCOPE_EXIT(function)                                             \
auto BK_UNIQUE_ID = ::bklib::make_on_scope_exit(                               \
    [&]() -> void function                                                     \
)

} //namespace bklib
