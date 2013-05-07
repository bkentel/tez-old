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
