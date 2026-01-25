#include <gtest/gtest.h>
#include <cmath>
#include "../src/option.hpp"
#include "../src/black_scholes.hpp"
#include "../src/normal.hpp"

class BlackScholesTest : public ::testing::Test {};

TEST_F(BlackScholesTest, AtmCall) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, true};
    double price = BlackScholes::price(opt);
    EXPECT_NEAR(price, 10.45, 0.5);
}

TEST_F(BlackScholesTest, AtmPut) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, false};
    double price = BlackScholes::price(opt);
    EXPECT_NEAR(price, 5.57, 0.5);
}

TEST_F(BlackScholesTest, PutCallParity) {
    Option call = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, true};
    Option put = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, false};
    
    double call_price = BlackScholes::price(call);
    double put_price = BlackScholes::price(put);
    double discount = std::exp(-0.05 * 1.0);
    
    double lhs = call_price - put_price;
    double rhs = 100.0 - 100.0 * discount;
    EXPECT_NEAR(lhs, rhs, 1e-6);
}

TEST_F(BlackScholesTest, ItmCall) {
    Option opt = {"TEST", 110.0, 100.0, 0.05, 0.2, 1.0, true};
    double price = BlackScholes::price(opt);
    EXPECT_GE(price, 10.0);
}

TEST_F(BlackScholesTest, OtmCall) {
    Option opt = {"TEST", 90.0, 100.0, 0.05, 0.2, 1.0, true};
    double price = BlackScholes::price(opt);
    EXPECT_GT(price, 0.0);
    EXPECT_LT(price, 10.0);
}

TEST_F(BlackScholesTest, DeepItmCall) {
    Option opt = {"TEST", 150.0, 100.0, 0.05, 0.2, 1.0, true};
    double price = BlackScholes::price(opt);
    EXPECT_GE(price, 50.0);
}

TEST_F(BlackScholesTest, DeltaCallBounds) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, true};
    double delta = BlackScholes::delta(opt);
    EXPECT_GT(delta, 0.0);
    EXPECT_LT(delta, 1.0);
}

TEST_F(BlackScholesTest, DeltaPutBounds) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, false};
    double delta = BlackScholes::delta(opt);
    EXPECT_GT(delta, -1.0);
    EXPECT_LT(delta, 0.0);
}

TEST_F(BlackScholesTest, DeltaAtm) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 1.0, true};
    double delta = BlackScholes::delta(opt);
    EXPECT_NEAR(delta, 0.64, 0.1);
}

TEST_F(BlackScholesTest, HighVolatility) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.8, 1.0, true};
    double price = BlackScholes::price(opt);
    EXPECT_GT(price, 0.0);
}

TEST_F(BlackScholesTest, ShortExpiry) {
    Option opt = {"TEST", 100.0, 100.0, 0.05, 0.2, 0.001, true};
    double price = BlackScholes::price(opt);
    EXPECT_NEAR(price, 0.0, 0.3);
}

TEST_F(BlackScholesTest, DeepOtm) {
    Option opt = {"TEST", 50.0, 100.0, 0.05, 0.2, 1.0, true};
    double price = BlackScholes::price(opt);
    EXPECT_GT(price, 0.0);
    EXPECT_LT(price, 1.0);
}
