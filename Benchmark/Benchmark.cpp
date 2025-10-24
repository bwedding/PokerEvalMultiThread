#include <print>
#include <chrono>
#include <array>
#include <random>
#include <vector>
#include <span>
#include <future>
#include <execution>
#include <numeric>
#include <algorithm>
#include "Poker.h"

/****************************************************************
    Poker Hand Evaluator Benchmark

    Tests performance by evaluating random poker hands and
    calculating hands evaluated per second.
    
    Configure CARD_COUNT below to switch between 5-card and 7-card evaluation.
****************************************************************/

using namespace poker;
using namespace std::chrono;

// Configuration: Set to 5 or 7 to choose evaluation type
constexpr int CARD_COUNT = 7;

// Generate random poker hands from a deck
template<int N>
std::vector<std::array<int, N>> generate_test_hands(int count) {
    std::vector<std::array<int, N>> hands(count);
    auto deck = init_deck();

    std::for_each(std::execution::par_unseq, hands.begin(), hands.end(),
        [&deck](auto& hand) {
            thread_local std::mt19937 gen(std::random_device{}());

            // Create indices array fresh each time
            std::array<int, 52> indices;
            std::iota(indices.begin(), indices.end(), 0);

            // Partial Fisher-Yates shuffle
            for (int j = 0; j < N; ++j) {
                std::uniform_int_distribution<int> dist(j, 51);
                int swap_idx = dist(gen);
                std::swap(indices[j], indices[swap_idx]);
                hand[j] = deck[indices[j]];
            }
        }
    );

    return hands;
}

// Wrapper function to evaluate hands based on card count
template<int N>
unsigned short evaluate_hand(std::span<const int> hand) {
    if constexpr (N == 5) {
        return eval_5hand(hand);
    } else if constexpr (N == 7) {
        return eval_7hand(hand);
    } else {
        static_assert(N == 5 || N == 7, "Only 5-card and 7-card evaluation supported");
    }
}

int main() {
    std::println("=== {}-Card Poker Hand Evaluator Benchmark ===\n", CARD_COUNT);

    // Warm-up run
    std::println("Warming up...");
    if constexpr (CARD_COUNT == 5) {
        auto warmup_hands = generate_test_hands<5>(10000);
        for (auto& hand : warmup_hands) {
            volatile auto result = evaluate_hand<5>(std::span{ hand });
            (void)result; // Prevent optimization
        }
    } else {
        auto warmup_hands = generate_test_hands<7>(10000);
        for (auto& hand : warmup_hands) {
            volatile auto result = evaluate_hand<7>(std::span{ hand });
            (void)result; // Prevent optimization
        }
    }

    // Benchmark configurations
    struct BenchmarkConfig {
        int num_hands;
        const char* description;
    };


    // Single-threaded test
    std::println("\n=== SINGLE-THREADED {}-Card Test ===", CARD_COUNT);
    
    if constexpr (CARD_COUNT == 5) {
        auto st_hands = generate_test_hands<5>(10'000'000);
        auto st_start = steady_clock::now();
        unsigned long long st_total = 0;
        for (const auto& hand : st_hands) {
            st_total += evaluate_hand<5>(std::span{ hand });
        }
        auto st_end = steady_clock::now();
        auto st_elapsed = duration_cast<nanoseconds>(st_end - st_start).count() / 1e9;
        std::println("Single-thread: {:.4f}s, {:.2f}M hands/sec",
            st_elapsed, 10.0 / st_elapsed);
        std::println("Checksum: {}", st_total);
    } else {
        auto st_hands = generate_test_hands<7>(10'000'000);
        auto st_start = steady_clock::now();
        unsigned long long st_total = 0;
        for (const auto& hand : st_hands) {
            st_total += evaluate_hand<7>(std::span{ hand });
        }
        auto st_end = steady_clock::now();
        auto st_elapsed = duration_cast<nanoseconds>(st_end - st_start).count() / 1e9;
        std::println("Single-thread: {:.4f}s, {:.2f}M hands/sec",
            st_elapsed, 10.0 / st_elapsed);
        std::println("Checksum: {}", st_total);
    }

    // Benchmark configurations - adjust based on your needs
    std::array<BenchmarkConfig, 4> configs;
    
    if constexpr (CARD_COUNT == 5) {
        // 5-card evaluation is faster, use larger test sets
        configs = { {
            {50'000'000, "50M hands"},
            {100'000'000, "100M hands"},
            {500'000'000, "500M hands"},
			{1'000'000'000, "1B hands"}
        } };
    } else {
        // 7-card evaluation is slower, use smaller test sets
        configs = { {
            {5'000'000, "5M hands"},
            {10'000'000, "10M hands"},
            {50'000'000, "50M hands"},
			{100'000'000, "100M hands"}
        } };
    }

    for (const auto& config : configs) {
        std::println("\n--- {} ---", config.description);
        std::println("Generating {} random {}-card hands...", config.num_hands, CARD_COUNT);

        unsigned long long total = 0;
        auto start = steady_clock::now();
        auto end = steady_clock::now();

        if constexpr (CARD_COUNT == 5) {
            auto test_hands = generate_test_hands<5>(config.num_hands);
            std::println("Evaluating...");
            
            start = steady_clock::now();
            total = std::transform_reduce(
                std::execution::par,
                test_hands.begin(),
                test_hands.end(),
                0ULL,
                std::plus<unsigned long long>{},
                [](const auto& hand) -> unsigned long long {
                    return evaluate_hand<5>(std::span{ hand });
                }
            );
            end = steady_clock::now();
        } else {
            auto test_hands = generate_test_hands<7>(config.num_hands);
            std::println("Evaluating...");
            
            start = steady_clock::now();
            total = std::transform_reduce(
                std::execution::par,
                test_hands.begin(),
                test_hands.end(),
                0ULL,
                std::plus<unsigned long long>{},
                [](const auto& hand) -> unsigned long long {
                    return evaluate_hand<7>(std::span{ hand });
                }
            );
            end = steady_clock::now();
        }

        // Calculate statistics
        auto elapsed_ns = duration_cast<nanoseconds>(end - start).count();
        double elapsed_sec = elapsed_ns / 1'000'000'000.0;
        double hands_per_sec = config.num_hands / elapsed_sec;
        double ns_per_hand = elapsed_ns / static_cast<double>(config.num_hands);

        // Print results
        std::println("\nResults:");
        std::println("  Total hands evaluated: {:L}", config.num_hands);
        std::println("  Elapsed time: {:.4f} seconds", elapsed_sec);
        std::println("  Hands per second: {:.0f}", hands_per_sec);
        std::println("  Million hands/sec: {:.2f}M", hands_per_sec / 1'000'000.0);
        std::println("  Nanoseconds per hand: {:.2f} ns", ns_per_hand);
        std::println("  Microseconds per hand: {:.2f} us", ns_per_hand / 1000.0);
        std::println("  Checksum (prevent optimization): {}", total);
    }

    // Frequency distribution check
    std::println("\n\n=== Hand Distribution Check (100K hands) ===");
    
    std::array<int, 10> freq{};

    if constexpr (CARD_COUNT == 5) {
        auto dist_hands = generate_test_hands<5>(100'000);
        for (const auto& hand : dist_hands) {
            unsigned short value = evaluate_hand<5>(std::span{ hand });
            int rank = hand_rank(value);
            ++freq[rank];
        }
    } else {
        auto dist_hands = generate_test_hands<7>(100'000);
        for (const auto& hand : dist_hands) {
            unsigned short value = evaluate_hand<7>(std::span{ hand });
            int rank = hand_rank(value);
            ++freq[rank];
        }
    }

    std::println("\nHand type distribution:");
    for (int i = 1; i <= 9; ++i) {
        double percentage = (freq[i] * 100.0) / 100'000;
        std::println("  {:>15s}: {:6d} ({:5.2f}%)",
            value_str[i], freq[i], percentage);
    }

    // Expected probabilities
    if constexpr (CARD_COUNT == 5) {
        std::println("\nExpected 5-card probabilities (for reference):");
        std::println("  Straight Flush: ~0.0015%");
        std::println("  Four of a Kind: ~0.024%");
        std::println("  Full House:     ~0.144%");
        std::println("  Flush:          ~0.197%");
        std::println("  Straight:       ~0.392%");
        std::println("  Three of Kind:  ~2.11%");
        std::println("  Two Pair:       ~4.75%");
        std::println("  One Pair:       ~42.3%");
        std::println("  High Card:      ~50.1%");
    } else {
        std::println("\nExpected 7-card probabilities (for reference):");
        std::println("  Straight Flush: ~0.03%");
        std::println("  Four of a Kind: ~0.17%");
        std::println("  Full House:     ~2.60%");
        std::println("  Flush:          ~3.03%");
        std::println("  Straight:       ~4.62%");
        std::println("  Three of Kind:  ~4.83%");
        std::println("  Two Pair:       ~23.5%");
        std::println("  One Pair:       ~43.8%");
        std::println("  High Card:      ~17.4%");
    }

    return 0;
}