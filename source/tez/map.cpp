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
            auto const& tile = m.data_.at(x, y);
            auto const  type = tile.type;

            auto out_char = static_cast<char>(type);

            if (type == tile_category::door) {
                out_char = tile.get_data<door_data>().state == door_data::door_state::open ? 'O' : 'C';
            }

            std::cout << out_char;
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
    typedef room::connection_point point_t;

    static unsigned const MAX_PATH_FAILURES       = 5;
    static unsigned const MAX_FIND_START_FAILURES = 5;
    
    auto const temp = origin.bounds();
    BK_ASSERT(temp.left >= 0);
    BK_ASSERT(temp.top >= 0);
    auto const bounds = static_cast<bklib::rect<unsigned>>(temp);

    BK_DECLARE_DIRECTION_ARRAYS(dir_x, dir_y);
    //--------------------------------------------------------------------------
    // Get a random unit vector
    //--------------------------------------------------------------------------    
    auto const get_random_vector = [&](distribution_t const& dist) {
        auto const i = dist(random_);
        return bklib::make_point(dir_x[i], dir_y[i]);
    };
    //--------------------------------------------------------------------------
    auto const is_pathable = [](tile_category const type) {
        return (type == tile_category::corridor) ||
               (type == tile_category::empty);
    };
    //--------------------------------------------------------------------------
    auto const is_connectable = [&](map::const_block const block) -> bool {
        auto const get = [](tile_data const* tile) {
            return tile ? tile->type : tile_category::empty;
        };

        static auto const CEIL  = tile_category::ceiling;
        static auto const FLOOR = tile_category::floor;
        static auto const WALL  = tile_category::wall;

        if (get(block.here()) != CEIL) {
            return false;
        }

        auto const n = get(block.north());
        auto const s = get(block.south());
        auto const e = get(block.east());
        auto const w = get(block.west());

        return (n == CEIL && s == CEIL && (e == FLOOR || w == FLOOR)) ||
               (e == CEIL && w == CEIL && (n == FLOOR || s == WALL));
    };
    //--------------------------------------------------------------------------
    auto const is_on_path = [&](point_t const p) {
        return path_.cend() != std::find_if(
            path_.cbegin(),
            path_.cend(), [&](point_t const q) { return p == q; }
        );
    };
    //--------------------------------------------------------------------------
    auto const is_in_origin = [&](point_t const p) {
        return bklib::intersects(bounds, p);
    };
    //--------------------------------------------------------------------------
    auto const find_path_start = [&]() -> std::pair<bool, point_t> {
        auto const check = [](tile_data const* tile) {
            return tile ? tile->type != tile_category::door : true;
        };

        for (unsigned i = 0; i < MAX_FIND_START_FAILURES; ++i) {
            auto const  p     = origin.find_connection_point(dir, random_);           
            auto const  type  = map.at(p).type;
            auto const& block = map.block_at(p);
                 
            if ((type == tile_category::ceiling) &&
                check(block.north()) && check(block.south()) &&
                check(block.east())  && check(block.west())
            ) {
                return std::make_pair(true, p);
            }
        }
        
        return std::make_pair(false, room::connection_point(0, 0));
    };
    //--------------------------------------------------------------------------
    auto const path_start = find_path_start();
    if (!path_start.first) {
        return false;
    }
    
    auto pos = path_start.second;

    distribution_t const& dist =
        (dir == direction::north ) ? path_prob_n :
        (dir == direction::south ) ? path_prob_s :
        (dir == direction::east )  ? path_prob_e :
        (dir == direction::west )  ? path_prob_w : path_prob_w;

    path_.clear();
    path_.push_back(pos);

    bool found_path = false;

    for (unsigned i = 1; !found_path && i < MAX_FIND_START_FAILURES; ++i) {
        auto const p = bklib::translate_by(pos, get_random_vector(dist));

        if (!map.is_valid_position(p)) {
            continue;
        } else if (!is_pathable(map.at(p).type)) {
            if (is_in_origin(p)) {
                continue;
            } else if (!is_connectable(map.block_at(p))) {
                continue;
            } else if (is_on_path(p)) {
                continue;
            } else {
                found_path = true;
            }
        }

        path_.emplace_back((pos = p));
        i--;
    }

    return found_path && path_.size() >= 2;
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
    first.get_data<door_data>().state = door_data::door_state::open;
    last.get_data<door_data>().state  = door_data::door_state::closed;

    path_.clear();
}
