#include "pch.hpp"
#include <gtest/gtest.h>

#include "direction.hpp"

TEST(Direction, GetXAxis) {
    auto x = get_x_axis_vector<direction::here>::value;

    EXPECT_EQ( 0, get_x_axis_vector<direction::here>::value);
    EXPECT_EQ( 0, get_x_axis_vector<direction::none>::value);
    EXPECT_EQ( 0, get_x_axis_vector<direction::north>::value);
    EXPECT_EQ( 0, get_x_axis_vector<direction::south>::value);
    EXPECT_EQ( 1, get_x_axis_vector<direction::east>::value);
    EXPECT_EQ(-1, get_x_axis_vector<direction::west>::value);
    EXPECT_EQ( 1, get_x_axis_vector<direction::north_east>::value);
    EXPECT_EQ(-1, get_x_axis_vector<direction::north_west>::value);
    EXPECT_EQ( 1, get_x_axis_vector<direction::south_east>::value);
    EXPECT_EQ(-1, get_x_axis_vector<direction::south_west>::value);
    EXPECT_EQ( 0, get_x_axis_vector<direction::up>::value);
    EXPECT_EQ( 0, get_x_axis_vector<direction::down>::value);

    EXPECT_EQ( 0, get_x_vector(direction::here));
    EXPECT_EQ( 0, get_x_vector(direction::none));
    EXPECT_EQ( 0, get_x_vector(direction::north));
    EXPECT_EQ( 0, get_x_vector(direction::south));
    EXPECT_EQ( 1, get_x_vector(direction::east));
    EXPECT_EQ(-1, get_x_vector(direction::west));
    EXPECT_EQ( 1, get_x_vector(direction::north_east));
    EXPECT_EQ(-1, get_x_vector(direction::north_west));
    EXPECT_EQ( 1, get_x_vector(direction::south_east));
    EXPECT_EQ(-1, get_x_vector(direction::south_west));
    EXPECT_EQ( 0, get_x_vector(direction::up));
    EXPECT_EQ( 0, get_x_vector(direction::down));
}

TEST(Direction, GetYAxis) {
    EXPECT_EQ( 0, get_y_axis_vector<direction::here>::value);
    EXPECT_EQ( 0, get_y_axis_vector<direction::none>::value);
    EXPECT_EQ(-1, get_y_axis_vector<direction::north>::value);
    EXPECT_EQ( 1, get_y_axis_vector<direction::south>::value);
    EXPECT_EQ( 0, get_y_axis_vector<direction::east>::value);
    EXPECT_EQ( 0, get_y_axis_vector<direction::west>::value);
    EXPECT_EQ(-1, get_y_axis_vector<direction::north_east>::value);
    EXPECT_EQ(-1, get_y_axis_vector<direction::north_west>::value);
    EXPECT_EQ( 1, get_y_axis_vector<direction::south_east>::value);
    EXPECT_EQ( 1, get_y_axis_vector<direction::south_west>::value);
    EXPECT_EQ( 0, get_y_axis_vector<direction::up>::value);
    EXPECT_EQ( 0, get_y_axis_vector<direction::down>::value);

    EXPECT_EQ( 0, get_y_vector(direction::here));
    EXPECT_EQ( 0, get_y_vector(direction::none));
    EXPECT_EQ(-1, get_y_vector(direction::north));
    EXPECT_EQ( 1, get_y_vector(direction::south));
    EXPECT_EQ( 0, get_y_vector(direction::east));
    EXPECT_EQ( 0, get_y_vector(direction::west));
    EXPECT_EQ(-1, get_y_vector(direction::north_east));
    EXPECT_EQ(-1, get_y_vector(direction::north_west));
    EXPECT_EQ( 1, get_y_vector(direction::south_east));
    EXPECT_EQ( 1, get_y_vector(direction::south_west));
    EXPECT_EQ( 0, get_y_vector(direction::up));
    EXPECT_EQ( 0, get_y_vector(direction::down));
}

TEST(Direction, GetZAxis) {
    EXPECT_EQ( 0, get_z_axis_vector<direction::here>::value);
    EXPECT_EQ( 0, get_z_axis_vector<direction::none>::value);
    EXPECT_EQ( 0, get_z_axis_vector<direction::north>::value);
    EXPECT_EQ( 0, get_z_axis_vector<direction::south>::value);
    EXPECT_EQ( 0, get_z_axis_vector<direction::east>::value);
    EXPECT_EQ( 0, get_z_axis_vector<direction::west>::value);
    EXPECT_EQ( 0, get_z_axis_vector<direction::north_east>::value);
    EXPECT_EQ( 0, get_z_axis_vector<direction::north_west>::value);
    EXPECT_EQ( 0, get_z_axis_vector<direction::south_east>::value);
    EXPECT_EQ( 0, get_z_axis_vector<direction::south_west>::value);
    EXPECT_EQ(-1, get_z_axis_vector<direction::up>::value);
    EXPECT_EQ( 1, get_z_axis_vector<direction::down>::value);

    EXPECT_EQ( 0, get_z_vector(direction::here));
    EXPECT_EQ( 0, get_z_vector(direction::none));
    EXPECT_EQ( 0, get_z_vector(direction::north));
    EXPECT_EQ( 0, get_z_vector(direction::south));
    EXPECT_EQ( 0, get_z_vector(direction::east));
    EXPECT_EQ( 0, get_z_vector(direction::west));
    EXPECT_EQ( 0, get_z_vector(direction::north_east));
    EXPECT_EQ( 0, get_z_vector(direction::north_west));
    EXPECT_EQ( 0, get_z_vector(direction::south_east));
    EXPECT_EQ( 0, get_z_vector(direction::south_west));
    EXPECT_EQ(-1, get_z_vector(direction::up));
    EXPECT_EQ( 1, get_z_vector(direction::down));
}
