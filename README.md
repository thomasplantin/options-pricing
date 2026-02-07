# C++ Options Pricing Engine

A high-performance, CPU-optimized options pricing engine that demonstrates why C++ is essential for computational finance. Prices thousands of European options using Monte Carlo simulation with millions of paths per option, resulting in billions of computations.

## Setup & Quick Start

### Prerequisites

- **C++20** compiler (g++ or clang++)
- **Make** build system
- **Google Test** (optional, for running tests)
  ```bash
  # macOS
  brew install googletest
  
  # Linux (Ubuntu/Debian)
  sudo apt-get install libgtest-dev
  ```

### Build

Clone the repository and build:
```bash
git clone https://github.com/thomasplantin/options-pricing.git
cd options-pricing
make
```

This creates `bin/pricing.out` executable.

### Run

**Single dataset:**
```bash
# Baseline implementation
./bin/pricing.out data/synthetic/european-options/options_medium.csv

# Optimized implementation
./bin/pricing.out --optimized data/synthetic/european-options/options_medium.csv
```

**Benchmark all datasets:**
```bash
make benchmark
```

This runs small (100 options), medium (1,000 options), and large (10,000 options) datasets with both baseline and optimized implementations, then displays a comparison table.

**Benchmark specific dataset:**
```bash
make benchmark-run data=small   # or medium, large
```

**Run tests:**
```bash
make test
```

**Clean build artifacts:**
```bash
make clean
```

## Performance Results

**M2 MacBook Air (4P+4E cores):**
- **10,000 options** × 1M paths = **10 billion calculations** in **40.6 seconds**
- **Throughput**: 246 million paths/second
- **Memory**: <2MB peak usage
- **Optimization**: 46% faster than baseline implementation

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
Primary pricing method using Geometric Brownian Motion under the **risk-neutral measure**:

```
S_T = S₀ × exp[(r - σ²/2)T + σ√T × Z]
```

**Parameters:**
- `S₀` = Current stock price
- `K` = Strike price  
- `r` = Risk-free rate (replaces actual drift under no-arbitrage)
- `σ` = Volatility (annualized)
- `T` = Time to expiration (years)
- `Z` ~ N(0,1) = Standard normal random variable

**Payoffs:**
- Call: `max(S_T - K, 0)`
- Put: `max(K - S_T, 0)`

**Price:** `e^(-rT) × (1/N) × Σ payoff(S_T^i)`

### Why Monte Carlo vs Black-Scholes?

| Method | Use Case | Trade-off |
|--------|----------|-----------|
| **Black-Scholes** | Closed-form European options | Fast, exact for model assumptions |
| **Monte Carlo** | Path-dependent, exotic options | Flexible but computationally expensive |

For European options, Black-Scholes gives the exact answer instantly. We use Monte Carlo here to:
1. Demonstrate the simulation framework for future exotic extensions
2. Validate MC convergence against the known BS solution
3. Showcase high-performance C++ for compute-intensive workloads

### Validation & Convergence

- **Target**: |MC price − BS price| / BS price < 1%
- **Path count**: 1M paths → standard error ∝ 1/√N ≈ σ_payoff / 1000
- Deterministic seeding ensures reproducible results

### Expected Return Metric

```
Expected Return = Price / Strike
```

This ratio ranks options by relative "cheapness" of exposure—not actual expected return (which requires real-world drift estimates).

### Model Limitations

| Assumption | Implication |
|------------|-------------|
| **European exercise** | No early exercise; American options require different methods |
| **No dividends** | Underlying pays no dividends during option life |
| **Constant volatility** | σ is fixed; no stochastic vol or term structure |
| **Log-normal prices** | Stock cannot go negative; ignores jumps or fat tails |
| **Continuous trading** | Assumes frictionless hedging (no transaction costs) |

### Input Format (CSV)

```
symbol,S,K,r,sigma,T,isCall
AAPL_C_150_30,145.50,150.00,0.05,0.25,0.25,1
```

## Project Structure

```
src/
├── main.cpp                    # Unified main with runtime selection
├── core/
│   ├── option.hpp              # Option data structure
│   └── constants.hpp           # Global constants
├── math/
│   ├── normal.hpp              # Normal distribution CDF
│   └── black_scholes.hpp       # Analytical pricing
├── monte_carlo/
│   ├── baseline.hpp            # Standard Monte Carlo
│   └── optimized.hpp           # Batched + unrolled version
└── utils/
    └── csv_loader.hpp          # CSV data input

tests/
├── math/
│   ├── normal_test.cpp
│   └── black_scholes_test.cpp
└── monte_carlo/
    ├── baseline_test.cpp
    └── optimized_test.cpp
```
