#include "pch.hpp"
#include "window.hpp"

namespace {

bklib::window* get_window_ptr(HWND handle) {
    ::SetLastError(0);

    auto const result = ::GetWindowLongPtrW(handle, GWLP_USERDATA);

    if (result == 0) {
        // 0 could signal an error
        auto const err = ::GetLastError();
        if (err) {
            BK_TODO;
        }
    }

    return reinterpret_cast<bklib::window*>(result);
}

void set_window_ptr(HWND handle, bklib::window* ptr) {
    ::SetLastError(0);
    
    auto const result = ::SetWindowLongPtrW(
        handle,
        GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>(ptr)
    );

    if (result == 0) {
        // 0 could signal an error
        auto const err = ::GetLastError();
        if (err != 0) {
            BK_TODO;
        }
    }
}

} //namespace

LRESULT CALLBACK bklib::window::global_wnd_proc_(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam
) throw() try {   
    // set the instance pointer for the window given by hwnd if it was
    // just created.
    if (msg == WM_NCCREATE) {
        auto const cs =
            reinterpret_cast<CREATESTRUCTW const*>(lParam);
        auto const window_ptr =
            reinterpret_cast<window*>(cs->lpCreateParams);

        set_window_ptr(hwnd, window_ptr);

        // disable legacy IME messages; we are TSF aware.
        auto const result = ::ImmDisableIME((DWORD)-1);
        if (result == FALSE) {
            BK_TODO;
        }
    }

    // the window object to forward the message to.
    auto const window = get_window_ptr(hwnd);

    if (window) {
        return window->wnd_proc_(hwnd, msg, wParam, lParam);
    } else {
        // it's possible we will receive some messages beofre WM_NCCREATE;
        // use the default handler.
        return ::DefWindowProcW(hwnd, msg, wParam, lParam);
    }
} catch (std::exception&) {
    BK_TODO;
} catch (...) {
    BK_TODO;
}

LRESULT bklib::window::wnd_proc_(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam
) {
    switch (msg) {
    case WM_CLOSE   : return handle_close_(hwnd, wParam, lParam);
    case WM_PAINT   : return handle_paint_(hwnd, wParam, lParam);
    case WM_SIZE    : return handle_size_(hwnd, wParam, lParam);
    case WM_KEYDOWN : return handle_keydown_(hwnd, wParam, lParam);
    }

    return ::DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT bklib::window::handle_close_(
    HWND hwnd, WPARAM wParam, LPARAM lParam
) {
    if (on_close_) {
        on_close_(*this);
    }    

    ::PostQuitMessage(0);
    return ::DefWindowProcW(hwnd, WM_CLOSE, wParam, lParam);
}

LRESULT bklib::window::handle_paint_(
    HWND hwnd, WPARAM wParam, LPARAM lParam
) {
    if (on_paint_) {
        on_paint_(*this);
        return 0;
    }

    return ::DefWindowProcW(hwnd, WM_PAINT, wParam, lParam);
}

LRESULT bklib::window::handle_keydown_(
    HWND hwnd, WPARAM wParam, LPARAM lParam
) {
    if (on_keydown_) {
        unsigned const repeat   = lParam & 0x00FF;
        unsigned const scan     = lParam & 0x0F00;
        unsigned const extended = lParam & 0x1000;
        unsigned const was_down = lParam & 0x4000;

        auto const code = static_cast<unsigned>(wParam);

        on_keydown_(*this, code, repeat, scan, was_down != 0);
        return 0;
    }

    return ::DefWindowProcW(hwnd, WM_PAINT, wParam, lParam);
}

LRESULT bklib::window::handle_size_(
    HWND hwnd, WPARAM wParam, LPARAM lParam
) {
    if (on_size_) {
        RECT rect;
        ::GetClientRect(handle_, &rect);

        auto const w = static_cast<unsigned>(std::abs(rect.right  - rect.left));
        auto const h = static_cast<unsigned>(std::abs(rect.bottom - rect.top));

        on_size_(*this, w, h);

        return 0;
    }

    return ::DefWindowProcW(hwnd, WM_PAINT, wParam, lParam);
}

HWND bklib::window::create_window_(window* wnd) {
    auto const instance = ::GetModuleHandleW(L"");

    static wchar_t const CLASS_NAME[] = L"test_wnd";
    static WNDCLASSEXW const wc = {
        sizeof(WNDCLASSEXW),
        CS_HREDRAW | CS_VREDRAW,
        global_wnd_proc_,
        0,
        0,
        instance,
        ::LoadIconW(nullptr, MAKEINTRESOURCEW(IDI_WINLOGO)),
        ::LoadCursorW(nullptr, MAKEINTRESOURCEW(IDC_ARROW)),
        nullptr,
        nullptr,
        CLASS_NAME,
        nullptr
    };

    ::RegisterClassExW(&wc); // ignore return value

    HWND const result = ::CreateWindowExW(
        0,
        CLASS_NAME,
        L"window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        (HWND)nullptr,
        (HMENU)nullptr,
        instance,
        wnd
    );

    if (result == 0) {
        BK_TODO;
    }

    ::ShowWindow(result, SW_SHOWDEFAULT);
    ::UpdateWindow(result);

    return result;
}

bklib::window::window()
    : is_closed_(false)
    , handle_(create_window_(this))
{
}

bool bklib::window::is_closed() const {
    return is_closed_;
}

HWND bklib::window::handle() const {
    return handle_;
}

void bklib::window::do_events(bool const wait) {
    ::MSG msg;

    while (wait || ::PeekMessageW(&msg, 0, 0, 0, PM_NOREMOVE)) {
        auto const result = ::GetMessageW(&msg, 0, 0, 0);
        
        switch (result) {
        case -1 : // error
            BK_TODO;
        case 0 : // WM_QUIT
            is_closed_ = true;
            return;
        }

        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }
}
