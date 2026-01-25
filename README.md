# C++ Options Pricing Engine

A high-performance, CPU-optimized options pricing engine that demonstrates why C++ is essential for computational finance. Prices thousands of European options using Monte Carlo simulation with millions of paths per option.

## Performance Results

**M2 MacBook Air (4P+4E cores):**
- **10,000 options** × 1M paths = **10 billion calculations** in **40.6 seconds**
- **Throughput**: 246 million paths/second
- **Memory**: <2MB peak usage
- **Optimization**: 46% faster than baseline implementation

## Quick Start

Build both versions:
```bash
make
```

Run baseline implementation:
```bash
./pricing.out data/synthetic/european-options/options_medium.csv
```

Run optimized implementation:
```bash
./pricing_optimized.out data/synthetic/european-options/options_medium.csv
```

Compare performance side-by-side:
```bash
make benchmark
```

Test different dataset sizes:
```bash
# Small dataset (100 options)
./pricing.out data/synthetic/european-options/options_small.csv

# Large dataset (10,000 options)
./pricing_optimized.out data/synthetic/european-options/options_large.csv
```

Run comprehensive benchmarks:
```bash
./benchmark.sh
```

## Architecture

```
CSV Input → Option Structs → Monte Carlo Pricing → Ranking → Top 5 Output
```

**Key Components:**
- **Monte Carlo Engine**: Geometric Brownian Motion simulation
- **Threading**: Optimized for 4-8 cores with lock-free aggregation
- **Memory**: Batch processing with aligned arrays for cache efficiency
- **Compiler**: `-O3 -march=native -ffast-math` for maximum performance

## Performance Optimizations

1. **Batching**: Process 1024 paths at once for cache locality
2. **Loop Unrolling**: 4x unroll reduces overhead
3. **Lock-Free**: Pre-allocated arrays eliminate mutex contention
4. **Memory Alignment**: 32-byte aligned for optimal cache performance

**Thread Scaling on M2:**
- 1 thread: 475ms
- 4 threads: 464ms (optimal)
- 8 threads: 494ms (efficiency cores add overhead)

## Financial Models

### Black-Scholes Formula
Used for validation and delta calculation:

```
C = S₀N(d₁) - Ke^(-rT)N(d₂)
P = Ke^(-rT)N(-d₂) - S₀N(-d₁)

where:
d₁ = [ln(S₀/K) + (r + σ²/2)T] / (σ√T)
d₂ = d₁ - σ√T
```

### Monte Carlo Simulation
Primary pricing method using Geometric Brownian Motion:

```
S_T = S₀ × exp[(r - σ²/2)T + σ√T × Z]
```

**Where:**
- `S₀` = Current stock price
- `K` = Strike price  
- `r` = Risk-free rate
- `σ` = Volatility (annualized)
- `T` = Time to expiration (years)
- `Z` ~ N(0,1) = Standard normal random variable

**Payoffs:**
- Call: `max(S_T - K, 0)`
- Put: `max(K - S_T, 0)`

**Price:** `e^(-rT) × (1/N) × Σ payoff(S_T^i)`

### Data Processing

**Input Format (CSV):**
```
symbol,S,K,r,sigma,T,isCall
AAPL_C_150_30,145.50,150.00,0.05,0.25,0.25,1
```

**Risk-Neutral Valuation:**
- Uses risk-free rate `r` for drift
- Discounts payoffs at risk-free rate
- Assumes no dividends or early exercise

**Convergence:**
- 1M paths typically converge within 1% of Black-Scholes
- Standard error decreases as 1/√N
- Deterministic seeding ensures reproducible results

**Expected Return Calculation:**
```
Expected Return = Price / Strike
```
Used for ranking options by potential profitability.

## Build Requirements

- **C++20** compiler (g++ or clang++)
- **macOS/Linux** (tested on M2 MacBook Air)
- **Make** build system

## Project Structure

```
src/
├── main.cpp                    # Baseline implementation
├── main_optimized.cpp          # Performance-optimized version
├── monte_carlo.hpp             # Standard Monte Carlo
├── monte_carlo_optimized.hpp   # Batched + unrolled version
├── black_scholes.hpp           # Analytical pricing
├── option.hpp                  # Data structures
└── csv_loader.hpp              # Data input
```
