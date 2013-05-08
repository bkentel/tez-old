#include "pch.hpp"
#include "room_generator.hpp"

#include "geometry.hpp"
#include "map.hpp"

#include <vector>

simple_room_generator::simple_room_generator(random_t random)
    : generator(random)
{
}

room simple_room_generator::generate() {
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

    return room(std::move(result));
}

compound_room_generator::compound_room_generator(random_t random)
    : generator(random)
{
}

generator::grid_t compound_room_generator::make_compound_room_base_() {
    typedef std::uniform_int_distribution<unsigned> distribution_t;

    static auto const CELL_SIZE_MIN = 4;
    static auto const CELL_SIZE_MAX = 6;
    static auto const COUNT_MIN     = 10;
    static auto const COUNT_MAX     = 20;

    auto const cell_size  = distribution_t(CELL_SIZE_MIN, CELL_SIZE_MAX)(random_);
    auto const cell_count = distribution_t(COUNT_MIN, COUNT_MAX)(random_);   

    min_max<signed> range_x;
    min_max<signed> range_y;

    distribution_t dist(0, 3);

    //--------------------------------------------------------------------------
    // true if the position given by p has already been used.
    //--------------------------------------------------------------------------
    auto const is_occupied = [&](point_t const p) {
        return std::find(
            std::begin(points_), std::end(points_), p
        ) != std::end(points_);
    };
    //--------------------------------------------------------------------------
    // get a point offset in a cardinal direction from p
    //--------------------------------------------------------------------------
    auto const get_point = [&](point_t const p) -> point_t {
        auto const base   = dist(random_);
        auto       result = p;

        for (unsigned i = 0; (i < 4) && is_occupied(result); ++i) {
            switch ((base + i) % 4) {
            case 0 : result = point_t(p.x + 1, p.y + 0); break;
            case 1 : result = point_t(p.x - 1, p.y + 0); break;
            case 2 : result = point_t(p.x + 0, p.y + 1); break;
            case 3 : result = point_t(p.x + 0, p.y - 1); break;
            }
        }

        return result;
    };
    //--------------------------------------------------------------------------
    // add a point and update the x and y range
    //--------------------------------------------------------------------------
    auto const add_point = [&](point_t const p) {
        range_x(p.x);
        range_y(p.y);
        points_.push_back(p);
    };

    points_.clear();
    points_.reserve(cell_count);
    
    point_t p(0, 0); //current position
    add_point(p);

    while (points_.size() < cell_count) {
        auto const q = get_point(p);
        if (p == q) {
            //failed to move
            break;
        }
            
        //add the point and update p
        add_point(p = q);
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

room compound_room_generator::generate() {
    auto result = make_compound_room_base_();

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
        BK_ASSERT(b.here() != nullptr);

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

    return room(std::move(result));
}

namespace {

unsigned get_probabilities(unsigned dir, unsigned index) {
    struct probability_list {
        unsigned values[4];
    };

    static probability_list const path_probabilities[] = {
        {800, 10, 20, 20}, //north
        {10, 800, 20, 20}, //south
        {20, 20, 800, 10}, //east
        {20, 20, 10, 800}, //west
    };

    BK_ASSERT(dir < 4);
    BK_ASSERT(index < 4);

    return path_probabilities[dir].values[index];
};

static auto get_prob_n = std::bind(get_probabilities, 0, std::placeholders::_1);
static auto get_prob_s = std::bind(get_probabilities, 1, std::placeholders::_1);
static auto get_prob_e = std::bind(get_probabilities, 2, std::placeholders::_1);
static auto get_prob_w = std::bind(get_probabilities, 3, std::placeholders::_1);

static auto const path_prob_n = std::discrete_distribution<unsigned>(4, 0, 4, get_prob_n);
static auto const path_prob_s = std::discrete_distribution<unsigned>(4, 0, 4, get_prob_s);
static auto const path_prob_e = std::discrete_distribution<unsigned>(4, 0, 4, get_prob_e);
static auto const path_prob_w = std::discrete_distribution<unsigned>(4, 0, 4, get_prob_w);

} //namespace

path_generator::path_generator(random_wrapper<unsigned> random)
    : path_dist_n_(4, 0, 4, get_prob_n)
    , path_dist_s_(4, 0, 4, get_prob_s)
    , path_dist_e_(4, 0, 4, get_prob_e)
    , path_dist_w_(4, 0, 4, get_prob_w)
    , random_(random)
{
}

bool path_generator::generate(room const& origin, map const& m, direction const dir) {
    auto const map_w = m.width();
    auto const map_h = m.height();

    //--------------------------------------------------------------------------
    // Get a random unit vector
    //--------------------------------------------------------------------------
    auto const get_random_vector = [&](distribution_t const& dist) -> point2d<signed> {
        signed const dir_x[] = { 0, 0, 1, -1};
        signed const dir_y[] = {-1, 1, 0,  0};

        auto const i = dist(random_);
        
        return point2d<signed>(dir_x[i], dir_y[i]);
    };
    //--------------------------------------------------------------------------
    auto const is_in_bounds = [&](unsigned const x, unsigned const y) {
        return (x >= 0) && (y >= 0) && (x < map_w) && (y < map_h);
    };
    //--------------------------------------------------------------------------
    auto const is_pathable = [&](unsigned const x, unsigned const y) {
        auto const c = m.at(x, y).type;
        return (c == tile_category::corridor) || (c == tile_category::empty);
    };
    //--------------------------------------------------------------------------
    auto const is_connectable = [&](unsigned const x, unsigned const y) -> bool {
        auto const& b = m.block_at(x, y);

        auto const n = b.north() ? b.north()->type : tile_category::empty;
        auto const s = b.south() ? b.south()->type : tile_category::empty;
        auto const e = b.east()  ? b.east()->type  : tile_category::empty;
        auto const w = b.west()  ? b.west()->type  : tile_category::empty;

        static auto const C = tile_category::ceiling;
        auto const check = [&](tile_category const c) {
            return c == tile_category::ceiling || c == tile_category::wall;
        };

        return (n == C && s == C) ^ (e == C && w == C) &&
            (check(e) || check(w) || check(n) || check(s));

        //return true;
    };
    //--------------------------------------------------------------------------
    auto const is_on_path = [&](unsigned const x, unsigned const y) {
        return std::find_if(path_.cbegin(), path_.cend(), [&](point_t const p) {
            return p.x == x && p.y == y;
        }) != path_.cend();
    };
    //--------------------------------------------------------------------------
    auto const bounds = origin.bounds();
    auto const is_in_origin = [&](unsigned const x, unsigned const y) {
        return bounds.contains(static_cast<signed>(x), static_cast<signed>(y));
    };
    //--------------------------------------------------------------------------
    auto p = origin.find_connectable_point(random_, dir);
    BK_ASSERT(is_in_bounds(p.x, p.y));
    auto const check_val = m.at(p.x, p.y).type;
    BK_ASSERT(check_val != tile_category::empty && check_val != tile_category::floor);

    distribution_t const& dist =
        (dir == direction::north ) ? path_prob_n :
        (dir == direction::south ) ? path_prob_s :
        (dir == direction::east )  ? path_prob_e :
        (dir == direction::west )  ? path_prob_w : path_prob_w;

    path_.clear();
    path_.push_back(p);

    unsigned fail_count = 0;
    bool     found_path = false;

    while (!found_path && fail_count < 10) {
        auto const v = get_random_vector(dist);
        auto const x = p.x + v.x;
        auto const y = p.y + v.y;

        if (!is_in_bounds(x, y)) {
            fail_count++;
            continue;
        }

        if (!is_pathable(x, y)) {
            if (is_in_origin(x, y) || !is_connectable(x, y) || is_on_path(x, y)) {
                fail_count++;
                continue;
            }
             
             found_path = true;
        }

        path_.emplace_back(x, y);
        p.x = x;
        p.y = y;
    }

    if (!found_path || path_.size() < 2) {
        return false;
    }

    return true;
}

void path_generator::write_path(map& out) {
    BK_ASSERT(path_.size() >= 2);

    auto const path = [&](unsigned const i) -> tile_category& {
        return out.at(path_[i].x, path_[i].y).type;
    };

    unsigned i = 0;

    path(i++) = tile_category::door_start;
    while (i < path_.size() - 1) {
        path(i++) = tile_category::corridor;
    }
    path(i) = tile_category::door_end;

    path_.clear();
}
