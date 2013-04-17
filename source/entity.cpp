#include "pch.hpp"
#include "entity.hpp"

#include "level.hpp"

entity::entity(coord_t x, coord_t y, level* level)
    : level_(level)
    , x_(x), y_(y)
{
}

void entity::attach(level& level) {
    BK_ASSERT(level_ != &level);
    level_ = &level;
}

void entity::detach(level& level) {
    BK_ASSERT(level_ == &level);

    level_ = nullptr;
}

bool entity::relocate_to(coord_t x, coord_t y) {
    if (level_ == nullptr) {
        return false;
    }

    if (level_->can_relocate_to(*this, x, y)) {
        set_position_(x, y);
        return true;
    }

    return false;
}

bool entity::move_to(direction dir) {
    if (level_ == nullptr) {
        return false;
    }

    if(level_->can_move_to(*this, dir)) {
        move_to_(dir);
        return true;
    }

    return false;
}

location_t entity::location() const {
    location_t const result = {x_, y_, 0};
    return result;
}

void entity::move_to_(direction const dir) {
    auto const delta = get_delta(dir);

    x_ += delta.dx;
    y_ += delta.dy;
}

void entity::set_position_(coord_t x, coord_t y) {
    x_ = x;
    y_ = y;
}
