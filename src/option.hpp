#pragma once
#include <string>

// Represents a single option contract
struct Option {
    std::string symbol;  // Unique identifier
    double S;            // Spot price (current stock price)
    double K;            // Strike price
    double r;            // Risk-free rate (annualized)
    double sigma;        // Volatility (annualized)
    double T;            // Time to maturity (years)
    bool isCall;         // true = call, false = put
};

// Holds pricing results for one option
struct Result {
    std::string symbol;
    double price;           // Option price from Monte Carlo
    double delta;           // First derivative (sensitivity to spot price)
    double expectedReturn;  // (price - cost) / cost, for ranking
};
