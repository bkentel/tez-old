#include "pch.hpp"
#include "room_generator.hpp"

#include "geometry.hpp"

#include <vector>

unsigned const path_generator_dist<direction::north>::values[4] = {
    80, 10, 20, 20
};

unsigned const path_generator_dist<direction::south>::values[4] = {
    10, 80, 20, 20
};

unsigned const path_generator_dist<direction::east>::values[4] = {
    20, 20, 80, 10
};

unsigned const path_generator_dist<direction::west>::values[4] = {
    20, 20, 10, 80
};

/////

simple_room_generator::simple_room_generator(random_t random)
    : generator(random)
{
}

generator::grid_t simple_room_generator::generate() {
    static auto const MIN_W = 3;
    static auto const MAX_W = 10;
    static auto const MIN_H = 4;
    static auto const MAX_H = 10;

    typedef std::uniform_int_distribution<unsigned> dist_t;

    auto const w = dist_t(MIN_W, MAX_W)(random_);
    auto const h = dist_t(MIN_H, MAX_H)(random_);

    grid_t result(w, h, tile_category::floor);
    
    for (auto& b : result.block_iterator()) {
        auto const x = b.x;
        auto const y = b.y;

        if (
            (x == 0) ||
            (y == 0) ||
            (x == w - 1) ||
            (y == h - 1)
        ) {
            result.at(x, y) = tile_category::ceiling;
        } else if(*b.north() == tile_category::ceiling) {
            result.at(x, y) = tile_category::wall;
        }
    }

    return result;
}

compound_room_generator::compound_room_generator(random_t random)
    : generator(random)
{
}

namespace {

generator::grid_t make_compound_room_base(generator::random_t random) {
    static auto const CELL_SIZE_MIN = 4;
    static auto const CELL_SIZE_MAX = 6;
    static auto const COUNT_MIN     = 10;
    static auto const COUNT_MAX     = 20;

    typedef point2d<signed> point_t;
    
    std::vector<point_t> points;  //list of occupied points
    point_t              p(0, 0); //current position

    typedef std::uniform_int_distribution<unsigned> dist_t;

    auto const cell_size  = dist_t(CELL_SIZE_MIN, CELL_SIZE_MAX)(random);
    auto const cell_count = dist_t(COUNT_MIN, COUNT_MAX)(random);   

    //true if the position given by p has already been used.
    auto const is_occupied = [&](point_t const p) {
        return std::find(
            std::begin(points), std::end(points), p
        ) != std::end(points);
    };
    
    //get a point offset in a cardinal direction from p
    auto const get_point = [](point_t const p, unsigned const index) -> point_t {
        switch (index) {
        case 0 : return point_t(p.x + 1, p.y + 0);
        case 1 : return point_t(p.x - 1, p.y + 0);
        case 2 : return point_t(p.x + 0, p.y + 1);
        case 3 : return point_t(p.x + 0, p.y - 1);
        }

        return p;
    };

    min_max<signed> range_x;
    min_max<signed> range_y;

    //uniform distribution over [0, 3].
    dist_t dist(0, 3);

    points.reserve(cell_count);

    while (points.size() < cell_count) {
        auto const i = dist(random);

        auto const p0 = get_point(p, (i + 0) % 4);
        auto const p1 = get_point(p, (i + 1) % 4);
        auto const p2 = get_point(p, (i + 2) % 4);
        auto const p3 = get_point(p, (i + 3) % 4);

        auto const q = 
            !is_occupied(p0) ? p0 :
            !is_occupied(p1) ? p1 :
            !is_occupied(p2) ? p2 :
            !is_occupied(p3) ? p3 : p;

        //no move was possible -- stop looping
        if (q == p) {
            break;
        }
            
        //add the point and update p
        points.push_back(p = q);
        
        range_x(p.x);
        range_y(p.y);
    }

    auto const w = range_x.distance() + 1;
    auto const h = range_y.distance() + 1;

    generator::grid_t result(w*cell_size, h*cell_size, tile_category::empty);

    for (auto const p : points) {
        auto const xb = (p.x - range_x.min) * cell_size;
        auto const yb = (p.y - range_y.min) * cell_size;

        for (auto yi = 0U; yi < cell_size; ++yi) {
            for (auto xi = 0U; xi < cell_size; ++xi) {
                auto const x = xi + xb;
                auto const y = yi + yb;

                result.at(x, y) = tile_category::floor;
            }
        }
    }

    return result;
}

} //namespace

generator::grid_t compound_room_generator::generate() {
    auto result = make_compound_room_base(random_);

    static auto const empty = tile_category::empty;

    auto const get_value = [](tile_category* p) {
        return p ? *p : empty;
    };
    
    auto const ceiling_rule = [&](grid_block<tile_category>& b) {
        if (get_value(b.north()) == empty) return true;
        if (get_value(b.south()) == empty) return true;
        if (get_value(b.east())  == empty) return true;
        if (get_value(b.west())  == empty) return true;

        //all of nsew are not empty
        if (get_value(b.north_west()) == empty) return true;
        if (get_value(b.north_east()) == empty) return true;
        if (get_value(b.south_west()) == empty) return true;
        if (get_value(b.south_east()) == empty) return true;

        return false;
    };

    for (auto& b : result.block_iterator()) {
        if (b.here() == nullptr || *b.here() == empty) {
            continue;
        }
        
        auto& here = *b.here();

        if (ceiling_rule(b)) {
            here = tile_category::ceiling;
        } else if (get_value(b.north()) == tile_category::ceiling) {
            here = tile_category::wall;
        }
    }

    return result;
}
