/*
 * loxodrome_on_ellipsoidTest.cpp
 *
 * \date May 12, 2014
 * \author cec
 */

#include "loxodrome_on_ellipsoidTest.hpp"
#include "ssc/geometry/loxodrome_on_ellipsoid.hpp"
#include <cmath>

#define PI (4.*atan(1.))
#define RAD (PI/180.)
#define EPS 1E-6

using namespace ssc::geometry;

loxodrome_on_ellipsoidTest::loxodrome_on_ellipsoidTest() : a(ssc::random_data_generator::DataGenerator(564562)),
                                                           latitude_of_Murray_Spring(-(36  + 47/60. + 49.2232/3600)*RAD),
                                                           longitude_of_Murray_Spring((148 + 11/60. + 48.3333/3600)*RAD),
                                                           latitude_of_Wauka_1978(-(37  + 30/60. + 18.0674/3600)*RAD),
                                                           longitude_of_Wauka_1978((149  + 58/60. + 32.9932/3600)*RAD),
                                                           inverse_flattening(298.257222101), // GRS80
                                                           semi_major_axis(6378137) // GRS80
{
}

loxodrome_on_ellipsoidTest::~loxodrome_on_ellipsoidTest()
{
}

void loxodrome_on_ellipsoidTest::SetUp()
{
}

void loxodrome_on_ellipsoidTest::TearDown()
{
}

TEST_F(loxodrome_on_ellipsoidTest, isometric_latitude)
{
//! [loxodrome_on_ellipsoidTest isometric_latitude_example]
    const double e = sqrt((2.-1./inverse_flattening)/inverse_flattening);
    ASSERT_NEAR(-(39  + 23/60. + 36.268670/3600)*RAD, isometric_latitude(e, latitude_of_Murray_Spring), EPS);
    ASSERT_NEAR(-(40  + 16/60. + 40.540366/3600)*RAD, isometric_latitude(e, latitude_of_Wauka_1978), EPS);
//! [loxodrome_on_ellipsoidTest isometric_latitude_example]
}

TEST_F(loxodrome_on_ellipsoidTest, meridian_distance)
{
//! [loxodrome_on_ellipsoidTest meridian_distance_example]
    ASSERT_NEAR(-4073983.614420, meridian_distance(1/inverse_flattening, semi_major_axis, latitude_of_Murray_Spring), EPS);
    ASSERT_NEAR(-4152559.155874, meridian_distance(1/inverse_flattening, semi_major_axis, latitude_of_Wauka_1978), EPS);
//! [loxodrome_on_ellipsoidTest meridian_distance_example]
}

TEST_F(loxodrome_on_ellipsoidTest, azimuth_of_loxodrome)
{
//! [loxodrome_on_ellipsoidTest azimuth_of_loxodrome_example]
    const double e = sqrt((2.-1./inverse_flattening)/inverse_flattening);
    ASSERT_NEAR((116  + 26/60. + 8.400701/3600)*RAD,
                azimuth_of_loxodrome(e,
                                     latitude_of_Murray_Spring, longitude_of_Murray_Spring,
                                     latitude_of_Wauka_1978, longitude_of_Wauka_1978),
                EPS);
//! [loxodrome_on_ellipsoidTest azimuth_of_loxodrome_example]
}

TEST_F(loxodrome_on_ellipsoidTest, loxodrome_distance)
{
//! [loxodrome_on_ellipsoidTest inverse_example]
    double s12 = 0;
    double az12 = 0;
    loxodrome_inverse(inverse_flattening,
                      semi_major_axis,
                      latitude_of_Murray_Spring, longitude_of_Murray_Spring,
                      latitude_of_Wauka_1978, longitude_of_Wauka_1978,
                      s12, az12);
    ASSERT_NEAR(176497.829952, s12, EPS);
    //! [loxodrome_on_ellipsoidTest inverse_example]
}

TEST_F(loxodrome_on_ellipsoidTest, direct_problem_of_the_loxodrome_on_the_ellipsoid)
{
    //! [loxodrome_on_ellipsoidTest loxodrome_direct_example]
    double s12 = 176497.829952;
    double az12 = (116  + 26/60. + 8.400701/3600)*RAD;
    double lat = 0;
    double lon = 0;
    loxodrome_direct(inverse_flattening,
                     semi_major_axis,
                     latitude_of_Murray_Spring, longitude_of_Murray_Spring,
                     lat, lon,
                     s12, az12);
    ASSERT_NEAR(latitude_of_Wauka_1978, lat, EPS);
    ASSERT_NEAR(longitude_of_Wauka_1978, lon, EPS);
    //! [loxodrome_on_ellipsoidTest loxodrome_direct_example]
}

TEST_F(loxodrome_on_ellipsoidTest, direct_problem_of_the_loxodrome_on_the_ellipsoid_for_big_distances)
{
    //! [loxodrome_on_ellipsoidTest loxodrome_direct_example]
    double expected_lat = 0;
    double expected_lon = 0;
    double s12 = 0;
    double az12 = 0;
    loxodrome_inverse(inverse_flattening,
                      semi_major_axis,
                      latitude_of_Murray_Spring, longitude_of_Murray_Spring,
                      expected_lat, expected_lon,
                      s12, az12);
    double lat = 0;
    double lon = 0;
    loxodrome_direct(inverse_flattening,
                     semi_major_axis,
                     latitude_of_Murray_Spring, longitude_of_Murray_Spring,
                     lat, lon,
                     s12, az12);
    ASSERT_NEAR(cos(expected_lat), cos(lat), EPS);
    ASSERT_NEAR(sin(expected_lat), sin(lat), EPS);
    ASSERT_NEAR(cos(expected_lon), cos(lon), EPS);
    ASSERT_NEAR(sin(expected_lon), sin(lon), EPS);
    //! [loxodrome_on_ellipsoidTest loxodrome_direct_example]
}

TEST_F(loxodrome_on_ellipsoidTest, can_convert_isometric_latitude_to_latitude)
{
    double phi;
    phi = -0.4;      ASSERT_NEAR(phi, convert_isometric_latitude_to_latitude(isometric_latitude(phi)), EPS);
    phi = 0;         ASSERT_NEAR(phi, convert_isometric_latitude_to_latitude(isometric_latitude(phi)), EPS);
    phi = 1;         ASSERT_NEAR(phi, convert_isometric_latitude_to_latitude(isometric_latitude(phi)), EPS);
    phi = -1;        ASSERT_NEAR(phi, convert_isometric_latitude_to_latitude(isometric_latitude(phi)), EPS);
    phi = 0.4;       ASSERT_NEAR(phi, convert_isometric_latitude_to_latitude(isometric_latitude(phi)), EPS);
    phi = -0.4;      ASSERT_NEAR(phi, convert_isometric_latitude_to_latitude(isometric_latitude(phi)), EPS);
    phi = -1.4;      ASSERT_NEAR(phi, convert_isometric_latitude_to_latitude(isometric_latitude(phi)), EPS);
    phi = 1.4;       ASSERT_NEAR(phi, convert_isometric_latitude_to_latitude(isometric_latitude(phi)), EPS);
    phi = PI/2.-1E-3;ASSERT_NEAR(phi, convert_isometric_latitude_to_latitude(isometric_latitude(phi)), 1E-3);
}
