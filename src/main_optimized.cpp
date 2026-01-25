#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <random>
#include <memory>
#include "option.hpp"
#include "csv_loader.hpp"
#include "black_scholes.hpp"
#include "monte_carlo_optimized.hpp"

constexpr size_t NUM_PATHS = 1'000'000;
constexpr unsigned int BASE_SEED = 12345;

// Lock-free result aggregation using pre-allocated arrays
void price_options_worker_optimized(
    const std::vector<Option>& options,
    size_t start_idx,
    size_t end_idx,
    Result* results_array,  // Pre-allocated, no mutex needed
    unsigned int seed
) {
    // Thread-local RNG with better distribution
    std::mt19937 rng(seed);
    
    for (size_t i = start_idx; i < end_idx; ++i) {
        const auto& opt = options[i];
        
        double mc_price = MonteCarloOptimized::price(opt, NUM_PATHS, rng);
        double delta = BlackScholes::delta(opt);
        double expected_return = mc_price / opt.K;
        
        // Direct write to pre-allocated position
        results_array[i] = {opt.symbol, mc_price, delta, expected_return};
    }
}

// Benchmark function
void benchmark_implementation(const std::vector<Option>& options, const std::string& name) {
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 8;
    
    // Pre-allocate results array
    auto results = std::make_unique<Result[]>(options.size());
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    size_t options_per_thread = options.size() / num_threads;
    
    for (unsigned int t = 0; t < num_threads; ++t) {
        size_t start_idx = t * options_per_thread;
        size_t end_idx = (t == num_threads - 1) ? options.size() : (t + 1) * options_per_thread;
        unsigned int seed = BASE_SEED + t;
        
        threads.emplace_back(
            price_options_worker_optimized,
            std::cref(options),
            start_idx,
            end_idx,
            results.get(),
            seed
        );
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Convert to vector for sorting
    std::vector<Result> result_vec(results.get(), results.get() + options.size());
    
    std::sort(result_vec.begin(), result_vec.end(), 
        [](const Result& a, const Result& b) {
            return a.expectedReturn > b.expectedReturn;
        });
    
    std::cout << "\n=== " << name << " Results ===" << std::endl;
    std::cout << "Time: " << duration.count() << " ms" << std::endl;
    std::cout << "Throughput: " << (options.size() * NUM_PATHS) / (duration.count() / 1000.0) / 1e6 
              << " million paths/sec" << std::endl;
    
    std::cout << "\nTop 5 Options:" << std::endl;
    std::cout << "Rank\tSymbol\t\t\tPrice\t\tExpReturn" << std::endl;
    
    for (size_t i = 0; i < std::min(size_t(5), result_vec.size()); ++i) {
        const auto& r = result_vec[i];
        std::cout << (i+1) << "\t" << r.symbol << "\t\t"
                  << r.price << "\t" << r.expectedReturn << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <csv_file>" << std::endl;
        return 1;
    }
    
    try {
        std::cout << "Loading options from " << argv[1] << "..." << std::endl;
        auto options = CSVLoader::load(argv[1]);
        std::cout << "Loaded " << options.size() << " options" << std::endl;
        
        // Run optimized benchmark
        benchmark_implementation(options, "Optimized Implementation");
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
