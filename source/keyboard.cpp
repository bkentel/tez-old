#include "pch.hpp"
#include "keyboard.hpp"

bool key_mapping::operator<(key_mapping rhs) const {
    unsigned const a =
        (combo.shift ? 1 : 0) << 0 |
        (combo.ctrl  ? 1 : 0) << 1 |
        (combo.alt   ? 1 : 0) << 2;

    unsigned const b =
        (rhs.combo.shift ? 1 : 0) << 0 |
        (rhs.combo.ctrl  ? 1 : 0) << 1 |
        (rhs.combo.alt   ? 1 : 0) << 2;


    return (combo.key_code == rhs.combo.key_code) ?
        (a < b) : (combo.key_code < rhs.combo.key_code);
}

key_mapper::key_mapper() {
    // should be enough to cover all virtual key codes
    mappings_.reserve(0xFF);
}

void key_mapper::set_mapping(key_combo combo, command_type command) {
    key_mapping const mapping{combo, command};

    auto const where = std::lower_bound(std::begin(mappings_), std::end(mappings_), mapping);

    if (where == std::end(mappings_)) {
        mappings_.push_back(mapping);
    } else if (where->combo == combo) {
        where->command = command;
    } else {
        size_t const beg = std::distance(std::begin(mappings_), where);
        mappings_.push_back(mapping);
        size_t const end = mappings_.size();

        BK_ASSERT(end >= 1);

        for (size_t i = end - 1; i > beg; --i) {
            std::swap(mappings_[i], mappings_[i-1]);
        }
    }   
}

command_type key_mapper::get_mapping(key_combo combo) const {
    key_mapping const mapping{combo, command_type::nothing};

    auto const where = std::lower_bound(std::begin(mappings_), std::end(mappings_), mapping);

    return (where == std::end(mappings_) || where->combo != combo) ?
        command_type::nothing : where->command;

}
