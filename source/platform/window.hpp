#pragma once

#include <Windows.h>
#include <functional>

namespace bklib {

#define BK_DEFINE_EVENT(NAME, SIG, VAR)                     \
    template <> struct listener_t<event_type::##NAME> {     \
        typedef std::function<SIG> type;                    \
    };                                                      \
                                                            \
    template <>                                             \
    void listen<event_type::##NAME>(                        \
        listener_t<event_type::##NAME>::type callback       \
    ) {                                                     \
        VAR = callback;                                     \
    }

class window {
public:
    window();

    void do_events(bool wait = true);

    bool is_closed() const;

    HWND handle() const;
    ////////////////////////////////////////////////////////////////////////////
    // Events
    ////////////////////////////////////////////////////////////////////////////

    enum class event_type {
        on_create,
        on_paint,
        on_close,
        on_size,
        on_keydown,
    };
    
    template <event_type E>
    struct listener_t;

    template <event_type E>
    void listen(typename listener_t<E>::type callback);

    BK_DEFINE_EVENT(on_create,  void (window&), on_create_);
    BK_DEFINE_EVENT(on_paint,   void (window&), on_paint_);
    BK_DEFINE_EVENT(on_close,   void (window&), on_close_);
    BK_DEFINE_EVENT(on_size,    void (window&, unsigned w, unsigned h), on_size_);
    BK_DEFINE_EVENT(on_keydown, void (window&, unsigned code, unsigned repeat, unsigned scan, bool was_down), on_keydown_);
private:
    
    static HWND create_window_(window* wnd);
    static LRESULT CALLBACK global_wnd_proc_(
        HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam
    );

    LRESULT wnd_proc_(
        HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam
    );

    LRESULT handle_close_(HWND hwnd, WPARAM wParam, LPARAM lParam);
    listener_t<event_type::on_close>::type on_close_;

    LRESULT handle_paint_(HWND hwnd, WPARAM wParam, LPARAM lParam);
    listener_t<event_type::on_paint>::type on_paint_;
    
    LRESULT handle_create_(HWND hwnd, WPARAM wParam, LPARAM lParam);
    listener_t<event_type::on_create>::type on_create_;

    LRESULT handle_size_(HWND hwnd, WPARAM wParam, LPARAM lParam);
    listener_t<event_type::on_size>::type on_size_;

    LRESULT handle_keydown_(HWND hwnd, WPARAM wParam, LPARAM lParam);
    listener_t<event_type::on_keydown>::type on_keydown_;

    bool is_closed_;
    HWND handle_;
};

#undef BK_DEFINE_EVENT

} //namespace bklib
