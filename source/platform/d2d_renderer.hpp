#pragma once

#include <Windows.h>
#include <d2d1.h>

#include "bklib/geometry.hpp"

namespace bklib {

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

    void resize(unsigned w, unsigned h);

    void begin_draw();

    void end_draw();

    void fill_rect(int color, float left, float top, float right, float bottom);

    void draw_bitmap(bklib::rect<int> src, bklib::rect<int> dest);

    ~d2d_renderer();
private:
    bool is_drawing_;

    d2d_unique<ID2D1Factory>::type          factory_;
    d2d_unique<ID2D1HwndRenderTarget>::type target_;
    d2d_unique<ID2D1SolidColorBrush>::type  solid_brush_;
    d2d_unique<IWICImagingFactory>::type    image_factory_;
    d2d_unique<ID2D1Bitmap>::type           image_;
    
};

} //namespace bklib
