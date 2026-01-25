#pragma once
#include <cmath>
#include <random>
#include "option.hpp"

/**
 * Monte Carlo pricing for European options
 * 
 * Stock dynamics (GBM - Geometric Brownian Motion, risk-neutral):
 * S_T = S · exp((r - σ²/2)·T + σ·√T·Z)   where Z ~ N(0,1), σ² = variance
 * 
 * Payoffs:
 * Call: max(S_T - K, 0)
 * Put:  max(K - S_T, 0)
 * 
 * Price = e^(-rT) · (1/N) · Σ payoff(S_T^i)
 */
class MonteCarlo {
public:
    /**
     * Price an option using Monte Carlo simulation
     * @param opt Option to price
     * @param num_paths Number of simulation paths
     * @param rng Random number generator (one per thread)
     */
    static double price(const Option& opt, size_t num_paths, std::mt19937& rng) {
        std::normal_distribution<double> normal(0.0, 1.0);
        
        double drift = (opt.r - 0.5 * opt.sigma * opt.sigma) * opt.T;
        double diffusion = opt.sigma * std::sqrt(opt.T);
        double discount = std::exp(-opt.r * opt.T);
        
        double sum_payoff = 0.0;
        
        for (size_t i = 0; i < num_paths; ++i) {
            double Z = normal(rng);
            double S_T = opt.S * std::exp(drift + diffusion * Z);
            
            double payoff = opt.isCall ? std::max(S_T - opt.K, 0.0) 
                                       : std::max(opt.K - S_T, 0.0);
            
            sum_payoff += payoff;
        }
        
        return discount * (sum_payoff / num_paths);
    }
};
