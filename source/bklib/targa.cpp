#include "pch.hpp"
#include "targa.hpp"

#include <fstream>

namespace {

#pragma pack(push, tga, 1) //Enforce tight packing

enum class color_map_type : uint8_t {
    absent  = 0,
    present = 1,
};

enum class image_type : uint8_t {
    none             = 0,
    color_mapped     = 1,
    true_color       = 2,
    black_white      = 3,
    rle_color_mapped = 9,
    rle_true_color   = 10,
    rle_black_white  = 11,
};
   
struct color_map_spec {
    uint16_t first_entry_index;
    uint16_t length;
    uint8_t  entry_size;
};
    
struct image_descriptor {
    uint8_t alpha    : 4;
    uint8_t right    : 1;
    uint8_t top      : 1;
    uint8_t reserved : 2;
};

static_assert(sizeof(image_descriptor) == 1, "bad size");

struct image_spec {
    uint16_t         x_origin;
    uint16_t         y_origin;
    uint16_t         width;
    uint16_t         height;
    uint8_t          depth;
    image_descriptor descriptor;
};

struct header {
    uint8_t        id_length;
    color_map_type color_map_type;
    image_type     image_type;
    color_map_spec color_map_spec;
    image_spec     image_spec;
};

struct footer {
    uint32_t ext_area_offset;
    uint32_t dev_dir_offset;
    char     signature[16];
    char     dot_terminator;
    char     null_terminator;
};

static_assert(sizeof(header) == 18, "bad size");  // sanity check.
static_assert(sizeof(footer) == 26, "bad size");  // sanity check.

#pragma pack(pop, tga) //Enforce tight packing

} // namespace

using bklib::image_targa;

class image_targa::impl_t {
public:
    impl_t(
        std::string const& //filename
    ) {
    }

    impl_t(uint16_t w, uint16_t h, image_targa::image_type type)
        : width_(w)
        , height_(h)
        , depth_(0)
        , data_size_(0)
        , header_(nullptr)
        , footer_(nullptr)
        , image_data_(nullptr)
    {
        ::image_type const targa_type =
            type == image_targa::image_type::r8 ?
            ::image_type::black_white :
            ::image_type::true_color;
        
        uint8_t const depth =
            type == image_targa::image_type::r8 ?
            8 :
            type == image_targa::image_type::rgb24 ?
                24 :
                32;

        depth_ = depth / 8;

        uint8_t const alpha =
            type == image_targa::image_type::rgba32 ?
            8 :
            0;

        header const head = {
            0,                      //uint8_t        id_length;
            color_map_type::absent, //color_map_type color_map_type;
            targa_type,             //image_type     image_type;
            { //color_map_spec color_map_spec;
                0, //uint16_t first_entry_index;
                0, //uint16_t length;
                0, //uint8_t  entry_size;
            },
            { //image_spec image_spec;
                0,     //uint16_t x_origin;
                0,     //uint16_t y_origin;
                w,     //uint16_t width;
                h,     //uint16_t height;
                depth, //uint8_t  depth;
                { //image_descriptor descriptor;
                    alpha & 0xF, //uint8_t alpha    : 4;
                    0,     //uint8_t right    : 1;
                    1,     //uint8_t top      : 1;
                    0,     //uint8_t reserved : 2;
                }
            }
        };

        size_t const pixel_data_size = depth_ * w * h;
        data_size_ = pixel_data_size + sizeof(header);

        data_.reset(new uint8_t[data_size_]());
        std::fill_n(data_.get(), data_size_, 0);

        header_     = reinterpret_cast<header*>(data_.get());
        image_data_ = data_.get() + sizeof(header);

        memcpy(header_, &head, sizeof(header));
    }

    void set(
        uint16_t x,
        uint16_t y,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t //a
    ) {
        BK_ASSERT(x < width_);
        BK_ASSERT(y < height_);
        
        auto const i = (y*width_ + x) * depth_;

        //image_data_[i+3] = a;
        image_data_[i+0] = b;
        image_data_[i+1] = g;
        image_data_[i+2] = r;
    }

    void save(std::string const& filename) {
        std::ofstream out;
    
        out.open(filename, std::ios::binary | std::ios::out | std::ios::trunc);

        out.write(reinterpret_cast<char*>(data_.get()), data_size_);
    }

    uint16_t width()  const { return width_; }
    uint16_t height() const { return height_; }
    size_t   size()   const { return data_size_; }
private:
    uint16_t width_;
    uint16_t height_;
    uint8_t  depth_;
    size_t   data_size_;

    header*  header_;
    footer*  footer_;
    uint8_t* image_data_;

    std::unique_ptr<uint8_t[]> data_;
};

image_targa::image_targa(
    std::string const& //filename
) {
}

image_targa::image_targa(uint16_t w, uint16_t h, image_type type)
    : impl_(new impl_t(w, h, type))
{
}

image_targa::~image_targa() {
}

uint16_t image_targa::width() const {
    return impl_->width();
}

uint16_t image_targa::height() const {
    return impl_->height();
}

size_t image_targa::size() const {
    return impl_->size();
}

image_targa::image_type image_targa::type() const {
    return image_type::rgb24;
}

void image_targa::save(std::string const& filename) {
    impl_->save(filename);
}

void image_targa::set(
    uint16_t x, uint16_t y,
    uint8_t r, uint8_t g, uint8_t b, uint8_t a
) {
    impl_->set(x, y, r, g, b, a);
}
