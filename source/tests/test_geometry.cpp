#include "pch.hpp"
#include "geometry.hpp"

#include <gtest/gtest.h>

TEST(Rect, Separate) {
    typedef rect<signed>    rect_t;
    typedef point2d<signed> point_t;

    static auto const WB = 5;
    static auto const HB = 5;

    static auto const WA = 3;
    static auto const HA = 3;

    rect_t const a(point_t(1, 1), WA, HA);
    rect_t const b(point_t(0, 0), WB, HB);
    
    auto const do_test = [&](signed dx, signed dy)
    {
        auto const a0 = translate(a, dx, dy);    
        auto const an = separate_rects<direction::north>::get(a0, b);
        auto const as = separate_rects<direction::south>::get(a0, b);
        auto const ae = separate_rects<direction::east>::get(a0, b);
        auto const aw = separate_rects<direction::west>::get(a0, b);

        EXPECT_TRUE( intersection_of(a0, b).is_rect());

        EXPECT_FALSE(intersection_of(an, b).is_rect());
        EXPECT_FALSE(intersection_of(as, b).is_rect());
        EXPECT_FALSE(intersection_of(ae, b).is_rect());
        EXPECT_FALSE(intersection_of(aw, b).is_rect());

        EXPECT_EQ(an.bottom, b.top);
        EXPECT_EQ(as.top,    b.bottom);
        EXPECT_EQ(ae.left,   b.right);
        EXPECT_EQ(aw.right,  b.left);

        EXPECT_EQ(a0.width(), an.width());
        EXPECT_EQ(a0.width(), as.width());
        EXPECT_EQ(a0.width(), ae.width());
        EXPECT_EQ(a0.width(), aw.width());

        EXPECT_EQ(a0.height(), an.height());
        EXPECT_EQ(a0.height(), as.height());
        EXPECT_EQ(a0.height(), ae.height());
        EXPECT_EQ(a0.height(), aw.height());
    };

    do_test( 0,  0);
    do_test( 0, -2);
    do_test( 0,  2);
    do_test(-2,  0);
    do_test(-2, -2);
    do_test(-2,  2);
    do_test( 2,  0);
    do_test( 2, -2);
    do_test( 2,  2);
}
