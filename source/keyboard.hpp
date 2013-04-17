#pragma once

#include <vector>

enum class command_type {
    nothing,
    direction_n,
    direction_ne,
    direction_e,
    direction_se,
    direction_s,
    direction_sw,
    direction_w,
    direction_nw,
    direction_up,
    direction_down,
    direction_here,
};

struct key_combo {
    key_combo(unsigned code, bool shift = false, bool ctrl = false, bool alt = false)
        : key_code(code), shift(shift), ctrl(ctrl), alt(alt)
    {
    }

    bool operator==(key_combo rhs) const {
        return key_code == rhs.key_code &&
               shift    == rhs.shift &&
               ctrl     == rhs.ctrl &&
               alt      == rhs.alt;
    }

    bool operator!=(key_combo rhs) const {
        return !(*this == rhs);
    }

    unsigned key_code;
    bool shift;
    bool ctrl;
    bool alt;
};

struct key_mapping {
    key_combo    combo;
    command_type command;

    bool operator<(key_mapping rhs) const;
};

class key_mapper {
public:
    key_mapper();

    void set_mapping(key_combo combo, command_type command);

    command_type get_mapping(key_combo combo) const;
private:
    std::vector<key_mapping> mappings_;
};
