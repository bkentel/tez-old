#include "pch.hpp"
#include "tile_map.hpp"

tile_grid::tile_grid(unsigned width, unsigned height)
    : data_(width, height)
{
}

////////////////////////////

namespace {
static std::default_random_engine random(1984);
static auto const PADDING = 2;

struct layout_generator {
    typedef rect<signed>    rect_t;
    typedef point2d<signed> point_t;

    std::vector<room> placed_rooms;

    static unsigned delta_w(rect_t const a, rect_t const b) {
        return a.width() >= b.width()
            ? a.width() - b.width()
            : b.width() - a.width();
    }

    static unsigned delta_h(rect_t const a, rect_t const b) {
        return a.height() >= b.height()
            ? a.height() - b.width()
            : b.height() - a.height();
    }

    static rect_t place_relative_to_n(rect_t const reference, rect_t const r) {
        auto const x   = reference.left;
        auto const y   = reference.top;
        auto const off = delta_w(reference, r) / 2;

        return rect_t(
            point_t(x + off, y - (r.height() + PADDING)),
            r.width(),
            r.height()
        );
    }

    static rect_t place_relative_to_s(rect_t const reference, rect_t const r) {
        auto const x   = reference.left;
        auto const y   = reference.bottom;
        auto const off = delta_w(reference, r) / 2;

        return rect_t(
            point_t(x + off, y + (r.height() + PADDING)),
            r.width(),
            r.height()
        );
    }

    static rect_t place_relative_to_w(rect_t const reference, rect_t const r) {
        auto const x   = reference.left;
        auto const y   = reference.top;
        auto const off = delta_h(reference, r) / 2;

        return rect_t(
            point_t(x - (r.width() + PADDING), y + off),
            r.width(),
            r.height()
        );
    }

    static rect_t place_relative_to_e(rect_t const reference, rect_t const r) {
        auto const x   = reference.right;
        auto const y   = reference.top;
        auto const off = delta_h(reference, r) / 2;

        return rect_t(
            point_t(x + (r.width() + PADDING), y + off),
            r.width(),
            r.height()
        );
    }

    rect_t place_at(rect_t const where) {
        auto intersection = rect_t(0, 0, 0, 0);
        auto other        = rect_t(0, 0, 0, 0);

        auto const it = std::find_if(
            std::begin(placed_rooms),
            std::end(placed_rooms),
            [&](room const& r) {
                other        = r.bounds();
                intersection = intersection_of(where, other);

                return intersection.is_rect();
            }
        );

        if (it == std::end(placed_rooms)) {
            return where;
        }

        return rect_t(0, -1, 0, -1);
    }

    rect_t place_relative_to(rect_t const reference, rect_t const r) {
        static decltype(&place_relative_to_n) const functions[] = {
            &place_relative_to_n,
            &place_relative_to_s,
            &place_relative_to_e,
            &place_relative_to_w,
        };
    
        std::uniform_int_distribution<> dist(0, 3);

        auto const i = dist(random);
        
        rect_t result =
            place_at(functions[(i + 0) % 4](reference, r));

        return result.is_rect() ? result :
            (result = place_at(functions[(i + 1) % 4](reference, r))).is_rect() ? result :
            (result = place_at(functions[(i + 2) % 4](reference, r))).is_rect() ? result :
            (result = place_at(functions[(i + 3) % 4](reference, r))).is_rect() ? result : result;
    }

    void layout(std::vector<room> rooms) {
        std::vector<room*> result_rooms;

        rect_t reference = rect_t(0, 0, 0, 0);

        while (!rooms.empty()) {
            room r = std::move(rooms.back());
            rooms.pop_back();

            auto const result_rect = place_relative_to(reference, r.bounds());
            if (result_rect.is_rect()) {
                r.translate_to(result_rect.left, result_rect.top);
                placed_rooms.emplace_back(std::move(r));
            } else {
                break;
            }
        }
    };
};

} //namespace