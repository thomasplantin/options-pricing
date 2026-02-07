#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <chrono>
#include <random>
#include <memory>
#include "core/option.hpp"
#include "utils/csv_loader.hpp"
#include "math/black_scholes.hpp"
#include "monte_carlo/baseline.hpp"
#include "monte_carlo/optimized.hpp"

constexpr size_t NUM_PATHS = 1'000'000;
constexpr unsigned int BASE_SEED = 12345;

/**
 * Configuration parsed from command-line arguments
 */
struct Config {
    std::string csv_file;
    bool use_optimized = false;
};

/**
 * Parse command-line arguments
 * @param argc Argument count
 * @param argv Argument values
 * @return Config struct with parsed values
 * @throws std::runtime_error on invalid arguments
 */
Config parse_args(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        throw std::runtime_error("Usage: " + std::string(argv[0]) + " [--optimized] <csv_file>");
    }
    
    Config config;
    
    if (argc == 3) {
        if (std::string(argv[1]) != "--optimized") {
            throw std::runtime_error("Unknown flag: " + std::string(argv[1]));
        }
        config.use_optimized = true;
        config.csv_file = argv[2];
    } else {
        config.csv_file = argv[1];
    }
    
    return config;
}

/**
 * Worker function for each thread
 * Processes a subset of options and stores results in pre-allocated array
 * @tparam MCEngine Monte Carlo engine (MonteCarlo or MonteCarloOptimized)
 * @param options Vector of options to price
 * @param start_idx Starting index in options vector
 * @param end_idx Ending index (exclusive)
 * @param results_array Pre-allocated array for results (lock-free)
 * @param seed RNG seed for this thread
 */
template<typename MCEngine>
void price_options_worker(
    const std::vector<Option>& options,
    size_t start_idx,
    size_t end_idx,
    Result* results_array,
    unsigned int seed
) {
    std::mt19937 rng(seed);
    
    for (size_t i = start_idx; i < end_idx; ++i) {
        const auto& opt = options[i];
        
        double mc_price = MCEngine::price(opt, NUM_PATHS, rng);
        double delta = BlackScholes::delta(opt);
        double expected_return = mc_price / opt.K;
        
        results_array[i] = {opt.symbol, mc_price, delta, expected_return};
    }
}

int main(int argc, char* argv[]) {
    try {
        auto config = parse_args(argc, argv);
        
        // Load options
        std::cout << "Loading options from " << config.csv_file << "..." << std::endl;
        auto options = CSVLoader::load(config.csv_file);
        std::cout << "Loaded " << options.size() << " options" << std::endl;
        
        // Determine thread count
        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 4;
        std::cout << "Using " << num_threads << " threads" << std::endl;
        std::cout << "Mode: " << (config.use_optimized ? "Optimized" : "Baseline") << std::endl;
        
        // Pre-allocate results array (lock-free)
        auto results = std::make_unique<Result[]>(options.size());
        
        // Start timing
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Partition work across threads
        std::vector<std::thread> threads;
        size_t options_per_thread = options.size() / num_threads;
        
        for (unsigned int t = 0; t < num_threads; ++t) {
            size_t start_idx = t * options_per_thread;
            size_t end_idx = (t == num_threads - 1) ? options.size() : (t + 1) * options_per_thread;
            unsigned int seed = BASE_SEED + t;
            
            if (config.use_optimized) {
                threads.emplace_back(
                    price_options_worker<MonteCarloOptimized>,
                    std::cref(options), start_idx, end_idx, results.get(), seed
                );
            } else {
                threads.emplace_back(
                    price_options_worker<MonteCarlo>,
                    std::cref(options), start_idx, end_idx, results.get(), seed
                );
            }
        }
        
        // Wait for all threads
        for (auto& thread : threads) {
            thread.join();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Convert to vector for sorting
        std::vector<Result> result_vec(results.get(), results.get() + options.size());
        
        // Rank by expected return
        std::sort(result_vec.begin(), result_vec.end(), 
            [](const Result& a, const Result& b) {
                return a.expectedReturn > b.expectedReturn;
            });
        
        // Output top 5
        std::cout << "\n=== Top 5 Options by Expected Return ===" << std::endl;
        std::cout << "Rank\tSymbol\t\t\tPrice\t\tDelta\t\tExpReturn" << std::endl;
        
        for (size_t i = 0; i < std::min(size_t(5), result_vec.size()); ++i) {
            const auto& r = result_vec[i];
            std::cout << (i+1) << "\t" << r.symbol << "\t\t"
                      << r.price << "\t\t" << r.delta << "\t" << r.expectedReturn << std::endl;
        }
        
        std::cout << "\nTotal time: " << duration.count() << " ms" << std::endl;
        std::cout << "Throughput: " << (options.size() * NUM_PATHS) / (duration.count() / 1000.0) / 1e6 
                  << " million paths/sec" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
