// PokerEval.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <print>
#include <chrono>
#include <array>
#include <ranges>
#include "Poker.h"
#include "arrays.h"

/****************************************************************
    This code tests the evaluator by looping over all 2,598,960
    possible five-card poker hands, calculating each hand's
    distinct value, and displaying the frequency count of each
    hand type. It also prints the amount of time taken to
    perform all the calculations.

    Kevin L. Suffecool (a.k.a "Cactus Kev"), 2001
    kevin@suffe.cool

    Modernized to C++20
****************************************************************/

namespace {

    // The expected frequency count for each hand rank.
    constexpr std::array<int, 10> expected_freq = {
        0, 40, 624, 3744, 5108, 10200, 54912, 123552, 1098240, 1302540
    };

} // anonymous namespace

int main()
{
    using namespace poker;
    using namespace std::chrono;


    // Initialize the deck.
    auto deck = poker::init_deck();

    // Zero out the frequency array.
    std::array<int, 10> freq{};

    // Capture start time.
    auto start = steady_clock::now();

    // Loop over every possible five-card hand.
    std::array<int, 5> hand;

    for (int a = 0; a < 48; ++a) {
        hand[0] = deck[a];
        for (int b = a + 1; b < 49; ++b) {
            hand[1] = deck[b];
            for (int c = b + 1; c < 50; ++c) {
                hand[2] = deck[c];
                for (int d = c + 1; d < 51; ++d) {
                    hand[3] = deck[d];
                    for (int e = d + 1; e < 52; ++e) {
                        hand[4] = deck[e];

                        unsigned short value = eval_5hand(hand);
                        int n = poker::hand_rank(value);

                        // Uncomment to print each hand:
                        // print_hand(hand);
                        // std::println("  {}", value_str[n]);

                        ++freq[n];
                    }
                }
            }
        }
    }

    // Capture end time.
    auto end = steady_clock::now();

    // Print results.
    for (int i : std::views::iota(1, 10)) 
    {
        std::print("{:>15s}: {:8d}", value_str[i], freq[i]);
        if (freq[i] != expected_freq[i]) 
        {
            std::println(" (expected {})", expected_freq[i]);
        }
        else {
            std::println("");
        }
    }

    // Calculate and print elapsed time.
    auto elapsed = duration_cast<microseconds>(end - start);
    std::println("\nElapsed time: {:.4f} (msecs)",
        elapsed.count() / 1000.0);

    return 0;
}

