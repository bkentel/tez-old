#include "pch.hpp"
#include "map_layout.hpp"

#include "room_generator.hpp" //temp

using tez::map_layout;

namespace {

static auto const PADDING = 1;

typedef tez::map_layout::rect_t rect_t;

//==============================================================================
//! Adjust the position of @p where such that it intersects none of the
//! @p rooms.
//!
//! @returns @c true if @p where intersects no @p rooms or @p where could be moved
//! such that it intersects no @p rooms.
//! @returns @c false otherwise.
//==============================================================================
bool adjust_rect(
    rect_t&                      where,
    map_layout::room_list const& rooms
) {
    static auto const MAX_ATTEMPTS = 5u;   

    auto const beg = std::cbegin(rooms);
    auto const end = std::cend(rooms);

    //Make MAX_ATTEMPTS attempts to relocate [where] while there are still
    //intersections.
    for (auto i = 0; i < MAX_ATTEMPTS; ++i) {
        auto const it = std::find_if(beg, end, [&](tez::room const& room) {
            return intersects(where, room.bounds());
        });        

        if (it == end) {
            //no intersections
            return true;
        }

        auto const& other = it->bounds();

        //move [where] the minimum distance possible so that it no longer
        //intersects
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

        //try again
    }

    return false;
}

//==============================================================================
//! Return @p rect such that the centres of @p rect and @p reference are equal.
//==============================================================================
rect_t get_centered_rect(
    rect_t const reference,
    rect_t const rect
) {
    auto const dx =
        static_cast<signed>(reference.width()) -
        static_cast<signed>(rect.width());

    auto const dy =
        static_cast<signed>(reference.height()) -
        static_cast<signed>(rect.height());

    return bklib::translate_to(rect,
        reference.left + dx / 2,
        reference.top  + dy / 2
    );
}

//==============================================================================
//! Return @p room offset such that it is centered relative to @p reference and
//! offset toward @p dir such that it does not intersect @p reference.
//==============================================================================
rect_t get_rect_relative_to(
    tez::direction const dir,
    rect_t         const reference,
    rect_t         const room
) {
    auto const room_rect = get_centered_rect(reference, room);

    auto const translate = [&](signed const dx, signed const dy) {
        return bklib::translate_by(room_rect, dx, dy);
    };

    unsigned distance = 0;

    switch (dir) {
    case tez::direction::north :
        distance = bklib::distance(room_rect.bottom, reference.top);
        return translate(0, 0 - PADDING - distance);
    case tez::direction::south :
        distance = bklib::distance(room_rect.top, reference.bottom);
        return translate(0, PADDING + distance);
    case tez::direction::east :
        distance = bklib::distance(room_rect.right, reference.left);
        return translate(0 - PADDING - distance, 0);
    case tez::direction::west :
        distance = bklib::distance(room_rect.left, reference.right);
        return translate(PADDING + distance, 0);
    }

    //should never get here
    BK_ASSERT(false);

    return room_rect;
}

} //namespace

void map_layout::add_room(tez::room room) {
    //--------------------------------------------------------------------------
    // Add candidates for each cardinal direction except [from] in random order.
    //--------------------------------------------------------------------------
    auto const add_candidates = [&](direction const from, rect_t const rect) {
        auto dir = tez::random_cardinal_direction(random_);
        for (auto i = 0u; i < NUM_CARDINAL_DIR-1; ++i) {
            if (dir != from) {
                candidates_.emplace(dir, rect);
            }

            dir = tez::next_cardinal_direction(dir);
        }
    };
    //--------------------------------------------------------------------------
    // Return a usuable candidate position.
    //--------------------------------------------------------------------------
    auto const get_candidate = [&] {
        if (candidates_.empty()) {
            //add candidates for the rect containing all current rooms.
            add_candidates(direction::here, rect_t(
                extent_x_.min, extent_y_.min, extent_x_.max, extent_y_.max
            ));
        }

        auto const candidate = candidates_.front();
        candidates_.pop();

        return candidate;
    };
    //--------------------------------------------------------------------------
    auto where = rect_t(0, 0, 0, 0);
    auto dir   = direction::here;

    //find a useable candidate
    while (!where || !adjust_rect(where, rooms_)) {
        std::tie(dir, where) = get_candidate();
        where = get_rect_relative_to(dir, where, room.bounds());
    }

    add_candidates(tez::opposite_direction(dir), where);

    room.translate_to(where.left, where.top);
    rooms_.emplace_back(std::move(room));

    extent_x_(where.left);
    extent_x_(where.right);
    extent_y_(where.top);
    extent_y_(where.bottom);
}

tez::map map_layout::make_map() {
    static unsigned const MAX_ATTEMPTS_PER_ROOM = 5;
    static unsigned const MAX_ATTEMPTS_PER_DIR  = 5;

    auto result = tez::map(extent_x_.distance(), extent_y_.distance());

    if (extent_x_.min < 0 || extent_y_.min < 0) {
        normalize();
    }

    for (auto const& room : rooms_) {
        result.add_room(room);
    }
   
    auto pg    = path_generator(bklib::make_random_wrapper(random_));
    auto graph = boost::adjacency_matrix<boost::undirectedS>(rooms_.size());

    //--------------------------------------------------------------------------
    // Get a random NSEW direction
    //--------------------------------------------------------------------------
    auto const find_path = [&](tez::room const& room)
        -> std::pair<bool, unsigned>
    {
        bool found_path = false;

        for (unsigned i = 0; !found_path && i < MAX_ATTEMPTS_PER_ROOM; ++i) {
            auto const side = random_cardinal_direction(random_);
            
            for (unsigned j = 0; !found_path && j < MAX_ATTEMPTS_PER_DIR; ++j) {
                found_path = pg.generate(room, result, side);
            }
        }
        
        if (!found_path) {
            return std::make_pair(false, 0u);
        }

        auto start_point = static_cast<room::point_t>(pg.start_point());
        auto end_point   = static_cast<room::point_t>(pg.end_point());

        BK_ASSERT(room.contains(start_point));
            
        unsigned end_index = 0;
        for (auto const& r : rooms_) {
            if (r.contains(end_point)) break;
            ++end_index;
        }
        
        return std::make_pair(true, end_index);
    };
    //--------------------------------------------------------------------------

    
    unsigned src_index  = 0;
    unsigned end_index  = 0;
    bool     found_path = false;
    
    //for each room, attempt to find a path that connects it to another room.
    for (auto const& room : rooms_) {    
        std::tie(found_path, end_index) = find_path(room);

        if (found_path) {
            boost::add_edge(src_index, end_index, graph);
            pg.write_path(result);
        }

        ++src_index;
    }
    
    std::vector<unsigned> components(rooms_.size(), 0);
    std::vector<unsigned> vertex_counts(rooms_.size(), 0);
    std::vector<unsigned> components_after(rooms_.size(), 0);

    //while there is more than one component in the graph
    for (
        auto count = boost::connected_components(graph, &components[0]);
        count > 1;
    ) {
        //count the total verticies in each component
        for (auto c : components) {
            ++vertex_counts[c];
        }

        //the index of the component with the fewest verticies
        auto const min = [&] {
            auto const beg = std::cbegin(vertex_counts);
            auto const end = std::cend(vertex_counts);
            return static_cast<unsigned>(
                std::distance(beg, std::min_element(beg, beg + count))
            );
        }();

        auto const beg = std::cbegin(components);
        auto const end = std::cend(components);
        
        //while a bridge between components has not been found
        for (bool found_bridge = false; !found_bridge;) {
            //for each vertex, in order, in the component with the minimum
            //number of verticies attempt to add a new path as a bridge
            for (
                auto where = std::find(beg, end, min);
                !found_bridge && where != end;
                where = std::find(++where, end, min)
            ) {
                src_index = static_cast<unsigned>(std::distance(beg, where));
                auto const& room = rooms_[src_index];

                std::tie(found_path, end_index) = find_path(room);
                if (!found_path) continue;

                bool const exists = boost::edge(src_index, end_index, graph).second;
                if (exists) continue;

                boost::add_edge(src_index, end_index, graph);

                auto const new_count = boost::connected_components(
                    graph, &components_after[0]
                );
                BK_ASSERT(new_count <= count);

                //the path wasn't a bridge; try again
                if (new_count == count) {
                    boost::remove_edge(src_index, end_index, graph);
                    continue;
                }

                found_bridge = true;
                
                //commit the new path and components
                pg.write_path(result);
                count = new_count;
                std::copy(
                    std::cbegin(components_after),
                    std::cend(components_after),
                    std::begin(components)
                );

                //reset the vertex counts
                std::fill(
                    std::begin(vertex_counts), std::end(vertex_counts), 0
                );
            }
        }
    }

    return result;
}

void map_layout::normalize() {
    auto const dx = extent_x_.min;
    auto const dy = extent_y_.min;

    for (auto& room : rooms_) {
        room.translate_by(0 - dx, 0 - dy);
    }

    extent_x_.min =  0;
    extent_x_.max -= dx;

    extent_y_.min =  0;
    extent_y_.max -= dy;
}
