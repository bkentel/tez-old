#include "pch.hpp"
#include "map_layout.hpp"

#include "room_generator.hpp" //temp

using tez::map_layout;

namespace {

static auto const PADDING = 1;

typedef tez::map_layout::rect_t rect_t;

//==============================================================================
//! Return a valid rect where the rect given by [where] can be placed,
//! otherwise return an invalid rect.
//==============================================================================
rect_t
find_rect_at(
    rect_t                 where,
    map_layout::room_list const& rooms
) {
    static size_t const MAX_INTERSECTIONS = 4;
    rect_t const* intersections[MAX_INTERSECTIONS] = {nullptr};

    //--------------------------------------------------------------------------
    // Fills [intersections] with up to [COUNT] rects that intersect with
    // [where] and return the number of intersections found.
    //--------------------------------------------------------------------------
    auto const find_intersections = [&] { 
        unsigned count = 0;
        for (auto const& room : rooms) {
            auto const& bounds = room.bounds();
                
            if (intersects(where, bounds)) {
                intersections[count++] = &bounds;
                if (count == MAX_INTERSECTIONS) break;
            }
        }

        return count;
    };
    //--------------------------------------------------------------------------
    static auto const MAX_ATTEMPTS = 5u;
    
    unsigned adjust_count       = 0; //numner of attempted adjustments
    unsigned intersection_count = 0;

    //attempt to relocate [where] while there are intersections
    while (intersection_count = find_intersections()) {
        if (adjust_count++ >= MAX_ATTEMPTS) {
            return rect_t(0, 0, 0, 0);
        }

        auto const& other = *intersections[0];

        auto const left   = bklib::distance(where.right,  other.left);
        auto const right  = bklib::distance(where.left,   other.right);
        auto const top    = bklib::distance(where.bottom, other.top);
        auto const bottom = bklib::distance(where.top,    other.bottom);

        auto const min_dim = bklib::min(left, right, top, bottom);
        
        where = (min_dim == left)  ?
                    bklib::translate_by(where, 0 - left - PADDING, 0)  :
                (min_dim == right) ?
                    bklib::translate_by(where, 0 + right + PADDING, 0) :
                (min_dim == top)   ?
                    bklib::translate_by(where, 0, 0 - top - PADDING) :
                    bklib::translate_by(where, 0, 0 + bottom + PADDING);
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
        bklib::make_point(
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
    tez::direction const  dir,
    rect_t         const  reference,
    tez::room      const& r
) {
    auto const room_rect   = get_centered_rect(reference, r.bounds());
    auto       result_rect = rect_t(0, 0, 0, 0);

    auto const left   = bklib::distance(room_rect.right,  reference.left);
    auto const right  = bklib::distance(room_rect.left,   reference.right);
    auto const top    = bklib::distance(room_rect.bottom, reference.top);
    auto const bottom = bklib::distance(room_rect.top,    reference.bottom);

    switch (dir) {
    case tez::direction::north : return bklib::translate_by(room_rect, 0, 0 - PADDING - top);
    case tez::direction::south : return bklib::translate_by(room_rect, 0, 0 + PADDING + bottom);
    case tez::direction::east :  return bklib::translate_by(room_rect, 0 - PADDING - left, 0);
    case tez::direction::west :  return bklib::translate_by(room_rect, 0 + PADDING + right, 0 );
    }

    return room_rect;
}

} //namespace

//==============================================================================
//! Add [r] to the layout such that it intersects no existing rooms.
//==============================================================================
void map_layout::add_room(tez::room r) {
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

        auto const first = std::uniform_int_distribution<>(0, 3)(random_);

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
        if (result_rect) {
            add_room(r, result_rect);
            add_candidates(tez::opposite_direction(candidate.first), result_rect);

            break;
        }
    }
}

tez::map map_layout::make_map() {
    //--------------------------------------------------------------------------
    // Get a random NSEW direction
    //--------------------------------------------------------------------------
    auto const get_random_direction = [&]() -> direction {
        direction const dir[] = {
            direction::north, direction::south,
            direction::east,  direction::west,
        };

        auto const i = std::uniform_int_distribution<unsigned>(0, 3)(random_);
        
        return dir[i];
    };
    //--------------------------------------------------------------------------
    auto result = tez::map(extent_x_.distance(), extent_y_.distance());

    for (auto const& r : rooms_) {
        //translate the rooms such that all rooms have positive x and y.
        result.add_room(r, 0 - extent_x_.min, 0 - extent_y_.min);
    }

    //std::cout << "generating paths..." << std::endl;
    
    auto pg = path_generator(bklib::make_random_wrapper(random_));

    //unsigned room_num = 0;

    for (auto const& room : rooms_) {
        //std::cout << "generating paths for room " << (room_num++) << std::endl;

        for (bool path = false; !path; ) {
            auto const side = get_random_direction();
            
            //switch (side) {
            //case direction::north : std::cout << "trying north side..." << std::endl; break;
            //case direction::south : std::cout << "trying south side..." << std::endl; break;
            //case direction::east :  std::cout << "trying east side..." << std::endl; break;
            //case direction::west :  std::cout << "trying west side..." << std::endl; break;
            //default : BK_ASSERT(false); break;
            //}


            for (unsigned i = 0; !path && i < 10; ++i) {
                path = pg.generate(room, result, side);
            }
        }
        
        pg.write_path(result);
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