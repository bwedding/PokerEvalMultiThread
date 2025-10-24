#include "Poker.h"
#include <array>
#include <algorithm>
#include <random>
#include <print>
#include <ranges>
#include <string_view>

// Poker hand evaluator
//
// Kevin L. Suffecool (a.k.a. "Cactus Kev")
// kevin@suffe.cool
//
// Modernized to C++20

namespace poker {

    using Deck = std::array<int, 52>;
    using Hand = std::span<const int>;  // Change from std::span<int>



    // Print a poker hand in human-readable format (e.g., "Ac 4d 7c Jh 2s")
    void print_hand(Hand hand)
    {
        constexpr std::string_view ranks = "??23456789TJQKA?";

        for (int card : hand) {
            int r = (card >> 8) & 0xF;
            char suit = [card]() constexpr {
                if (card & CLUB) return 'c';
                if (card & DIAMOND) return 'd';
                if (card & HEART) return 'h';
                return 's';
                }();

            std::print("{}{} ", ranks[r], suit);
        }
    }



   

} // namespace poker
