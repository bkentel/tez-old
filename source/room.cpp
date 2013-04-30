#include "pch.hpp"
#include "room.hpp"
#include "util.hpp"

namespace {

static std::default_random_engine random(1984);
static auto const PADDING = 2;

template <typename T>
inline T distance(T const a, T const b) {
    return a >= b ? a - b : b - a;
}

template <typename T>
T min(T const head) {
    return head;
}

template <typename T, typename... Ts>
T min(T const head, Ts... tail) {
    T const tail_min = min(tail...);
    return head <= tail_min ? head : tail_min;
}

typedef rect<signed>    rect_t;
typedef point2d<signed> point_t;

//==========================================================================
//! Return a valid rect where the rect given by [where] can be placed,
//! otherwise return an invalid rect.
//==========================================================================
rect_t find_rect_at(
    rect_t                 where,
    room::room_list const& rooms
) {
    static size_t const COUNT = 4;
    rect_t const* intersections[COUNT] = {nullptr};

    //----------------------------------------------------------------------
    // Fills [intersections] with up to [COUNT] rects that intersect with
    // [where] and return the number of intersections found.
    //----------------------------------------------------------------------        
    auto const find_intersections = [&] { 
        for (size_t i = 0; i < COUNT; ++i) intersections[i] = nullptr;

        unsigned count = 0;
        for (auto const& r : rooms) {
            auto const& other = r->bounds();
                
            if (intersection_of(where, other).is_rect()) {
                intersections[count] = &other;
                if (++count == COUNT) break;
            }
        }

        return count;
    };
    //----------------------------------------------------------------------  

    unsigned intersection_count = 0; //number of intersecting rooms
    unsigned adjust_count       = 0; //numner of attempted adjustments

    //attempt to relocate [where] while there are intersections
    while ((intersection_count = find_intersections()) != 0) {
        //TODO: handle cases where more than one intersection is found.
        if (intersection_count != 1 || adjust_count++ == 4) {
            return rect_t(0, 0, 0, 0);
        }

        auto const& other = *intersections[0];

        auto const left   = distance(where.right,  other.left);
        auto const right  = distance(where.left,   other.right);
        auto const top    = distance(where.bottom, other.top);
        auto const bottom = distance(where.top,    other.bottom);

        auto const min_dim = min(left, right, top, bottom);
            
        if (min_dim == left) {
            where = separate_rects<direction::west>::get(
                where, other, PADDING
            );
        } else if (min_dim == right) {
            where = separate_rects<direction::east>::get(
                where, other, PADDING
            );
        } else if (min_dim == top) {
            where = separate_rects<direction::north>::get(
                where, other, PADDING
            );
        } else {
            where = separate_rects<direction::south>::get(
                where, other, PADDING
            );
        }
    }

    return where;
}

//==========================================================================
//! Return [r] centered around [reference].
//==========================================================================
rect_t get_centered_rect(
    rect_t const reference,
    rect_t const r
) {
    auto const dx =
        static_cast<signed>(reference.width()) -
        static_cast<signed>(r.width());

    auto const dy =
        static_cast<signed>(reference.height()) -
        static_cast<signed>(r.height());

    return rect_t(
        point_t(
            reference.left + dx / 2,
            reference.top  + dy / 2
        ),
        r.width(),
        r.height()
    );
}

//==========================================================================
//! Returns a valid rect of the same size as [r] offset from [reference] in
//! the direction given  by [dir] where [r] can be placed. Otherwise,
//! returns an invalid rect.
//==========================================================================
rect_t get_rect_relative_to(
    direction const  dir,
    rect_t    const  reference,
    room      const& r
) {
    auto const room_rect   = get_centered_rect(reference, r.bounds());
    auto       result_rect = rect_t(0, 0, 0, 0);

    #define BK_CASE_DIR(D)                        \
        case D :                                  \
            result_rect = separate_rects<D>::get( \
                room_rect, reference, PADDING     \
            );                                    \
        break

    switch (dir) {
    BK_CASE_DIR(direction::north);
    BK_CASE_DIR(direction::south);
    BK_CASE_DIR(direction::east);
    BK_CASE_DIR(direction::west);
    }

    #undef BK_CASE_DIR

    return result_rect;
}

} //namespace

//==========================================================================
//! Layout the list of rooms given by [rooms] such that no rooms overlap.
//==========================================================================    
room::room_list room::layout(room_list rooms) {
    room_list       result_rooms;
    min_max<signed> mm_x;
    min_max<signed> mm_y;

    std::queue<std::pair<direction, rect_t>> candidates;

    //----------------------------------------------------------------------
    //
    //----------------------------------------------------------------------
    auto const add_candidates = [&](direction const from, rect_t const r) {
        static direction const direction[] = {
            direction::north,
            direction::east,
            direction::south,
            direction::west,
        };

        auto const first = std::uniform_int_distribution<>(0, 3)(random);

        for (unsigned i = 0; i < 4; ++i) {
            auto const dir = direction[(first + i) % 4];
            if (dir != from) {
                candidates.emplace(dir, r);
            }
        }
    };
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    //
    //----------------------------------------------------------------------
    auto const add_room = [&](std::unique_ptr<room> r, rect_t where) {
        r->translate_to(where.left, where.top);
        result_rooms.emplace_back(std::move(r));

        mm_x(where.left);
        mm_x(where.right);
        mm_y(where.top);
        mm_y(where.bottom);
    };
    //----------------------------------------------------------------------

    candidates.emplace(direction::none, rect_t(0, 0, 0, 0));

    while (!rooms.empty()) {
        auto r = std::move(rooms.back());
        rooms.pop_back();

        while (!candidates.empty()) {
            auto candidate = candidates.front();
            candidates.pop();

            auto result_rect = get_rect_relative_to(candidate.first, candidate.second, *r);
            BK_ASSERT(result_rect.is_rect());

            result_rect = find_rect_at(result_rect, result_rooms);

            if (result_rect.is_rect()) {
                add_room(std::move(r), result_rect);
                add_candidates(opposite(candidate.first), result_rect);

                break;
            }

            if (candidates.empty()) {
                __debugbreak(); //TODO
            }
        }
    }

    for (auto& r : result_rooms) {
        r->translate_by(-mm_x.min, -mm_y.min);
    }

    return result_rooms;        
}
