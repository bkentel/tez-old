#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace bklib {

//==============================================================================
//! Targa image file manipulation.
//==============================================================================
class image_targa {
public:
    enum class image_type {
        rgb24,
        rgba32,
        r8,
    };

    explicit image_targa(std::string const& filename);

    image_targa(uint16_t w, uint16_t h, image_type type);

    ~image_targa();

    uint16_t   width()  const;
    uint16_t   height() const;
    size_t     size()   const;
    image_type type()   const;

    void save(std::string const& filename);

    void set(
        uint16_t x, uint16_t y,
        uint8_t r, uint8_t g, uint8_t b, uint8_t a
    );
private:
    class impl_t;
    std::unique_ptr<impl_t> impl_;
};

} //namespace bklib
