#include <gtest/gtest.h>
#include <cmath>
#include <random>
#include "core/option.hpp"
#include "monte_carlo/baseline.hpp"
#include "math/black_scholes.hpp"

class MonteCarloTest : public ::testing::Test {};

TEST_F(MonteCarloTest, Determinism) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, true};
    
    std::mt19937 rng1(42);
    double price1 = MonteCarlo::price(opt, 100000, rng1);
    
    std::mt19937 rng2(42);
    double price2 = MonteCarlo::price(opt, 100000, rng2);
    
    EXPECT_NEAR(price1, price2, 1e-10);
}

TEST_F(MonteCarloTest, ConvergenceToBs) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, true};
    double bs_price = BlackScholes::price(opt);
    
    std::mt19937 rng(42);
    double mc_price = MonteCarlo::price(opt, 1000000, rng);
    
    double tolerance = bs_price * 0.01;
    EXPECT_NEAR(mc_price, bs_price, tolerance);
}

TEST_F(MonteCarloTest, PutConvergence) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, false};
    double bs_price = BlackScholes::price(opt);
    
    std::mt19937 rng(42);
    double mc_price = MonteCarlo::price(opt, 1000000, rng);
    
    double tolerance = bs_price * 0.01;
    EXPECT_NEAR(mc_price, bs_price, tolerance);
}

TEST_F(MonteCarloTest, PositivePrice) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, true};
    std::mt19937 rng(42);
    double price = MonteCarlo::price(opt, 10000, rng);
    EXPECT_GE(price, 0.0);
}

TEST_F(MonteCarloTest, CallItmLowerBound) {
    Option opt = {"TEST", 110.0, 100.0, 0.05, 0.2, 1.0, true};
    std::mt19937 rng(42);
    double price = MonteCarlo::price(opt, 100000, rng);
    double intrinsic = 10.0;
    EXPECT_GE(price, intrinsic * 0.95);
}

TEST_F(MonteCarloTest, PutItmLowerBound) {
    Option opt = {"TEST", 90.0, 100.0, 0.05, 0.2, 1.0, false};
    std::mt19937 rng(42);
    double price = MonteCarlo::price(opt, 100000, rng);
    double intrinsic = 10.0;
    EXPECT_GE(price, intrinsic * 0.95);
}

TEST_F(MonteCarloTest, ShortExpiryConvergence) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 0.01, true};
    double bs_price = BlackScholes::price(opt);
    
    std::mt19937 rng(42);
    double mc_price = MonteCarlo::price(opt, 100000, rng);
    
    double tolerance = std::max(bs_price * 0.05, 0.01);
    EXPECT_NEAR(mc_price, bs_price, tolerance);
}

TEST_F(MonteCarloTest, HighVolatilityConvergence) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.8, 1.0, true};
    double bs_price = BlackScholes::price(opt);
    
    std::mt19937 rng(42);
    double mc_price = MonteCarlo::price(opt, 500000, rng);
    
    double tolerance = bs_price * 0.02;
    EXPECT_NEAR(mc_price, bs_price, tolerance);
}

TEST_F(MonteCarloTest, ConvergenceImproves) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, true};
    double bs_price = BlackScholes::price(opt);
    
    std::mt19937 rng1(42);
    double mc_100k = MonteCarlo::price(opt, 100000, rng1);
    
    std::mt19937 rng2(42);
    double mc_1m = MonteCarlo::price(opt, 1000000, rng2);
    
    double error_100k = std::abs(mc_100k - bs_price);
    double error_1m = std::abs(mc_1m - bs_price);
    
    EXPECT_LT(error_1m, error_100k);
}
