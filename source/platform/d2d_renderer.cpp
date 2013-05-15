#include "pch.hpp"
#include "d2d_renderer.hpp"

#define THROW_ON_FAILURE(x) [&] { \
    HRESULT const hr = (x); \
    if (FAILED(hr)) { \
        BK_TODO; \
    } \
}()

bklib::d2d_renderer::d2d_renderer(HWND handle) {   
    THROW_ON_FAILURE(::CoInitialize(nullptr));

    factory_.reset([&] {
        ID2D1Factory* result = nullptr;
        THROW_ON_FAILURE(::D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &result
        ));

        return result;
    }());

    target_.reset([&] {
        RECT r;
        ::GetClientRect(handle, &r);

        ID2D1HwndRenderTarget* result = nullptr;
        THROW_ON_FAILURE(factory_->CreateHwndRenderTarget(
            ::D2D1::RenderTargetProperties(),
            ::D2D1::HwndRenderTargetProperties(
                handle, ::D2D1::SizeU(r.right - r.left, r.bottom - r.top)
            ),
            &result
        ));

        return result;
    }());

    {
        ID2D1SolidColorBrush* brush = nullptr;
        HRESULT const hr = target_->CreateSolidColorBrush(
            D2D1::ColorF(0.1f, 0.1f, 0.1f),
            D2D1::BrushProperties(),
            &brush
        );

        if (FAILED(hr)) {
            BK_TODO;
        }

        solid_brush_.reset(brush);
    }
}

void bklib::d2d_renderer::resize(unsigned w, unsigned h) {
    HRESULT const hr = target_->Resize(D2D1::SizeU(w, h));

    if (FAILED(hr)) {
        BK_TODO;
    }
}

void bklib::d2d_renderer::begin_draw() {
    target_->BeginDraw();

    target_->SetTransform(D2D1::Matrix3x2F::Identity());
    target_->Clear(D2D1::ColorF(D2D1::ColorF::White));
}

void bklib::d2d_renderer::end_draw() {
    HRESULT const hr = target_->EndDraw();
}

void bklib::d2d_renderer::fill_rect(int color, float left, float top, float right, float bottom) {
    switch (color) {
    case 0 :
        solid_brush_->SetColor(D2D1::ColorF(0)); break;
    case 1 :
        solid_brush_->SetColor(D2D1::ColorF(0xFF)); break;
    }
    
    target_->FillRectangle(D2D1::RectF(left, top, right, bottom), solid_brush_.get());
}

bklib::d2d_renderer::~d2d_renderer() {
    ::CoUninitialize();
}
