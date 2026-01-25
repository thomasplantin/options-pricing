# C++ Options Pricing Engine – Project Overview & Agent Interaction Guide

This document defines the **overall project architecture**, goals, development phases, and how AI agents collaborate to build a high‑performance options pricing engine in C++.

## Project Purpose

Build a production‑grade, CPU‑optimized options pricing engine that:

* Prices **European options** using:

  * Black–Scholes (analytic)
  * Monte Carlo simulation
* Supports **thousands of options**
* Uses **millions of paths per option**
* Scales across CPU cores
* Ranks the **top 5 options by expected return**
* Demonstrates why **C++ is required for performance**

American options will be supported later via Longstaff–Schwartz.

---

## Technology Constraints

* Language: **C++20**
* Platform: Linux / macOS / Windows
* CPU only (no GPU)
* No external pricing libraries
* STL preferred
* Deterministic builds

---

## Core Pipeline

```
CSV Input
   ↓
Option Structs
   ↓
Black–Scholes Pricing (validation)
   ↓
Monte Carlo Pricing (multithreaded)
   ↓
Metrics Computation
   ↓
Ranking Engine
   ↓
Console Output (Top 5)
```

---

## Agents

| Agent             | Owner of                       |
| ----------------- | ------------------------------ |
| Quant Agent       | Financial correctness & models |
| C++ Systems Agent | Architecture & implementation  |
| Performance Agent | Speed & scalability            |
| Validation Agent  | Numerical correctness          |

---

## Agent Interaction Model

```
Quant Agent
   ↓ (models & equations)
C++ Systems Agent
   ↓ (working implementation)
Performance Agent
   ↓ (optimized engine)
Validation Agent
   ↓ (test results & error metrics)
   ↑
   └──────────── feedback loop ────────────┘
```

### Rules

* Quant Agent defines models first
* Systems Agent implements exactly
* Performance Agent optimizes without altering math
* Validation Agent verifies everything
* Failures loop back to responsible agent

---

## Development Phases

### Phase 1 – Financial Core

Owner: Quant Agent

* Define Black–Scholes formulas
* Define Monte Carlo model
* Payoff logic
* Greeks (delta)

Deliverable: Mathematical spec

---

### Phase 2 – C++ Implementation

Owner: C++ Systems Agent

* Project structure
* Data loading
* Pricing engine
* Threading system
* CLI output

Deliverable: Working engine

---

### Phase 3 – Performance Optimization

Owner: Performance Agent

* Profiling
* Loop optimization
* Memory layout tuning
* Thread scaling
* Compiler flags

Deliverable: Benchmarks

---

### Phase 4 – Validation & Testing

Owner: Validation Agent

* Compare MC vs Black–Scholes
* Error bounds
* Regression tests
* Edge cases

Deliverable: Validation report

---

### Phase 5 – American Options (Optional)

Owner: Quant Agent → Systems → Performance → Validation

* Longstaff–Schwartz implementation
* Regression solver
* Early exercise logic

---

## Performance Targets

| Metric            | Target       |
| ----------------- | ------------ |
| Options priced    | ≥ 1,000      |
| Paths/option      | ≥ 1,000,000  |
| Runtime (8 cores) | < 10 seconds |
| CPU utilization   | > 90%        |
| Relative error    | < 1%         |

---

## Build Standard

```bash
g++ -std=c++20 -O3 -march=native -pthread
```

---

## Output Format

Console example:

```
Rank  Symbol  Price   ExpectedReturn
1     SPX230C 12.45   0.183
2     SPX240C 10.91   0.162
3     ...
```

---

## Quality Gates

The project is considered complete only if:

* Monte Carlo converges to Black–Scholes
* Multi-core scaling is demonstrated
* Benchmarks are reproducible
* No memory leaks
* No race conditions
* Deterministic test results

---

## Disallowed

* Python
* GPU computing
* Black-box libraries
* UI frameworks
* Network dependencies

---

## Success Criteria

* Financially correct
* Numerically stable
* Clearly faster than Python
* Maintainable C++ codebase
* Clean separation of responsibilities
