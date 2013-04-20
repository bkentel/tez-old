#include "pch.hpp"

#include <gtest/gtest.h>

#include "../tile_map.hpp"
#include "../room_compound.hpp"
#include "../room_simple.hpp"
#include "../targa.hpp"


TEST(MapGen, Simple) {
    static auto const BUFFER = 1;

    typedef rect<signed>     rect_t;
    typedef point2d<signed> point_t;
    typedef std::pair<room_simple, rect_t> record_t;   

    std::vector<record_t>       placed_rooms;
    std::queue<record_t const*> candidates;

    std::default_random_engine gen(1984);

    ////////////////////////////////////////////////////////////////////////////
    auto const make_rect = [](room_simple const& room, signed x, signed y) {
        return rect_t(point_t(x, y), room.width(), room.height());
    };
    ////////////////////////////////////////////////////////////////////////////
    auto const emplace_room = [&](room_simple&& room, signed x, signed y) {       
        placed_rooms.emplace_back(
            std::move(room), make_rect(room, x, y)
        );

        candidates.push(&placed_rooms.back());
    };
    ////////////////////////////////////////////////////////////////////////////
    auto const relocate_rect_h = [](
        rect_t const& placed,
        rect_t const& intersection,
        rect_t&       room
    ) {
        auto const delta = BUFFER + intersection.width();

        if (intersection.left >= placed.left) {
            room.move_by(0+delta, 0);
        } else if (intersection.right <= placed.right) {
            room.move_by(0-delta, 0);
        } else {
            BK_ASSERT(false);
        }
    };
    ////////////////////////////////////////////////////////////////////////////
    auto const relocate_rect_v = [&](
        rect_t const& placed,
        rect_t const& intersection,
        rect_t&       room
    ) {
        auto const delta = BUFFER + intersection.height();

        if (intersection.top >= placed.top) {
            room.move_by(0, 0-delta);
        } else if (intersection.bottom <= placed.bottom) {
            room.move_by(0, 0+delta);
        } else {
            BK_ASSERT(false);
        }
    };
    ////////////////////////////////////////////////////////////////////////////
    auto const find_intersection = [&](
        rect_t const& test,
        rect_t&       other,
        rect_t&       intersection
    ) {       
        for (auto const& r : placed_rooms) {
            other        = r.second;
            intersection = intersection_of(test, other);
                
            if (intersection.is_rect()) return true;
        }

        return false;
    };
    ////////////////////////////////////////////////////////////////////////////
    auto const place_room = [&](room_simple&& room, rect_t const& relative_to) {
        auto const w = BUFFER + room.width();
        auto const h = BUFFER + room.height();

        auto const& r = relative_to;

        rect_t rects[] = {
            make_rect(room, r.right + w, r.top),
            make_rect(room, r.left  - w, r.top),
            make_rect(room, r.left,      r.bottom + h),
            make_rect(room, r.left,      r.top    - h),
        };

        std::uniform_int_distribution<size_t> dist(0, 3);

        auto const first = dist(gen);

        rect_t placed(0, 0, 0, 0);
        rect_t ir(0, 0, 0, 0);

        for (auto i = 0; i < 4; ++i) {
            auto& rect = rects[(i + first) % 4];
            
            while (find_intersection(rect, placed, ir)) {
                if (i >= 2) {
                    relocate_rect_h(placed, ir, rect);
                } else {
                    relocate_rect_v(placed, ir, rect);
                }
            }

            emplace_room(std::move(room), rect.left, rect.top);
            return;
        }
    };
    ////////////////////////////////////////////////////////////////////////////

    emplace_room(room_simple::generate(gen), 0, 0);

    while (placed_rooms.size() < 15) {       
        auto cand = candidates.front();
        candidates.pop();

        place_room(room_simple::generate(gen), cand->second);
    }


    image_targa image(200, 200, image_targa::image_type::rgb24);


    for (auto const& r : placed_rooms) {

        r.first.write(r.second.left + 50, r.second.top + 50,
            [&](unsigned x, unsigned y, room_part part) {
                switch (part) {
                case room_part::empty     : image.set(x, y, 0x00, 0x00, 0x00, 0x00); break;
                case room_part::floor     : image.set(x, y, 0xFF, 0x00, 0x00, 0x00); break;
                case room_part::v_edge    : image.set(x, y, 0x00, 0xFF, 0x00, 0x00); break;
                case room_part::h_edge    : image.set(x, y, 0x00, 0x00, 0xFF, 0x00); break;
                case room_part::corner_nw : image.set(x, y, 0xFF, 0xFF, 0x00, 0x00); break;
                case room_part::corner_ne : image.set(x, y, 0xFF, 0x00, 0xFF, 0x00); break;
                case room_part::corner_sw : image.set(x, y, 0x00, 0xFF, 0xFF, 0x00); break;
                case room_part::corner_se : image.set(x, y, 0xFF, 0xFF, 0xFF, 0x00); break;
                }
            }
        );
    }

    image.save("room.tga");
}

TEST(Rect, IntersectionValid) {
    typedef rect<signed> rect_t;

    static const signed S = 8;
    static const signed s = S / 2;

    rect_t const r0(-S, -S, S, S);
    rect_t const r1(-s, -s, s, s);
    
    EXPECT_EQ(r0.area(), 2*S*2*S);
    EXPECT_EQ(r1.area(), 2*s*2*s);

    EXPECT_TRUE(intersection_of(r0, r1).is_rect());
    EXPECT_EQ(intersection_of(r0, r1), r1);

    auto const test_f = [&] (rect_t const& test_rect, signed delta) {
        auto const rn  = intersection_of(r0, translate(test_rect,  0,     -delta));
        auto const rs  = intersection_of(r0, translate(test_rect,  0,      delta));
        auto const re  = intersection_of(r0, translate(test_rect,  delta,  0));
        auto const rw  = intersection_of(r0, translate(test_rect, -delta,  0));
        auto const rnw = intersection_of(r0, translate(test_rect, -delta, -delta));
        auto const rne = intersection_of(r0, translate(test_rect,  delta, -delta));
        auto const rsw = intersection_of(r0, translate(test_rect, -delta,  delta));
        auto const rse = intersection_of(r0, translate(test_rect,  delta,  delta));

        EXPECT_TRUE(rn.is_rect());
        EXPECT_TRUE(rs.is_rect());
        EXPECT_TRUE(re.is_rect());
        EXPECT_TRUE(rw.is_rect());
        EXPECT_TRUE(rnw.is_rect());
        EXPECT_TRUE(rne.is_rect());
        EXPECT_TRUE(rsw.is_rect());
        EXPECT_TRUE(rse.is_rect());

        EXPECT_EQ(rn.area(), rs.area());
        EXPECT_EQ(rn.area(), re.area());
        EXPECT_EQ(rn.area(), rw.area());

        EXPECT_EQ(rnw.area(), rne.area());
        EXPECT_EQ(rnw.area(), rsw.area());
        EXPECT_EQ(rnw.area(), rse.area());
    };

    test_f(r0, S);
    test_f(r1, s);
}

TEST(Range, IntersectionValid) {
    range<unsigned> const a(5, 10);
    range<unsigned> const b(0, 20);

    auto const ia = intersection_of(a, b);
    auto const ib = intersection_of(b, a);

    EXPECT_TRUE(ia.is_range());
    EXPECT_TRUE(ib.is_range());

    EXPECT_TRUE(a.intersects(b));
    EXPECT_TRUE(a.intersects(ia));
    EXPECT_TRUE(b.intersects(ia));

    EXPECT_EQ(ia, ib);
}

TEST(Range, IntersectionInvalid) {
    range<unsigned> const a(10, 5);
    range<unsigned> const b(20, 0);

    auto const ia = intersection_of(a, b);
    auto const ib = intersection_of(b, a);
    
    EXPECT_FALSE(ia.is_range());
    EXPECT_FALSE(ib.is_range());

    EXPECT_FALSE(a.intersects(b));
    EXPECT_FALSE(a.intersects(ia));
    EXPECT_FALSE(b.intersects(ia));

    EXPECT_EQ(ia, ib);
}

TEST(Range, Equality) {
    range<unsigned> const a(0,  10);
    range<unsigned> const b(10, 20);
    range<unsigned> const c = a;

    EXPECT_EQ(a, a);
    EXPECT_EQ(b, b);
    EXPECT_EQ(a, c);

    EXPECT_NE(a, b);
    EXPECT_NE(b, a);
    EXPECT_NE(b, c);
}

TEST(Range, ConstructValid) {
    range<unsigned> const r(0, 10);

    EXPECT_EQ(r.first,  0);
    EXPECT_EQ(r.last,   10);
    EXPECT_EQ(r.size(), 10);
    EXPECT_TRUE(r.is_range());   
}

TEST(Range, ConstructInvalid) {
    range<unsigned> const r(10, 0);

    EXPECT_EQ(r.first,  10);
    EXPECT_EQ(r.last,   0);
    EXPECT_EQ(r.size(), -10);
    EXPECT_FALSE(r.is_range());
}

TEST(MapGenerator, RoomCompound) {


    //image_targa image(100, 100, image_targa::image_type::rgb24);

    //std::default_random_engine gen(1984);

    //auto room1 = room_simple::generate(gen);
    //auto room2 = room_compound::generate(gen);   

    //room2.write(0, 0,
    //    [&](unsigned x, unsigned y, room_part part) {
    //        switch (part) {
    //        case room_part::empty     : image.set(x, y, 0x00, 0x00, 0x00, 0x00); break;
    //        case room_part::floor     : image.set(x, y, 0xFF, 0x00, 0x00, 0x00); break;
    //        case room_part::v_edge    : image.set(x, y, 0x00, 0xFF, 0x00, 0x00); break;
    //        case room_part::h_edge    : image.set(x, y, 0x00, 0x00, 0xFF, 0x00); break;
    //        case room_part::corner_nw : image.set(x, y, 0xFF, 0xFF, 0x00, 0x00); break;
    //        case room_part::corner_ne : image.set(x, y, 0xFF, 0x00, 0xFF, 0x00); break;
    //        case room_part::corner_sw : image.set(x, y, 0x00, 0xFF, 0xFF, 0x00); break;
    //        case room_part::corner_se : image.set(x, y, 0xFF, 0xFF, 0xFF, 0x00); break;
    //        }
    //    }
    //);

    //image.save("room.tga");
}

TEST(TileIterator, IteratorDistance) {
    static auto const w = 10;
    static auto const h = 10;

    tile_grid grid(w, h);
    
    {
        auto const region = grid.csub_region(0, 0, w, h);

        EXPECT_EQ(
            std::distance(region.cbegin(), region.cend()),
            grid.width() * grid.height()
        );
    }

    {
        auto const region = grid.csub_region(1, 1, 1, 1);

        EXPECT_EQ(
            std::distance(region.cbegin(), region.cend()),
            1
        );
    }

}

TEST(TileIterator, Iterator) {
    static auto const w = 10;
    static auto const h = 10;

    tile_grid grid(w, h);

    static auto const xo = 1;
    static auto const yo = 1;
    static auto const ws = 3;
    static auto const hs = 3;

    auto region = grid.sub_region(xo, yo, ws, hs);

    unsigned const values[][hs] = {
        0x100, 0x101, 0x102,
        0x110, 0x111, 0x112,
        0x120, 0x121, 0x122,
    };

    for (unsigned y = 0; y < hs; ++y) {
        for (unsigned x = 0; x < ws; ++x) {
            region(x, y).texture_id = values[x][y];
        }
    }

    // use an iterator to check the values
    auto it = region.cbegin();
    for (unsigned y = 0; y < hs; ++y) {
        for (unsigned x = 0; x < ws; ++x) {
            EXPECT_EQ(values[x][y], it->texture_id);
            ++it;
        }
    }

    // directly index into the tile_grid to check the values
    for (unsigned y = yo; y < yo + hs; ++y) {
        for (unsigned x = xo; x < xo + ws; ++x) {
            EXPECT_EQ(values[x - xo][y - yo], grid(x, y).texture_id);
        }
    }
}

TEST(TileGrid, Dimensions) {
    // Test the dimension functions.

    static auto const w = 10;
    static auto const h = 10;

    tile_grid grid(w, h);

    EXPECT_EQ(w, grid.width());
    EXPECT_EQ(h, grid.height());
}

TEST(TileGrid, Constructor) {
    EXPECT_THROW(tile_grid(0, 0), assertion_failure);
    EXPECT_THROW(tile_grid(0, 1), assertion_failure);
    EXPECT_THROW(tile_grid(1, 0), assertion_failure);
}

TEST(TileGrid, FunctionOperator) {
    static auto const w = 10;
    static auto const h = 10;

    tile_grid grid(w, h);

    EXPECT_THROW(grid(w, h), assertion_failure);
    EXPECT_THROW(grid(w, 0), assertion_failure);
    EXPECT_THROW(grid(0, h), assertion_failure);
}

TEST(TileGrid, SubRegion) {
    static auto const w = 10;
    static auto const h = 10;

    tile_grid grid(w, h);

    EXPECT_THROW(grid.sub_region(w, h, 1,   1), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, h, 1,   1), assertion_failure);
    EXPECT_THROW(grid.sub_region(w, 0, 1,   1), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, 0,   0), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, w,   0), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, 0,   h), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, w+1, h), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, w,   h+1), assertion_failure);
    EXPECT_THROW(grid.sub_region(0, 0, w+1, h+1), assertion_failure);
}

TEST(TileRegion, RegionAssign) {
    // Test the assigment operator.

    tile_grid grid(10, 10);

    auto r1 = grid.sub_region(1, 1, 9, 9);
    auto r2 = grid.sub_region(2, 2, 8, 8);
    auto r3 = grid.csub_region(3, 3, 7, 7);
    auto r4 = grid.csub_region(4, 4, 6, 6);

    // mutable to mutable
    r1 = r2;
    EXPECT_EQ(r1, r2);

    // mutable to const
    r3 = r1;
    EXPECT_EQ(r3, r1);

    // const to const
    r4 = r3;
    EXPECT_EQ(r4, r3);
}

TEST(TileRegion, RegionCopyConstr) {
    // Test the copy constructor.

    tile_grid grid(10, 10);

    // mutable to mutable
    auto r1 = grid.sub_region(1, 1, 9, 9);
    tile_grid::region r2(r1);
    EXPECT_EQ(r1, r2);

    // const to const
    auto r3 = grid.csub_region(3, 3, 7, 7);
    tile_grid::const_region r4(r3);
    EXPECT_EQ(r3, r4);

    // mutable to const
    tile_grid::const_region r5(r1);
    EXPECT_EQ(r1, r5);
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    RUN_ALL_TESTS();

    return 0;
}
