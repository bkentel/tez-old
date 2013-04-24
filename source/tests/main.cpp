#include "pch.hpp"
#include <gtest/gtest.h>


#include "geometry.hpp"
#include "../tile_map.hpp"
#include "../targa.hpp"


//struct room_set {
//    typedef rect<signed>                     rect_t;
//    typedef point2d<signed>                  point_t;
//    typedef std::unique_ptr<room>            unique_room;
//    typedef std::pair<unique_room, rect_t>   record_t;  
//    
//    struct intersection_t {
//        bool intersects() const {
//            return intersection.is_rect();
//        }
//
//        rect_t intersection;
//        rect_t intersecting_rect;
//    };
//
//    static auto const BUFFER_SIZE = 3;
//
//    static rect_t make_rect(room const& r, signed x, signed y) {
//        return rect_t(point_t(x, y), r.width(), r.height());
//    }
//
//    void add_room(unique_room room, rect_t const rect) {
//        BK_ASSERT(room->width() == rect.width());
//        BK_ASSERT(room->height() == rect.height());
//
//        std::cout << "add_room at " << rect  << std::endl;
//
//        rooms.emplace_back(
//            std::make_pair(std::move(room), rect)
//        );
//    }
//    
//    intersection_t get_intersection(rect_t const room_rect) const {
//        intersection_t result = {room_rect, room_rect};
//        
//        for (auto const& r : rooms) {
//            result.intersection = intersection_of(room_rect, r.second);
//
//            if (result.intersection.is_rect()) {
//                std::cout << "intersection\n"
//                    << "  room     " << room_rect << "\n"
//                    << "  existing " << r.second  << "\n"
//                    << "  result   " << result.intersection
//                    << std::endl;
//
//                result.intersecting_rect = r.second;
//                break;
//            }
//        }
//
//        return result;
//    }
//
//    static rect_t relocate_rect_x(rect_t const room, intersection_t const i) {
//        auto const delta = static_cast<signed>(BUFFER_SIZE + i.intersection.width());
//
//        if (i.intersection.area() == room.area()) {
//            return room;//BK_ASSERT(false);
//        } else if (i.intersection.area() == i.intersecting_rect.area()) {
//            return room;//BK_ASSERT(false);
//        } else if (i.intersection.left >= i.intersecting_rect.left) {
//            std::cout << "relocate_x by " << std::to_string(delta) << std::endl;
//            return translate(room, delta, 0);
//        } else if (i.intersection.right <= i.intersecting_rect.right) {
//            std::cout << "relocate_x by " << std::to_string(-delta) << std::endl;
//            return translate(room, -delta, 0);
//        } else {
//            BK_ASSERT(false);
//        }
//    }
//
//    static rect_t relocate_rect_y(rect_t const room, intersection_t const i) {
//        auto const delta = static_cast<signed>(BUFFER_SIZE + i.intersection.height());
//
//        if (i.intersection.area() == room.area()) {
//            return room;//BK_ASSERT(false);
//        } else if (i.intersection.area() == i.intersecting_rect.area()) {
//            return room;//BK_ASSERT(false);
//        } else if (i.intersection.top >= i.intersecting_rect.top) {
//            std::cout << "relocate_y by " << std::to_string(delta) << std::endl;
//            return translate(room, 0, delta);
//        } else if (i.intersection.bottom <= i.intersecting_rect.bottom) {
//            std::cout << "relocate_y by " << std::to_string(-delta) << std::endl;
//            return translate(room, 0, -delta);
//        } else {
//            BK_ASSERT(false);
//        }
//    }
//
//    bool place_room(unique_room room, rect_t const relative_to) {
//        auto const w = BUFFER_SIZE + room->width();
//        auto const h = BUFFER_SIZE + room->height();
//
//        rect_t const rects[] = {
//            make_rect(*room, relative_to.right + BUFFER_SIZE , relative_to.top),
//            make_rect(*room, relative_to.left  - w, relative_to.top),
//            make_rect(*room, relative_to.left,      relative_to.bottom + BUFFER_SIZE ),
//            make_rect(*room, relative_to.left,      relative_to.top    - h),
//        };
//
//        std::default_random_engine gen(1984);
//        auto const first = std::uniform_int_distribution<size_t>(0, 3)(gen);
//
//        for (auto i = 0; i < 4; ++i) {
//            auto r   = rects[(i + first) % 4];
//            auto ins = get_intersection(r);
//            
//            if (ins.intersects()) {
//                if (i == 0 || i == 1) {
//                    r = relocate_rect_y(r, ins);
//                } else if (i == 2 || i == 3) {
//                    r = relocate_rect_x(r, ins);
//                }
//
//                if (get_intersection(r).intersects()) {
//                    continue;
//                }
//            }
//
//            add_room(std::move(room), r);
//            return true;
//        }
//
//        return false;
//    }
//
//
//    explicit room_set(std::default_random_engine& gen)
//        : gen(gen)
//    {
//        std::queue<rect_t> candidates;
//    
//        {
//            auto room = unique_room(
//                new room_compound(room_compound::generate(gen))
//            );
//
//            auto const rect = make_rect(*room, 0, 0);
//
//            add_room(std::move(room), rect);
//            candidates.push(rect);
//        }
//
//        while (!candidates.empty() && rooms.size() < 10) {       
//            auto cand = candidates.front();
//
//            if (place_room(
//                unique_room(
//                    new room_compound(room_compound::generate(gen))
//                ),
//                cand)
//            ) {
//                candidates.push(rooms.back().second);
//            } else {
//                candidates.pop();
//            }
//        }
//        
//    }
//
//    std::vector<record_t> rooms;
//    std::default_random_engine& gen;
//};
//
//
//TEST(MapGen, Simple) {
//    image_targa image(300, 300, image_targa::image_type::rgb24);
//
//    std::default_random_engine gen(1984);
//    
//    room_set rooms(gen);
//
//    for (auto const& r : rooms.rooms) {
//
//        r.first->write(r.second.left + 100, r.second.top + 100,
//            [&](unsigned x, unsigned y, room_part part) {
//                switch (part) {
//                case room_part::empty     : image.set(x, y, 0x00, 0x00, 0x00, 0x00); break;
//                case room_part::floor     : image.set(x, y, 0xFF, 0x00, 0x00, 0x00); break;
//                case room_part::v_edge    : image.set(x, y, 0x00, 0xFF, 0x00, 0x00); break;
//                case room_part::h_edge    : image.set(x, y, 0x00, 0x00, 0xFF, 0x00); break;
//                case room_part::corner_nw : image.set(x, y, 0xFF, 0xFF, 0x00, 0x00); break;
//                case room_part::corner_ne : image.set(x, y, 0xFF, 0x00, 0xFF, 0x00); break;
//                case room_part::corner_sw : image.set(x, y, 0x00, 0xFF, 0xFF, 0x00); break;
//                case room_part::corner_se : image.set(x, y, 0xFF, 0xFF, 0xFF, 0x00); break;
//                }
//            }
//        );
//    }
//
//    image.save("room.tga");
//}

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
    //unsigned const seeds[] = {
    //    13,        
    //    987,
    //    555,
    //    1025,
    //    1984,
    //    999,
    //    1578,
    //    9999,
    //    123456,
    //    987,
    //    10001,
    //};

    //for (int i = 0; i < 10; ++i) {
    //    image_targa image(100, 100, image_targa::image_type::rgb24);

    //    auto const seed = seeds[i];
    //
    //    std::default_random_engine gen(seed);

    //    auto room = room_compound::generate(gen);   

    //    room.write(0, 0,
    //        [&](unsigned x, unsigned y, room_part part) {
    //            switch (part) {
    //            case room_part::empty     : image.set(x, y, 0x00, 0x00, 0x00, 0x00); break;
    //            case room_part::floor     : image.set(x, y, 0xFF, 0x00, 0x00, 0x00); break;
    //            case room_part::v_edge    : image.set(x, y, 0x00, 0xFF, 0x00, 0x00); break;
    //            case room_part::h_edge    : image.set(x, y, 0x00, 0x00, 0xFF, 0x00); break;
    //            case room_part::corner_nw : image.set(x, y, 0xFF, 0xFF, 0x00, 0x00); break;
    //            case room_part::corner_ne : image.set(x, y, 0xFF, 0x00, 0xFF, 0x00); break;
    //            case room_part::corner_sw : image.set(x, y, 0x00, 0xFF, 0xFF, 0x00); break;
    //            case room_part::corner_se : image.set(x, y, 0xFF, 0xFF, 0xFF, 0x00); break;
    //            }

    //            //std::cout << "(" << std::to_string(x) << " " << std::to_string(y) << ")";
    //        }
    //    );

    //    image.save(std::string("room") + std::to_string(seed) + ".tga");
    //}
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
