#pragma once
#include <cmath>
#include <random>
#include "core/option.hpp"

class MonteCarloOptimized {
public:
    static double price(const Option& opt, size_t num_paths, std::mt19937& rng) {
        constexpr size_t BATCH_SIZE = 1024;
        const size_t num_batches = num_paths / BATCH_SIZE;
        const size_t remainder = num_paths % BATCH_SIZE;
        
        const double drift = (opt.r - 0.5 * opt.sigma * opt.sigma) * opt.T;
        const double diffusion = opt.sigma * std::sqrt(opt.T);
        const double discount = std::exp(-opt.r * opt.T);
        
        std::normal_distribution<double> normal(0.0, 1.0);
        double sum_payoff = 0.0;
        
        alignas(32) double batch_randoms[BATCH_SIZE];
        
        for (size_t batch = 0; batch < num_batches; ++batch) {
            for (size_t i = 0; i < BATCH_SIZE; ++i) {
                batch_randoms[i] = normal(rng);
            }
            
            double batch_sum = 0.0;
            for (size_t i = 0; i < BATCH_SIZE; i += 4) {
                double Z1 = batch_randoms[i];
                double Z2 = batch_randoms[i+1];
                double Z3 = batch_randoms[i+2];
                double Z4 = batch_randoms[i+3];
                
                double S_T1 = opt.S * std::exp(drift + diffusion * Z1);
                double S_T2 = opt.S * std::exp(drift + diffusion * Z2);
                double S_T3 = opt.S * std::exp(drift + diffusion * Z3);
                double S_T4 = opt.S * std::exp(drift + diffusion * Z4);
                
                if (opt.isCall) {
                    batch_sum += std::max(S_T1 - opt.K, 0.0);
                    batch_sum += std::max(S_T2 - opt.K, 0.0);
                    batch_sum += std::max(S_T3 - opt.K, 0.0);
                    batch_sum += std::max(S_T4 - opt.K, 0.0);
                } else {
                    batch_sum += std::max(opt.K - S_T1, 0.0);
                    batch_sum += std::max(opt.K - S_T2, 0.0);
                    batch_sum += std::max(opt.K - S_T3, 0.0);
                    batch_sum += std::max(opt.K - S_T4, 0.0);
                }
            }
            sum_payoff += batch_sum;
        }
        
        for (size_t i = 0; i < remainder; ++i) {
            double Z = normal(rng);
            double S_T = opt.S * std::exp(drift + diffusion * Z);
            double payoff = opt.isCall ? std::max(S_T - opt.K, 0.0) 
                                       : std::max(opt.K - S_T, 0.0);
            sum_payoff += payoff;
        }
        
        return discount * (sum_payoff / num_paths);
    }
};
