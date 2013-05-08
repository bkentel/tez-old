#include "pch.hpp"
#include "bklib/geometry.hpp"

#include <gtest/gtest.h>

TEST(Distance, Arithmetic) {
    EXPECT_EQ(10, bklib::distance(10, 20));
    EXPECT_EQ(10, bklib::distance(20, 10));

    EXPECT_EQ(10u, bklib::distance(10u, 20u));
    EXPECT_EQ(10u, bklib::distance(20u, 10u));

    EXPECT_FLOAT_EQ(10.0f, bklib::distance(10.0f, 20.0f));
    EXPECT_FLOAT_EQ(10.0f, bklib::distance(20.0f, 10.0f));

    EXPECT_DOUBLE_EQ(10.0, bklib::distance(10.0, 20.0));
    EXPECT_DOUBLE_EQ(10.0, bklib::distance(20.0, 10.0));
}

TEST(Point2d, Constructor) {
    bklib::point2d<unsigned> p(10u, 20u);

    EXPECT_EQ(10u, p.x);
    EXPECT_EQ(20u, p.y);
}

TEST(Point2d, Comparison) {
    bklib::point2d<unsigned> a(10u, 20u);
    bklib::point2d<unsigned> b(15u, 25u);

    EXPECT_NE(a, b);
    EXPECT_NE(b, a);

    EXPECT_EQ(a, a);
    EXPECT_EQ(b, b);

    a = b;

    EXPECT_EQ(a, b);
    EXPECT_EQ(b, a);
}

TEST(Point2d, Distance) {
    bklib::point2d<unsigned> a(0u, 0u);
    bklib::point2d<unsigned> b(10u, 10u);

    EXPECT_EQ(200u, distance2(a, b));
    EXPECT_EQ(200u, distance2(b, a));

    EXPECT_DOUBLE_EQ(std::sqrt(200.0), distance(a, b));
    EXPECT_DOUBLE_EQ(std::sqrt(200.0), distance(b, a));
}

TEST(Point2d, Translate) {
    bklib::point2d<unsigned> p(0u, 0u);

    p.translate_by(10, 20);

    EXPECT_EQ(10u, p.x);
    EXPECT_EQ(20u, p.y);

    p.translate_to(1, 2);

    EXPECT_EQ(1u, p.x);
    EXPECT_EQ(2u, p.y);
}

TEST(Range, Constructor) {
    bklib::range<unsigned> range(10u, 20u);

    EXPECT_EQ(10u, range.first);
    EXPECT_EQ(20u, range.last);

    EXPECT_TRUE(range);
}

TEST(Range, Translate) {
    bklib::range<unsigned> range_a(10u, 20u);
    auto range_b = range_a;
    
    range_b.translate_by(10);
    EXPECT_TRUE(range_b);
    EXPECT_EQ(range_a.magnitude(), range_b.magnitude());
    EXPECT_EQ(range_b.first,  20);
    EXPECT_EQ(range_b.last, 30);

    range_b.translate_to(0);
    EXPECT_TRUE(range_b);
    EXPECT_EQ(range_a.magnitude(), range_b.magnitude());
    EXPECT_EQ(range_b.first,  0);
    EXPECT_EQ(range_b.last, 10);

}

TEST(Range, Comparison) {
    bklib::range<unsigned> a(10u, 20u);
    bklib::range<unsigned> b(15u, 25u);

    EXPECT_NE(a, b);
    EXPECT_NE(b, a);

    EXPECT_EQ(a, a);
    EXPECT_EQ(b, b);

    EXPECT_LT(a, b);

    a = b;

    EXPECT_EQ(a, b);
    EXPECT_EQ(b, a);
}

TEST(Range, IntersectionRange) {
    bklib::range<unsigned> const a(10u, 20u);
    bklib::range<unsigned> const b(15u, 25u);       
    bklib::range<unsigned> const c(30u, 35u);

    EXPECT_EQ(a, a.intersection_with(a));
    EXPECT_EQ(b, b.intersection_with(b));

    EXPECT_TRUE(a.intersects(b));
    EXPECT_TRUE(a.intersection_with(b));

    EXPECT_TRUE(b.intersects(a));
    EXPECT_TRUE(b.intersection_with(a));

    EXPECT_EQ(
        bklib::range<unsigned>(15u, 20u),
        a.intersection_with(b)
    );

    EXPECT_EQ(
        bklib::range<unsigned>(15u, 20u),
        b.intersection_with(a)
    );

    EXPECT_FALSE(a.intersects(c));
    EXPECT_FALSE(a.intersection_with(c));

    EXPECT_FALSE(b.intersects(c));
    EXPECT_FALSE(b.intersection_with(c));

    bklib::range<unsigned> const d(10u, 20u);
    bklib::range<unsigned> const e(0u,  10u);
    bklib::range<unsigned> const f(20u, 30u);

    EXPECT_TRUE(d.intersection_with(e));
    EXPECT_TRUE(d.intersection_with(f));

    EXPECT_EQ(
        bklib::range<unsigned>(10u, 10u),
        d.intersection_with(e)
    );

    EXPECT_EQ(
        bklib::range<unsigned>(20u, 20u),
        d.intersection_with(f)
    );
}

TEST(Range, IntersectionValue) {
    bklib::range<unsigned> const range(10u, 20u);

    EXPECT_FALSE(range.intersects(9u));
    EXPECT_TRUE(range.intersects(10u));
    EXPECT_TRUE(range.intersects(15u));
    EXPECT_TRUE(range.intersects(20u));
    EXPECT_FALSE(range.intersects(21u));
}

TEST(Rect, ConstructorPointSize) {
    bklib::rect<signed> const rect(bklib::point2d<signed>(10, 20), 10, 20);

    EXPECT_EQ(10, rect.left);
    EXPECT_EQ(20, rect.top);
    EXPECT_EQ(20, rect.right);
    EXPECT_EQ(40, rect.bottom);

    EXPECT_EQ(10, rect.width());
    EXPECT_EQ(20, rect.height());

    EXPECT_TRUE(rect);
}

TEST(Rect, ConstructorSides) {
    bklib::rect<signed> const rect(10, 20, 30, 40);

    EXPECT_EQ(10, rect.left);
    EXPECT_EQ(20, rect.top);
    EXPECT_EQ(30, rect.right);
    EXPECT_EQ(40, rect.bottom);

    EXPECT_EQ(20, rect.width());
    EXPECT_EQ(20, rect.height());

    EXPECT_TRUE(rect);
}

TEST(Rect, Translate) {
    bklib::rect<signed> const rect_a(bklib::point2d<signed>(10, 10), 10, 10);
    auto rect_b = rect_a;

    rect_b.translate_by(-10, -10);
    EXPECT_TRUE(rect_b);
    EXPECT_EQ(0,  rect_b.left);
    EXPECT_EQ(0,  rect_b.top);
    EXPECT_EQ(10, rect_b.right);
    EXPECT_EQ(10, rect_b.bottom);
    EXPECT_EQ(10, rect_b.width());
    EXPECT_EQ(10, rect_b.height());

    rect_b.translate_to(20, 20);
    EXPECT_TRUE(rect_b);
    EXPECT_EQ(20,  rect_b.left);
    EXPECT_EQ(20,  rect_b.top);
    EXPECT_EQ(30, rect_b.right);
    EXPECT_EQ(30, rect_b.bottom);
    EXPECT_EQ(10, rect_b.width());
    EXPECT_EQ(10, rect_b.height());
}


TEST(Rect, IntersectionRect) {
                      //  N  NE  E  SE  S  SW   W  NW
    signed const dx[] = { 0,  1, 1,  1, 0, -1, -1, -1};
    signed const dy[] = {-1, -1, 0,  1, 1,  1,  0, -1};

    bklib::rect<signed> const rect_a(bklib::point2d<signed>(10, 10), 10, 10);

    EXPECT_EQ(rect_a, rect_a.intersection_with(rect_a));

    for (unsigned i = 0; i < 8; ++i) {
        bklib::rect<signed> rect_b = rect_a;
        rect_b.translate_by(dx[i]*5, dy[i]*5);

        EXPECT_NE(rect_a, rect_b);

        EXPECT_TRUE(rect_a.intersects(rect_b));
        EXPECT_TRUE(rect_a.intersection_with(rect_b));

        EXPECT_EQ(dx[i] ? 5 : 10, rect_a.intersection_with(rect_b).width());
        EXPECT_EQ(dy[i] ? 5 : 10, rect_a.intersection_with(rect_b).height());
    }
}

TEST(Rect, IntersectionPoint) {
    bklib::rect<signed> const rect_a(bklib::point2d<signed>(10, 10), 10, 10);

    EXPECT_TRUE(rect_a.intersects(bklib::point2d<signed>(10, 10)));
    EXPECT_TRUE(rect_a.intersects(bklib::point2d<signed>(10, 15)));
    EXPECT_TRUE(rect_a.intersects(bklib::point2d<signed>(10, 20)));
    EXPECT_TRUE(rect_a.intersects(bklib::point2d<signed>(15, 10)));
    EXPECT_TRUE(rect_a.intersects(bklib::point2d<signed>(15, 15)));
    EXPECT_TRUE(rect_a.intersects(bklib::point2d<signed>(15, 20)));
    EXPECT_TRUE(rect_a.intersects(bklib::point2d<signed>(20, 10)));
    EXPECT_TRUE(rect_a.intersects(bklib::point2d<signed>(20, 15)));
    EXPECT_TRUE(rect_a.intersects(bklib::point2d<signed>(20, 20)));

    EXPECT_FALSE(rect_a.intersects(bklib::point2d<signed>(10, 9)));
    EXPECT_FALSE(rect_a.intersects(bklib::point2d<signed>(10, 21)));
    EXPECT_FALSE(rect_a.intersects(bklib::point2d<signed>(20, 9)));
    EXPECT_FALSE(rect_a.intersects(bklib::point2d<signed>(20, 21)));
    EXPECT_FALSE(rect_a.intersects(bklib::point2d<signed>(9,  10)));
    EXPECT_FALSE(rect_a.intersects(bklib::point2d<signed>(21, 10)));
    EXPECT_FALSE(rect_a.intersects(bklib::point2d<signed>(9,  20)));
    EXPECT_FALSE(rect_a.intersects(bklib::point2d<signed>(21, 20)));
}
