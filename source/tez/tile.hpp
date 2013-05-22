#pragma once

#include "tile_category.hpp"

#include <cstdint>

namespace tez {

//struct background_tile {
//    unsigned type    : 16;
//    unsigned flags   : 16;
//    unsigned texture : 16;
//    unsigned unused  : 16;
//    unsigned data    : 64;
//};

struct tile_data {
    template <typename T>
    T& get_data() {
        static_assert(sizeof(T) <= sizeof(data), "type is too big");
        BK_ASSERT(type == T::type);
        return reinterpret_cast<T&>(data);
    }

    template <typename T>
    T const& get_data() const {
        static_assert(sizeof(T) <= sizeof(data), "type is too big");
        BK_ASSERT(type == T::type);
        return reinterpret_cast<T const&>(data);
    }

    tile_category type;
    
    struct {
        uint8_t has_data    : 1;
        uint8_t is_passable : 1;
        uint8_t unused0     : 1;
        uint8_t unused1     : 1;
        uint8_t unused2     : 1;
        uint8_t unused3     : 1;
        uint8_t unused4     : 1;
        uint8_t unused5     : 1;
    } flags;
    
    uint16_t texture[3];
    uint64_t data;
};

struct door_data {
    static auto const type = tile_category::door;

    enum class door_state {
        open,
        closed,
        locked,
        broken,
    } state;
};

} //namespace tez
