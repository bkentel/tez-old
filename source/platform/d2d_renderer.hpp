#pragma once

#include <Windows.h>
#include <d2d1.h>

#include "bklib/geometry.hpp"

namespace bklib {
#define THROW_ON_FAILURE(x) [&] { \
    HRESULT const hr = (x); \
    if (FAILED(hr)) { \
        ::std::cout << "error = " << hr; \
        BK_TODO; \
    } \
}()

struct com_library {
    com_library() {
        THROW_ON_FAILURE(::CoInitializeEx(
            nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
        ));
    }

    ~com_library() {
        ::CoUninitialize();
    }
};

template <typename T>
struct d2d_deleter {
    void operator()(T* ptr) const {
        if (ptr) {
            ptr->Release();
        }
    }
};

template <typename T>
struct d2d_unique {
    typedef std::unique_ptr<T, d2d_deleter<T>> type;
};

class d2d_renderer {
public:
    d2d_renderer(HWND handle);

    void zoom_in();
    void zoom_out();
    void translate(int dx, int dy);

    void resize(unsigned w, unsigned h);

    void begin_draw();

    void end_draw();

    void fill_rect(int color, float left, float top, float right, float bottom);

    void draw_bitmap(unsigned index, point2d<unsigned> dest);

    ~d2d_renderer();
private:
    com_library com_;

    float zoom_factor_;
    float dx_, dy_;
    
    bool is_drawing_;

    d2d_unique<ID2D1Factory>::type          factory_;
    d2d_unique<ID2D1HwndRenderTarget>::type target_;
    d2d_unique<ID2D1SolidColorBrush>::type  solid_brush_;
    d2d_unique<IWICImagingFactory>::type    image_factory_;
    d2d_unique<ID2D1Bitmap>::type           image_;
    
};

} //namespace bklib
