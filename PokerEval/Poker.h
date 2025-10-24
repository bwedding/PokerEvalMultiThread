#pragma once

#include <array>
#include <span>
#include <string_view>
#include <cstdint>
#include "arrays.h"
#include <algorithm>
#include <random>

namespace poker {

    // Hand rank categories
    enum class HandRank : int {
        StraightFlush = 1,
        FourOfAKind = 2,
        FullHouse = 3,
        Flush = 4,
        Straight = 5,
        ThreeOfAKind = 6,
        TwoPair = 7,
        OnePair = 8,
        HighCard = 9
    };

    // Legacy defines for backward compatibility
    inline constexpr int STRAIGHT_FLUSH = 1;
    inline constexpr int FOUR_OF_A_KIND = 2;
    inline constexpr int FULL_HOUSE = 3;
    inline constexpr int FLUSH = 4;
    inline constexpr int STRAIGHT = 5;
    inline constexpr int THREE_OF_A_KIND = 6;
    inline constexpr int TWO_PAIR = 7;
    inline constexpr int ONE_PAIR = 8;
    inline constexpr int HIGH_CARD = 9;

    // Hand rank descriptions
    inline constexpr std::array<std::string_view, 10> value_str = {
        "",
        "Straight Flush",
        "Four of a Kind",
        "Full House",
        "Flush",
        "Straight",
        "Three of a Kind",
        "Two Pair",
        "One Pair",
        "High Card"
    };

    // Suit bit masks
    inline constexpr int CLUB = 0x8000;
    inline constexpr int DIAMOND = 0x4000;
    inline constexpr int HEART = 0x2000;
    inline constexpr int SPADE = 0x1000;

    // Card ranks
    enum class Rank : int {
        Deuce = 2,
        Trey = 3,
        Four = 4,
        Five = 5,
        Six = 6,
        Seven = 7,
        Eight = 8,
        Nine = 9,
        Ten = 10,
        Jack = 11,
        Queen = 12,
        King = 13,
        Ace = 14
    };

    // Legacy rank constants
    inline constexpr int Deuce = 2;
    inline constexpr int Trey = 3;
    inline constexpr int Four = 4;
    inline constexpr int Five = 5;
    inline constexpr int Six = 6;
    inline constexpr int Seven = 7;
    inline constexpr int Eight = 8;
    inline constexpr int Nine = 9;
    inline constexpr int Ten = 10;
    inline constexpr int Jack = 11;
    inline constexpr int Queen = 12;
    inline constexpr int King = 13;
    inline constexpr int Ace = 14;



    // Extract rank from card encoding
    constexpr int RANK(int x) noexcept {
        return (x >> 8) & 0xF;
    }

    // Type aliases
    using Deck = std::array<int, 52>;
    using Hand = std::span<const int>;  // Change from std::span<int>


    // Function declarations
    inline void shuffle_deck(Deck& deck)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::ranges::shuffle(deck, gen);
    }

    void print_hand(Hand hand);
    [[nodiscard]] constexpr unsigned find_fast(unsigned u) noexcept;


    [[nodiscard]] constexpr unsigned short eval_5cards(int c1, int c2, int c3, int c4, int c5) noexcept
    {
        // Rank bitmask for unique5/flushes index
        const uint32_t qbits = (c1 | c2 | c3 | c4 | c5) >> 16;

        // Compute "all-same-suit?" once
        const uint32_t suit_mask = (c1 & c2 & c3 & c4 & c5) & 0xF000;

        // Straights & high-card (unique5 non-zero only in those cases)
        if (uint16_t s = unique5[qbits]; s != 0) 
        {
            // Straight flush? Only if all suits match.
            if (suit_mask) return flushes[qbits];
            return s;
        }

        int q = qbits;
        // Perfect-hash lookup for remaining hands
        q = (c1 & 0xff) * (c2 & 0xff) * (c3 & 0xff) * (c4 & 0xff) * (c5 & 0xff);
        return hash_values[find_fast(q)];
    }

    // Evaluate the best five-card hand from seven cards
   // Uses brute-force enumeration of all 21 combinations
    inline unsigned short eval_7hand(Hand hand)
    {
        unsigned short best = 9999;

        for (const auto& perm : perm7)
        {
            unsigned short q = eval_5cards(
                hand[perm[0]],
                hand[perm[1]],
                hand[perm[2]],
                hand[perm[3]],
                hand[perm[4]]
            );

            if (q < best)
                best = q;
            if (best == 1)
                return 1;  // Royal Flush found
        }
        return best;
    }


    // Constexpr function definitions (must be in header)
    [[nodiscard]] constexpr Deck init_deck() noexcept
    {
        Deck deck{};
        int n = 0;
        int suit = CLUB;

        for (int i = 0; i < 4; ++i, suit >>= 1) {
            for (int j = 0; j < 13; ++j, ++n) {
                deck[n] = primes[j] | ((2 + j) << 8) | suit | (1 << (16 + j));
            }
        }
        return deck;
    }

    [[nodiscard]] constexpr int find_card(int rank, int suit, const Deck& deck) noexcept
    {
        for (int i = 0; i < 52; ++i) {
            int c = deck[i];
            if ((c & suit) && (RANK(c) == rank)) {
                return i;
            }
        }
        return -1;
    }

    [[nodiscard]] constexpr int hand_rank(unsigned short val) noexcept
    {
        if (val > 6185) return HIGH_CARD;        // 1277 high card
        if (val > 3325) return ONE_PAIR;         // 2860 one pair
        if (val > 2467) return TWO_PAIR;         //  858 two pair
        if (val > 1609) return THREE_OF_A_KIND;  //  858 three-kind
        if (val > 1599) return STRAIGHT;         //   10 straights
        if (val > 322)  return FLUSH;            // 1277 flushes
        if (val > 166)  return FULL_HOUSE;       //  156 full house
        if (val > 10)   return FOUR_OF_A_KIND;   //  156 four-kind
        return STRAIGHT_FLUSH;                   //   10 straight-flushes
    }

    [[nodiscard]] constexpr unsigned find_fast(unsigned u) noexcept
    {
        u += 0xe91aaa35;
        u ^= u >> 16;
        u += u << 8;
        u ^= u >> 4;
        unsigned b = (u >> 8) & 0x1ff;
        unsigned a = (u + (u << 2)) >> 19;
        unsigned r = a ^ hash_adjust[b];
        return r;
    }

    [[nodiscard]] constexpr unsigned short eval_5hand(Hand hand) noexcept
    {
        return eval_5cards(hand[0], hand[1], hand[2], hand[3], hand[4]);
    }

} // namespace poker

