#include "pch.hpp"
#include "map_layout.hpp"

#include "geometry.hpp"
#include "room.hpp"
#include "map.hpp"

#include "room_generator.hpp" //temp

namespace {

static auto const PADDING = 2;

typedef rect<signed>    rect_t;
typedef point2d<signed> point_t;

//==============================================================================
//! Return a valid rect where the rect given by [where] can be placed,
//! otherwise return an invalid rect.
//==============================================================================
rect_t find_rect_at(
    rect_t                       where,
    map_layout::room_list const& rooms
) {
    static size_t const COUNT = 4;

    rect_t const* intersections[COUNT] = {nullptr};

    //----------------------------------------------------------------------
    // Fills [intersections] with up to [COUNT] rects that intersect with
    // [where] and return the number of intersections found.
    //----------------------------------------------------------------------        
    auto const find_intersections = [&] { 
        for (size_t i = 0; i < COUNT; ++i) {
            intersections[i] = nullptr;
        }

        unsigned count = 0;
        for (auto const& r : rooms) {
            auto const& other = r.bounds();
                
            if (intersection_of(where, other).is_rect()) {
                intersections[count] = &other;
                if (++count == COUNT) break;
            }
        }

        return count;
    };

    unsigned adjust_count = 0; //numner of attempted adjustments

    //attempt to relocate [where] while there are intersections
    while (unsigned const intersection_count = find_intersections()) {
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

//==============================================================================
//! Return [r] centered around [reference].
//==============================================================================
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

//==============================================================================
//! Returns a valid rect of the same size as [r] offset from [reference] in
//! the direction given  by [dir] where [r] can be placed. Otherwise,
//! returns an invalid rect.
//==============================================================================
rect_t get_rect_relative_to(
    direction const  dir,
    rect_t    const  reference,
    room      const& r
) {
    auto const room_rect   = get_centered_rect(reference, r.bounds());
    auto       result_rect = rect_t(0, 0, 0, 0);

    return separate_rects_toward(dir, room_rect, reference, PADDING);
}

} //namespace

//==============================================================================
//! Add [r] to the layout such that it intersects no existing rooms.
//==============================================================================
void map_layout::add_room(room r, random_t random) {
    //--------------------------------------------------------------------------
    // Add candidates with [r] as the source for all directions other than
    // [from].
    //--------------------------------------------------------------------------
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
                candidates_.emplace(dir, r);
            }
        }
    };
    //--------------------------------------------------------------------------
    // Finalize the placement of the room [r].
    //--------------------------------------------------------------------------
    auto const add_room = [&](room& r, rect_t const where) {
        r.translate_to(where.left, where.top);
        rooms_.emplace_back(std::move(r));

        extent_x_(where.left);
        extent_x_(where.right);
        extent_y_(where.top);
        extent_y_(where.bottom);
    };
    //--------------------------------------------------------------------------
        
    //find a useable candidate
    for (;;) {
        if (candidates_.empty()) {
            //add candidates for the rect containing all current rooms.
            add_candidates(
                direction::here,
                rect_t(extent_x_.min, extent_y_.min,
                       extent_x_.max, extent_y_.max
                )
            );
        }

        //get a candidate location
        auto const candidate = candidates_.front();
        candidates_.pop();

        //try to place the room
        auto const result_rect = find_rect_at(
            get_rect_relative_to(
                candidate.first, candidate.second, r
            ),
            rooms_
        );

        //ok
        if (result_rect.is_rect()) {
            add_room(r, result_rect);
            add_candidates(opposite(candidate.first), result_rect);

            break;
        }
    }
}

static std::vector<point2d<unsigned>> path;

bool make_connection(room const& r, map& m, std::default_random_engine& random) {   
    std::uniform_int_distribution<> dir_dist(0, 3);
    direction dir = [&]() -> direction {
        switch (dir_dist(random)) {
        case 0 : return direction::north;
        case 1 : return direction::south;
        case 2 : return direction::east;
        case 3 : return direction::west;
        }

        return direction::none;
    }();

    auto gen = path_generator(dir);
    auto p = r.find_connectable_point(make_random_wrapper(random), dir);

    unsigned fail_count = 0;

    //std::vector<point2d<unsigned>> path;
    //path.reserve(32);
    path.clear();
    path.push_back(p);

    bool found_path = false;

    while (p.x > 0 && p.y > 0 && p.x < m.width() - 1 && p.y < m.height() - 1) {
        auto const dir = gen(random);
        auto const dx = get_x_vector(dir);
        auto const dy = get_y_vector(dir);

        auto const x = p.x + dx;
        auto const y = p.y + dy;

        auto const& type = m.at(x, y).type;

        if (type == tile_category::corridor ||
            type == tile_category::empty
        ) {
            path.emplace_back(x, y);
        } else if (!r.bounds().contains(x, y)) {
            path.emplace_back(x, y);
            found_path = true;
            break;
        } else {
            if (fail_count++ < 10) {
                //std::cout << "try again\n";
                continue;
            } else {
                //std::cout << "fail: count\n";
                return false;
            }
        }
        
        p.x = x;
        p.y = y;

        fail_count = fail_count > 0 ? --fail_count : 0;
    }

    if (path.size() < 2 || !found_path) {
        //std::cout << "fail: path\n";
        return false;
    }

    m.at(path[0].x, path[0].y).type = tile_category::door;
    
    for (unsigned i = 1; i < path.size() - 1; ++i) {
        m.at(path[i].x, path[i].y).type = tile_category::corridor;
    }
    
    auto const last = path.size() - 1;
    m.at(path[last].x, path[last].y).type = tile_category::door;

    return true;
}

map map_layout::make_map() const {
    auto const dx = extent_x_.min;
    auto const dy = extent_y_.min;

    auto result = map(extent_x_.distance(), extent_y_.distance());

    for (auto const& r : rooms_) {
        result.add_room(r, 0 - dx, 0 - dy);
    }

    std::default_random_engine random(1984);

    for (auto const& r : rooms_) {
        while(!make_connection(r, result, random));
    }
    
    return result;
}

void map_layout::normalize() {
    auto const dx = extent_x_.min;
    auto const dy = extent_y_.min;

    for (auto& r : rooms_) {
        r.translate_by(0 - dx, 0 - dy);
    }

    extent_x_.min =  0;
    extent_x_.max -= dx;

    extent_y_.min =  0;
    extent_y_.max -= dy;
}
