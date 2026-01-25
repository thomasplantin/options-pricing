#include <gtest/gtest.h>
#include <cmath>
#include "../src/normal.hpp"

class NormalTest : public ::testing::Test {};

TEST_F(NormalTest, CdfSymmetry) {
    double x = 1.5;
    double cdf_pos = norm_cdf(x);
    double cdf_neg = norm_cdf(-x);
    EXPECT_NEAR(cdf_pos + cdf_neg, 1.0, 1e-7);
}

TEST_F(NormalTest, CdfBounds) {
    EXPECT_NEAR(norm_cdf(0.0), 0.5, 1e-7);
    EXPECT_LT(norm_cdf(-10.0), 1e-6);
    EXPECT_GT(norm_cdf(10.0), 1.0 - 1e-6);
}

TEST_F(NormalTest, CdfMonotonic) {
    double cdf1 = norm_cdf(-2.0);
    double cdf2 = norm_cdf(0.0);
    double cdf3 = norm_cdf(2.0);
    EXPECT_LT(cdf1, cdf2);
    EXPECT_LT(cdf2, cdf3);
}

TEST_F(NormalTest, CdfStandardValues) {
    EXPECT_NEAR(norm_cdf(1.0), 0.8413, 1e-3);
    EXPECT_NEAR(norm_cdf(2.0), 0.9772, 1e-3);
    EXPECT_NEAR(norm_cdf(-1.0), 0.1587, 1e-3);
}
