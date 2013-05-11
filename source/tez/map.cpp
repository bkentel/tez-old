#include "pch.hpp"
#include "map.hpp"

namespace {
    static tez::tile_data default_tile = {
        tez::tile_category::empty,
        {0},
        0, 0, 0,
        0
    };

} //namespace

tez::map::map(unsigned width, unsigned height)
    : data_(width, height, default_tile)
{
}

void tez::map::add_room(room const& r, signed dx, signed dy) {
    grid_copy_transform(
        r,
        0, 0,
        r.width(), r.height(),
        data_,
        r.left() + dx, r.top() + dy,
        [](tile_category const& src_cat, tile_data& dest_data) {
            dest_data.type = src_cat;
        }
    );
}

std::ostream& tez::operator<<(std::ostream& out, tez::map const& m) {
    out << "map";

    auto const h = m.height();
    auto const w = m.width();

    for (unsigned y = 0; y < h; ++y) {
        std::cout << std::endl;

        for (unsigned x = 0; x < w; ++x) {
            std::cout << static_cast<char>(m.data_.at(x, y).type);
        }
    }
    
    std::cout << std::endl;

    return out;
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

tez::path_generator::path_generator(bklib::random_wrapper<> random)
    : path_dist_n_(4, 0, 4, get_prob_n)
    , path_dist_s_(4, 0, 4, get_prob_s)
    , path_dist_e_(4, 0, 4, get_prob_e)
    , path_dist_w_(4, 0, 4, get_prob_w)
    , random_(random)
{
}

bool tez::path_generator::generate(
    tez::room const& origin,
    tez::map  const& map,
    direction const  dir
) {
    auto const map_w = map.width();
    auto const map_h = map.height();
    
    auto const bounds = origin.bounds();
    //--------------------------------------------------------------------------
    // Get a random unit vector
    //--------------------------------------------------------------------------
    BK_DECLARE_DIRECTION_ARRAYS(dir_x, dir_y);
    
    auto const get_random_vector = [&](distribution_t const& dist) {
        auto const i = dist(random_);
        return bklib::make_point(dir_x[i], dir_y[i]);
    };
    //--------------------------------------------------------------------------
    auto const is_in_bounds = [&](unsigned const x, unsigned const y) {
        return (x >= 0) && (y >= 0) && (x < map_w) && (y < map_h);
    };
    //--------------------------------------------------------------------------
    auto const is_pathable = [&](unsigned const x, unsigned const y) {
        auto const c = map.at(x, y).type;
        return (c == tile_category::corridor) || (c == tile_category::empty);
    };
    //--------------------------------------------------------------------------
    auto const is_connectable = [&](unsigned const x, unsigned const y) -> bool {
        auto const& b = map.block_at(x, y);

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
    };
    //--------------------------------------------------------------------------
    auto const is_on_path = [&](unsigned const x, unsigned const y) {
        return std::find_if(path_.cbegin(), path_.cend(), [&](point_t const p) {
            return p.x == x && p.y == y;
        }) != path_.cend();
    };
    //--------------------------------------------------------------------------
    auto const is_in_origin = [&](unsigned const x, unsigned const y) {
        return bklib::intersects(bounds, bklib::make_point(x, y));
    };
    //--------------------------------------------------------------------------
    auto p = origin.find_connection_point(dir, random_);
    BK_ASSERT(is_in_bounds(p.x, p.y));
    auto const check_val = map.at(p.x, p.y).type;
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

void tez::path_generator::write_path(map& out) {
    BK_ASSERT(path_.size() >= 2);
 
    for (unsigned i = 1; i < path_.size() - 1; ++i) {
        auto& tile = out.at(path_[i].x, path_[i].y);
        tile.type = tile_category::corridor;
    }

    auto& first = out.at(path_.front().x, path_.front().y);
    auto& last  = out.at(path_.back().x, path_.back().y);

    first.type = last.type = tile_category::door;
    first.get_data<door_data>().state =
        last.get_data<door_data>().state = door_data::door_state::open;

    path_.clear();
}
