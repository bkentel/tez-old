#include "pch.hpp"
#include "d2d_renderer.hpp"

#define THROW_ON_FAILURE(x) [&] { \
    HRESULT const hr = (x); \
    if (FAILED(hr)) { \
        ::std::cout << "error = " << hr; \
        BK_TODO; \
    } \
}()

bklib::d2d_renderer::d2d_renderer(HWND handle) {   
    THROW_ON_FAILURE(::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));

    image_factory_.reset([&] {
        IWICImagingFactory* result = nullptr;
        THROW_ON_FAILURE(::CoCreateInstance(
            CLSID_WICImagingFactory1, nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&result)
        ));

        return result;
    }());

    factory_.reset([&] {
        ID2D1Factory* result = nullptr;

        D2D1_FACTORY_OPTIONS options = {
            D2D1_DEBUG_LEVEL_INFORMATION
        };

        THROW_ON_FAILURE(::D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &result
        ));

        return result;
    }());

    target_.reset([&] {
        RECT r;
        ::GetClientRect(handle, &r);

        auto props = ::D2D1::RenderTargetProperties();
        props.pixelFormat = ::D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED
        );

        ID2D1HwndRenderTarget* result = nullptr;
        THROW_ON_FAILURE(factory_->CreateHwndRenderTarget(
            props,
            ::D2D1::HwndRenderTargetProperties(
                handle, ::D2D1::SizeU(r.right - r.left, r.bottom - r.top)
            ),
            &result
        ));

        return result;
    }());


    d2d_unique<IWICBitmapDecoder>::type decoder([&] {
        IWICBitmapDecoder* result = nullptr;

        THROW_ON_FAILURE(image_factory_->CreateDecoderFromFilename(
            L"data/dungeon.bmp",
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            &result
        ));

        return result;
    }());

    d2d_unique<IWICBitmapFrameDecode>::type source([&] {
        IWICBitmapFrameDecode* result = nullptr;
        THROW_ON_FAILURE(decoder->GetFrame(0, &result));
        return result;
    }());

    d2d_unique<IWICFormatConverter>::type converter([&] {
        IWICFormatConverter* result = nullptr;
        THROW_ON_FAILURE(image_factory_->CreateFormatConverter(&result));
        return result;
    }());

    THROW_ON_FAILURE(converter->Initialize(
        source.get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0f,
        WICBitmapPaletteTypeCustom
    ));

    image_.reset([&] {
        ID2D1Bitmap* result = nullptr;
        THROW_ON_FAILURE(target_->CreateBitmapFromWicBitmap(
            converter.get(),
            nullptr,
            &result
        ));
        return result;
    }());
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

void bklib::d2d_renderer::draw_bitmap(bklib::rect<int> src, bklib::rect<int> dest) {
    target_->DrawBitmap(
        image_.get(),
        D2D1::RectF(0, 0, 255, 255),
        1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        D2D1::RectF(16, 0, 31, 15)
    );
}

bklib::d2d_renderer::~d2d_renderer() {
    ::CoUninitialize();
}
