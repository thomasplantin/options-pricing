#pragma once
#include <cmath>
#include "core/constants.hpp"

/** 
 * Standard normal probability density function
 * φ(x) = (1/√(2π)) * e^(-x²/2)
 */
inline double phi(double x) {
    return std::exp(-0.5 * x * x) / std::sqrt(2.0 * M_PI);
}

/**
 * Standard normal cumulative distribution function
 * Φ(x) ≈ 1 - φ(x) × (a₁t + a₂t² + a₃t³ + a₄t⁴ + a₅t⁵)   for x ≥ 0
 * where t = 1/(1 + p·x)
 * Uses Abramowitz & Stegun approximation (max error ~1.5e-7)
 */ 
inline double norm_cdf(double x) {
    // Use symmetry property: N(-x) = 1 - N(x)
    // The approximation formula only works for x ≥ 0, so we reflect negative values
    if (x < 0.0) {
        return 1.0 - norm_cdf(-x);
    }
    
    double t = 1.0 / (1.0 + constants::AS_P * x);
    double t_2 = t * t;
    double t_3 = t_2 * t;
    double t_4 = t_3 * t;
    double t_5 = t_4 * t;
    
    double poly = constants::AS_A1*t + constants::AS_A2*t_2 + constants::AS_A3*t_3 
                + constants::AS_A4*t_4 + constants::AS_A5*t_5;
    
    return 1.0 - phi(x) * poly;
}
