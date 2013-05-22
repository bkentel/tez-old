#include "pch.hpp"
#include "d2d_renderer.hpp"



bklib::d2d_renderer::d2d_renderer(HWND handle)
    : zoom_factor_(1.0f)
    , dx_(0.0f)
    , dy_(0.0f)
{   
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

    target_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    //auto const mode = target_->GetAntialiasMode();
}

void bklib::d2d_renderer::resize(unsigned w, unsigned h) {
    HRESULT const hr = target_->Resize(D2D1::SizeU(w, h));

    if (FAILED(hr)) {
        BK_TODO;
    }
}

void bklib::d2d_renderer::begin_draw() {
    target_->BeginDraw();  

    target_->SetTransform(
        D2D1::Matrix3x2F::Scale(zoom_factor_, zoom_factor_) *
        D2D1::Matrix3x2F::Translation(dx_, dy_)
    );
    target_->Clear(D2D1::ColorF(D2D1::ColorF::Black));
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

void bklib::d2d_renderer::draw_bitmap(unsigned index, point2d<unsigned> dest) {
    static unsigned const tex_dim  = 256;
    static unsigned const tile_dim = 16;
    static unsigned const tile_w   = tex_dim / tile_dim;

    unsigned const xi = index % tile_w;
    unsigned const yi = index / tile_w;

    unsigned const src_l = xi * tile_dim;
    unsigned const src_t = yi * tile_dim;
    unsigned const src_r = src_l + tile_dim;
    unsigned const src_b = src_t + tile_dim;

    unsigned const dst_l = dest.x * tile_dim;
    unsigned const dst_t = dest.y * tile_dim;
    unsigned const dst_r = dst_l + tile_dim;
    unsigned const dst_b = dst_t + tile_dim;

    target_->DrawBitmap(
        image_.get(),
        D2D1::RectF(dst_l, dst_t, dst_r, dst_b),
        1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        D2D1::RectF(src_l, src_t, src_r, src_b)
    );
}

bklib::d2d_renderer::~d2d_renderer() {
}

void bklib::d2d_renderer::zoom_in() {
    zoom_factor_ += 0.1f;
    if (zoom_factor_ > 10.0f) zoom_factor_ = 10.0f;
}

void bklib::d2d_renderer::zoom_out() {
    zoom_factor_ -= 0.1f;
    if (zoom_factor_ < 0.1f) zoom_factor_ = 0.1f;
}

void bklib::d2d_renderer::translate(int dx, int dy) {
    dx_ += dx;
    dy_ += dy;
}
