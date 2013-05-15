#include "pch.hpp"
#include "room_generator.hpp"

//==============================================================================
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
    
    for (auto& block : result.block_iterator()) {
        auto const x = block.x;
        auto const y = block.y;

        if ((x == 0)   || (y == 0) ||
            (x == w-1) || (y == h-1)
        ) {
            result.at(x, y) = tile_category::ceiling;
        } else if(*block.north() == tile_category::ceiling) {
            result.at(x, y) = tile_category::wall;
        }
    }

    return room(std::move(result), find_connection_point);
}

tez::simple_room_generator::connection_point
tez::simple_room_generator::find_connection_point(
    room      const& room,
    direction const  side,
    random_t         random
) {
    typedef std::uniform_int_distribution<unsigned> distribution_t;

    auto const w = room.width() - 1;
    auto const h = room.height() - 1;

    unsigned const x =
        (side == direction::west) ? 0 :
        (side == direction::east) ? w :
        distribution_t(1, w-1)(random);

    unsigned const y =
        (side == direction::north) ? 0 :
        (side == direction::south) ? h :
        distribution_t(2, h-1)(random);

    BK_ASSERT(room.at(x, y) == tile_category::ceiling);

    return connection_point(x + room.left(), y + room.top());
}

tez::compound_room_generator::compound_room_generator(random_t random)
    : generator(random)
{
}

//==============================================================================
namespace {

typedef bklib::random_wrapper<>         random_t;
typedef bklib::point2d<signed>          point_t;
typedef std::vector<point_t>            point_list;
typedef tez::grid2d<tez::tile_category> grid_t;

std::tuple<unsigned, bklib::min_max<>, bklib::min_max<>>
generate_points(
    point_list& out,
    random_t    random
) {
    typedef std::uniform_int_distribution<unsigned> distribution_t;

    BK_DECLARE_DIRECTION_ARRAYS(dx, dy);

    static auto const CELL_SIZE_MIN = 4;
    static auto const CELL_SIZE_MAX = 6;
    static auto const COUNT_MIN     = 10;
    static auto const COUNT_MAX     = 20;

    auto const cell_size  = distribution_t(CELL_SIZE_MIN, CELL_SIZE_MAX)(random);
    auto const cell_count = distribution_t(COUNT_MIN, COUNT_MAX)(random);   

    bklib::min_max<> range_x;
    bklib::min_max<> range_y;

    //--------------------------------------------------------------------------
    // Add a point and update the x and y range
    //--------------------------------------------------------------------------
    auto const add_point = [&](point_t const p) {
        range_x(p.x);
        range_y(p.y);
        out.push_back(p);
    };
    //----------------------------------------------------------------------
    auto const is_occupied = [&](point_t const p) {
        return std::cend(out) !=
            std::find(std::cbegin(out), std::cend(out), p);
    };
    //----------------------------------------------------------------------
    auto const choose_point = [&](point_t const p) -> std::pair<bool, point_t> {
        using namespace tez;

        auto const base = distribution_t(0, NUM_CARDINAL_DIR-1)(random);

        for (unsigned i = 0; i < NUM_CARDINAL_DIR; ++i) {
            unsigned const j = (base + i) % NUM_CARDINAL_DIR;
            auto     const q = point_t(p.x + dx[j], p.y + dy[j]);
            
            if (!is_occupied(q)) return std::make_pair(true, q);
        }

        return std::make_pair(false, p);
    };
    //----------------------------------------------------------------------

    out.clear();
    out.reserve(cell_count);

    auto p = point_t(0, 0); //current position

    for (bool found = true; found && out.size() < cell_count;) {
        add_point(p);
        std::tie(found, p) = choose_point(p);
    }

    return std::make_tuple(cell_size, range_x, range_y);
}

grid_t points_to_grid(
    point_list       const& points,
    unsigned         const  cell_size,
    bklib::min_max<> const  range_x,
    bklib::min_max<> const  range_y
) {
    auto const w = cell_size*(range_x.distance() + 1);
    auto const h = cell_size*(range_y.distance() + 1);
        
    auto result = grid_t(w, h, tez::tile_category::empty);

    for (auto const p : points) {
        auto const xb = (p.x - range_x.min) * cell_size;
        auto const yb = (p.y - range_y.min) * cell_size;

        for (auto yi = 0u; yi < cell_size; ++yi) {
            for (auto xi = 0u; xi < cell_size; ++xi) {
                result.at(xi + xb, yi + yb) = tez::tile_category::floor;
            }
        }
    }

    return result;
}

void transform_grid(grid_t& grid) {
    typedef tez::grid_block<tez::tile_category> block_t;

    static auto const EMPTY = tez::tile_category::empty;
    static auto const CEIL  = tez::tile_category::ceiling;
    static auto const WALL  = tez::tile_category::wall;

    auto const get_value = [](tez::tile_category* p) {
        return p ? *p : EMPTY;
    };
    
    auto const ceiling_rule = [&](block_t& block) {
        if (get_value(block.north()) == EMPTY) return true;
        if (get_value(block.south()) == EMPTY) return true;
        if (get_value(block.east())  == EMPTY) return true;
        if (get_value(block.west())  == EMPTY) return true;

        //all of nsew are not empty
        if (get_value(block.north_west()) == EMPTY) return true;
        if (get_value(block.north_east()) == EMPTY) return true;
        if (get_value(block.south_west()) == EMPTY) return true;
        if (get_value(block.south_east()) == EMPTY) return true;

        return false;
    };

    for (auto& block : grid.block_iterator()) {
        auto here = block.here();
        if (!here || *here == EMPTY) continue;

        if (ceiling_rule(block))                   *here = CEIL;
        else if (get_value(block.north()) == CEIL) *here = WALL;
    }
}

} //namespace

tez::room
tez::compound_room_generator::generate() {
    auto const points_info = generate_points(points_, random_);
    
    auto grid = points_to_grid(points_,
        std::get<0>(points_info),
        std::get<1>(points_info),
        std::get<2>(points_info)
    );

    transform_grid(grid);

    return room(std::move(grid), find_connection_point);
}

tez::compound_room_generator::connection_point
tez::compound_room_generator::find_connection_point(
    room      const& room,
    direction const  side,
    random_t         random
) {
    static auto const TARGET = tile_category::ceiling;

    auto const search_dir = opposite_direction(side);
    
    auto const w = room.width()  - 1;
    auto const h = room.height() - 1;

    unsigned x = (search_dir == direction::west) ? w :
                 (search_dir == direction::east) ? 0 :
                  std::uniform_int_distribution<>(0, w)(random);

    unsigned y = (search_dir == direction::north) ? h :
                 (search_dir == direction::south) ? 0 :
                  std::uniform_int_distribution<>(0, h)(random);

    auto const is_ns =
        (search_dir == direction::north || search_dir == direction::south);

    auto&      primary        = is_ns ? y : x;
    auto&      secondary      = is_ns ? x : y;
    auto const primary_size   = is_ns ? room.height() : room.width();
    auto const secondary_size = is_ns ? room.width()  : room.height();
    
    auto const delta = is_ns ? direction_vector(search_dir).second :
                               direction_vector(search_dir).first;

    //--------------------------------------------------------------------------
    auto const is_in_bounds = [&] {
        return (delta > 0 && primary < primary_size) ||
               (delta < 0 && primary > 0);
    };
    //--------------------------------------------------------------------------
    auto const is_valid_pos = [&] {
        BK_ASSERT(x <= w && y <= h);
        
        if (room.at(x, y) != TARGET) {
            return false;
        }

        primary += delta;
        BK_ON_SCOPE_EXIT({ primary -= delta; });
        
        auto const is_floor = [&](tile_category const c) {
            return c == tile_category::floor;
        };
        auto const is_wall = [&](tile_category const c) {
            return c == tile_category::wall && search_dir == direction::south;
        };

        auto const c = room.at(x, y);
        return is_floor(c) || is_wall(c);
    };
    //--------------------------------------------------------------------------

    //scan along the primary direction
    while (is_in_bounds() && (room.at(x, y) != TARGET)) {
        primary += delta;
    }
    BK_ASSERT(room.at(x, y) == TARGET);
                
    //scan along the secondary direction
    while (!is_valid_pos()) {
        secondary = (secondary + 1) % secondary_size;
    }
    BK_ASSERT(room.at(x, y) == TARGET);

    return connection_point(x + room.left(), y + room.top());
}

