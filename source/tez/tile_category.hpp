#pragma once

#include <ostream>
#include <cstdint>

namespace tez {

//! Tile categories used in map generation.
enum class tile_category : uint8_t {
    empty         = ' ',
    wall          = '|',
    ceiling       = '#',
    floor         = '.',
    pit           = '_',
    water         = '=',
    door          = 'D',
    corridor      = '+',
};

inline std::ostream& operator<<(std::ostream& out, tile_category const t) {
    return out << static_cast<char>(t);
}

} //namespace tez
