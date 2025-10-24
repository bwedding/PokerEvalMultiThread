# Ultra-Fast Poker Hand Evaluator

A high-performance C++23 poker hand evaluator based on [Cactus Kev's Poker Library](https://github.com/suffecool/pokerlibCactus), modernized with C++23 features and optimized for maximum throughput.

## Performance Results

This implementation achieves exceptional performance on modern hardware:

### 5-Card Hand Evaluation
- **Peak Performance**: 1,381 million hands/second (parallel)
- **Single-threaded**: 176 million hands/second
- **Latency**: 0.72 nanoseconds per hand
- **Sustained Performance**: 1,204 million hands/second (1 billion hands)

### 7-Card Hand Evaluation
- **Peak Performance**: 110 million hands/second (parallel)
- **Single-threaded**: 11.6 million hands/second
- **Latency**: 9.05 nanoseconds per hand
- **Sustained Performance**: 103 million hands/second (100 million hands)

## Key Features

- **Modern C++23**: Leverages latest language features including `std::print`, concepts, and constexpr improvements
- **SIMD Optimized**: Compiled with AVX2 instructions and 512-bit vector support
- **Parallel Processing**: Uses `std::execution::par` for multi-threaded evaluation
- **Template-based Design**: Single codebase handles both 5-card and 7-card evaluation
- **Configurable Benchmarks**: Easy switching between evaluation modes

## Requirements

- **C++23 compliant compiler** (GCC 13+, Clang 16+, MSVC 2022 17.6+)
- **AVX2 support** (Intel Haswell+ or AMD Excavator+)
- **Multi-core CPU** for optimal parallel performance

## Building

```bash
# Example with GCC (adjust flags for your compiler)
g++ -std=c++23 -O3 -march=native -mavx2 -fopenmp Benchmark.cpp Poker.cpp -o benchmark
```

## Usage

The benchmark can evaluate either 5-card or 7-card hands by changing a single constant:

```cpp
// In Benchmark.cpp, line 15:
constexpr int CARD_COUNT = 5;  // Change to 7 for 7-card evaluation
```

Run the benchmark:
```bash
./benchmark
```

## Algorithm Details

This implementation uses Cactus Kev's perfect hash approach:

- **5-card evaluation**: Direct lookup using perfect hash of card combinations
- **7-card evaluation**: Iterates through all 21 possible 5-card combinations and selects the best
- **Hand ranking**: Returns standardized values where lower numbers indicate stronger hands

## Hand Distribution Validation

The benchmark includes statistical validation against theoretical probabilities:

### 5-Card Hands (100K sample)
| Hand Type | Observed | Expected | Difference |
|-----------|----------|----------|------------|
| Straight Flush | 0.00% | 0.0015% | -0.0015% |
| Four of a Kind | 0.02% | 0.024% | -0.004% |
| Full House | 0.14% | 0.144% | -0.004% |
| Flush | 0.21% | 0.197% | +0.013% |
| Straight | 0.37% | 0.392% | -0.022% |
| Three of a Kind | 2.12% | 2.11% | +0.01% |
| Two Pair | 4.76% | 4.75% | +0.01% |
| One Pair | 42.33% | 42.3% | +0.03% |
| High Card | 50.05% | 50.1% | -0.05% |

### 7-Card Hands (100K sample)
| Hand Type | Observed | Expected | Difference |
|-----------|----------|----------|------------|
| Straight Flush | 0.04% | 0.03% | +0.01% |
| Four of a Kind | 0.15% | 0.17% | -0.02% |
| Full House | 2.67% | 2.60% | +0.07% |
| Flush | 2.94% | 3.03% | -0.09% |
| Straight | 4.70% | 4.62% | +0.08% |
| Three of a Kind | 4.77% | 4.83% | -0.06% |
| Two Pair | 23.30% | 23.5% | -0.20% |
| One Pair | 43.78% | 43.8% | -0.02% |
| High Card | 17.66% | 17.4% | +0.26% |

## Performance Notes

- Results measured on modern multi-core CPU with AVX2 support
- Parallel performance scales with available CPU cores
- Memory bandwidth becomes limiting factor at very high throughput
- 5-card evaluation is ~12x faster than 7-card due to algorithmic complexity

## Credits

Based on the original [Cactus Kev's Poker Library](https://github.com/suffecool/pokerlibCactus) by Steve Brecher and contributors. Modernized and optimized for C++23 with SIMD instructions and parallel processing capabilities.

## License

This project maintains compatibility with the original library's licensing terms.