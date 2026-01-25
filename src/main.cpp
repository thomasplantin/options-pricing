#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <random>
#include "option.hpp"
#include "csv_loader.hpp"
#include "black_scholes.hpp"
#include "monte_carlo.hpp"

constexpr size_t NUM_PATHS = 1'000'000;
constexpr unsigned int BASE_SEED = 12345;

/**
 * Worker function for each thread
 * Processes a subset of options and stores results
 */
void price_options_worker(
    const std::vector<Option>& options,
    size_t start_idx,
    size_t end_idx,
    std::vector<Result>& results,
    std::mutex& results_mutex,
    unsigned int seed
) {
    std::mt19937 rng(seed);
    std::vector<Result> local_results;
    
    for (size_t i = start_idx; i < end_idx; ++i) {
        const auto& opt = options[i];
        
        double mc_price = MonteCarlo::price(opt, NUM_PATHS, rng);
        double delta = BlackScholes::delta(opt);
        
        // Expected return = (price / intrinsic_value) - 1
        // For simplicity, using price as proxy
        double expected_return = mc_price / opt.K;
        
        local_results.push_back({opt.symbol, mc_price, delta, expected_return});
    }
    
    // Lock only for final aggregation
    std::lock_guard<std::mutex> lock(results_mutex);
    results.insert(results.end(), local_results.begin(), local_results.end());
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <csv_file>" << std::endl;
        return 1;
    }
    
    try {
        // Load options
        std::cout << "Loading options from " << argv[1] << "..." << std::endl;
        auto options = CSVLoader::load(argv[1]);
        std::cout << "Loaded " << options.size() << " options" << std::endl;
        
        // Determine thread count
        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 4;
        std::cout << "Using " << num_threads << " threads" << std::endl;
        
        // Start timing
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Partition work
        std::vector<std::thread> threads;
        std::vector<Result> results;
        std::mutex results_mutex;
        
        size_t options_per_thread = options.size() / num_threads;
        
        for (unsigned int t = 0; t < num_threads; ++t) {
            size_t start_idx = t * options_per_thread;
            // Last thread handles remainder from integer division
            size_t end_idx = (t == num_threads - 1) ? options.size() : (t + 1) * options_per_thread;
            unsigned int seed = BASE_SEED + t;
            
            threads.emplace_back(
                price_options_worker,
                std::cref(options),
                start_idx,
                end_idx,
                std::ref(results),
                std::ref(results_mutex),
                seed
            );
        }
        
        // Wait for all threads
        for (auto& thread : threads) {
            thread.join();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Rank by expected return
        std::sort(results.begin(), results.end(), 
            [](const Result& a, const Result& b) {
                return a.expectedReturn > b.expectedReturn;
            });
        
        // Output top 5
        std::cout << "\n=== Top 5 Options by Expected Return ===" << std::endl;
        std::cout << "Rank\tSymbol\t\t\tPrice\t\tDelta\t\tExpReturn" << std::endl;
        
        for (size_t i = 0; i < std::min(size_t(5), results.size()); ++i) {
            const auto& r = results[i];
            std::cout << (i+1) << "\t" 
                      << r.symbol << "\t\t"
                      << r.price << "\t\t"
                      << r.delta << "\t"
                      << r.expectedReturn << std::endl;
        }
        
        std::cout << "\nTotal time: " << duration.count() << " ms" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
