#include "pch.hpp"
#include "room_generator.hpp"

tez::simple_room_generator::simple_room_generator(random_t random)
    : generator(random)
{
}

tez::room
tez::simple_room_generator::generate() {
    static auto const MIN_W = 3;
    static auto const MAX_W = 10;
    static auto const MIN_H = 4;
    static auto const MAX_H = 10;

    typedef std::uniform_int_distribution<unsigned> distribution_t;

    auto const w = distribution_t(MIN_W, MAX_W)(random_);
    auto const h = distribution_t(MIN_H, MAX_H)(random_);

    grid_t result(w, h, tile_category::floor);
    
    for (auto& b : result.block_iterator()) {
        auto const x = b.x;
        auto const y = b.y;

        if ((x == 0)     || (y == 0) ||
            (x == w - 1) || (y == h - 1)
        ) {
            result.at(x, y) = tile_category::ceiling;
        } else if(*b.north() == tile_category::ceiling) {
            result.at(x, y) = tile_category::wall;
        }
    }

    return room(std::move(result));
}

tez::simple_room_generator::connection_point
tez::simple_room_generator::find_connection_point(
    direction side,
    random_t random
) {

    return connection_point(0, 0);
}

tez::compound_room_generator::compound_room_generator(random_t random)
    : generator(random)
{
}

//namespace {
//    typedef tez::compound_room_generator::point_t point_t;
//    typedef std::vector<point_t> point_list;
//    typedef tez::grid2d<tez::tile_category> grid_t;
//
//    point_list& generate_points(point_list& out) {
//    }
//
//    grid_t points_to_grid(point_list const& points) {
//    }
//
//    void transform(grid_t& grid) {
//    }
//}

tez::compound_room_generator::grid_t
tez::compound_room_generator::make_compound_room_base_() {
    typedef std::uniform_int_distribution<unsigned> distribution_t;

    static auto const CELL_SIZE_MIN = 4;
    static auto const CELL_SIZE_MAX = 6;
    static auto const COUNT_MIN     = 10;
    static auto const COUNT_MAX     = 20;

    auto const cell_size  = distribution_t(CELL_SIZE_MIN, CELL_SIZE_MAX)(random_);
    auto const cell_count = distribution_t(COUNT_MIN, COUNT_MAX)(random_);   

    bklib::min_max<> range_x;
    bklib::min_max<> range_y;

    //--------------------------------------------------------------------------
    // Get an unused point offset in a cardinal direction from [p],
    // otherwise [p].
    //--------------------------------------------------------------------------
    auto const get_point = [&](point_t const p) -> point_t {
        //----------------------------------------------------------------------
        auto const is_occupied = [&](point_t const p) {
            return std::end(points_) != std::find(
                std::begin(points_), std::end(points_), p
            );
        };
        //----------------------------------------------------------------------
        BK_DECLARE_DIRECTION_ARRAYS(dx, dy);

        auto const base = distribution_t(0, 3)(random_);

        for (unsigned i = 0; i < 4; ++i) {
            unsigned const j = (base + i) % 4u;
            auto     const q = point_t(p.x + dx[j], p.y + dy[j]);
            
            if (!is_occupied(q)) return q;
        }

        return p;
    };
    //--------------------------------------------------------------------------
    // Add a point and update the x and y range
    //--------------------------------------------------------------------------
    auto const add_point = [&](point_t const p) {
        range_x(p.x);
        range_y(p.y);
        points_.push_back(p);
    };
    //--------------------------------------------------------------------------

    points_.clear();
    points_.reserve(cell_count);
    
    point_t p(0, 0); //current position
    add_point(p);

    while (points_.size() < cell_count) {
        auto const q = get_point(p);
        
        if (p == q) {
            break; //failed to move
        }
            
        add_point(p = q); //add the point and update p
    }

    auto const w = cell_size*(range_x.distance() + 1);
    auto const h = cell_size*(range_y.distance() + 1);

    grid_t result(w, h, tile_category::empty);

    for (auto const p : points_) {
        auto const xb = (p.x - range_x.min) * cell_size;
        auto const yb = (p.y - range_y.min) * cell_size;

        for (auto yi = 0U; yi < cell_size; ++yi) {
            for (auto xi = 0U; xi < cell_size; ++xi) {
                result.at(xi + xb, yi + yb) = tile_category::floor;
            }
        }
    }

    return result;
}

tez::room
tez::compound_room_generator::generate() {
    auto result = make_compound_room_base_();

    static auto const EMPTY = tile_category::empty;

    auto const get_value = [](tile_category* p) {
        return p ? *p : EMPTY;
    };
    
    auto const ceiling_rule = [&](grid_block<tile_category>& b) {
        if (get_value(b.north()) == EMPTY) return true;
        if (get_value(b.south()) == EMPTY) return true;
        if (get_value(b.east())  == EMPTY) return true;
        if (get_value(b.west())  == EMPTY) return true;

        //all of nsew are not empty
        if (get_value(b.north_west()) == EMPTY) return true;
        if (get_value(b.north_east()) == EMPTY) return true;
        if (get_value(b.south_west()) == EMPTY) return true;
        if (get_value(b.south_east()) == EMPTY) return true;

        return false;
    };

    for (auto& b : result.block_iterator()) {
        BK_ASSERT(b.here() != nullptr);

        if (b.here() == nullptr || *b.here() == EMPTY) {
            continue;
        }
        
        auto& here = *b.here();

        if (ceiling_rule(b)) {
            here = tile_category::ceiling;
        } else if (get_value(b.north()) == tile_category::ceiling) {
            here = tile_category::wall;
        }
    }

    return room(std::move(result));
}

tez::compound_room_generator::connection_point
tez::compound_room_generator::find_connection_point(
    direction side,
    random_t  random
) {
    return connection_point(0, 0);
}

