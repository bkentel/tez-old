#include "pch.hpp"
#include "d2d_renderer.hpp"

d2d_renderer::d2d_renderer(HWND handle) {
    // init com
    {
        HRESULT const hr = ::CoInitialize(nullptr);
        if (hr != S_OK && hr != S_FALSE) {
            BK_TODO;
        }
    }

    // create the factory
    {
        ID2D1Factory* factory = nullptr;
        HRESULT const hr = ::D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory
        );

        if (FAILED(hr)) {
            BK_TODO;
        }

        factory_.reset(factory);
    }
        
    // create a render target
    {
        RECT rect;
        ::GetClientRect(handle, &rect);

        ID2D1HwndRenderTarget* target = nullptr;			
        HRESULT const hr = factory_->CreateHwndRenderTarget(
            ::D2D1::RenderTargetProperties(),
            ::D2D1::HwndRenderTargetProperties(
                handle,
                ::D2D1::SizeU(
                    rect.right - rect.left,
                    rect.bottom - rect.top
                )
            ),
            &target
        );

        if (FAILED(hr)) {
            BK_TODO;
        }

        target_.reset(target);
    }

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

void d2d_renderer::resize(unsigned w, unsigned h) {
    HRESULT const hr = target_->Resize(D2D1::SizeU(w, h));

    if (FAILED(hr)) {
        BK_TODO;
    }
}

void d2d_renderer::begin_draw() {
    target_->BeginDraw();

    target_->SetTransform(D2D1::Matrix3x2F::Identity());
    target_->Clear(D2D1::ColorF(D2D1::ColorF::White));
}

void d2d_renderer::end_draw() {
    HRESULT const hr = target_->EndDraw();
}

void d2d_renderer::fill_rect(int color, float left, float top, float right, float bottom) {
    switch (color) {
    case 0 :
        solid_brush_->SetColor(D2D1::ColorF(0)); break;
    case 1 :
        solid_brush_->SetColor(D2D1::ColorF(0xFF)); break;
    }
    
    target_->FillRectangle(D2D1::RectF(left, top, right, bottom), solid_brush_.get());
}

d2d_renderer::~d2d_renderer() {
    ::CoUninitialize();
}
