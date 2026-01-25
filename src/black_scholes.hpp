#pragma once
#include <cmath>
#include "option.hpp"
#include "normal.hpp"

/**
 * Black-Scholes pricing for European options
 * Used as validation baseline for Monte Carlo results
 * 
 * d₁ = [ln(S/K) + (r + σ²/2)T] / (σ√T)
 * d₂ = d₁ - σ√T
 * 
 * Call: C = S·N(d₁) - K·e^(-rT)·N(d₂)
 * Put:  P = K·e^(-rT)·N(-d₂) - S·N(-d₁)
 */
class BlackScholes {
public:
    /**
     * Price an option using Black-Scholes closed-form solution
     */
    static double price(const Option& opt) {
        double sqrt_T = std::sqrt(opt.T);
        double d1 = (std::log(opt.S / opt.K) + (opt.r + 0.5 * opt.sigma * opt.sigma) * opt.T) 
                    / (opt.sigma * sqrt_T);
        double d2 = d1 - opt.sigma * sqrt_T;
        
        double discount = std::exp(-opt.r * opt.T);

        return opt.isCall ? opt.S * norm_cdf(d1) - opt.K * discount * norm_cdf(d2)
                          : opt.K * discount * norm_cdf(-d2) - opt.S * norm_cdf(-d1);
    }
    
    /**
     * Calculate delta (∂Price/∂S)
     * Δ_call = N(d₁)
     * Δ_put  = N(d₁) - 1
     */
    static double delta(const Option& opt) {
        double sqrt_T = std::sqrt(opt.T);
        double d1 = (std::log(opt.S / opt.K) + (opt.r + 0.5 * opt.sigma * opt.sigma) * opt.T) 
                    / (opt.sigma * sqrt_T);

        return opt.isCall ? norm_cdf(d1)
                          : norm_cdf(d1) - 1.0;
    }
};
