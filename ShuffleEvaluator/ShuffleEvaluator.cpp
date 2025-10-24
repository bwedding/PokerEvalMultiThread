#include <print>
#include "Poker.h"
#include "arrays.h"

int main() {
    auto deck = poker::init_deck();

    std::println("First few cards in deck:");
    for (int i = 0; i < 13; ++i) {
        int card = deck[i];
        int rank = (card >> 8) & 0xF;
        int prime = card & 0xFF;
        int suit_bits = card & 0xF000;
        int rank_bits = (card >> 16);

        char suit = '?';
        if (suit_bits & poker::CLUB) suit = 'c';
        else if (suit_bits & poker::DIAMOND) suit = 'd';
        else if (suit_bits & poker::HEART) suit = 'h';
        else if (suit_bits & poker::SPADE) suit = 's';

        std::println("Card {}: 0x{:08X} - Rank: {}, Prime: {}, Suit: {}, RankBits: 0x{:04X}",
            i, card, rank, prime, suit, rank_bits);
    }

    // Test a known hand: Royal Flush in clubs
    std::println("\nTesting Royal Flush in Clubs:");
    int ten_c = poker::find_card(poker::Ten, poker::CLUB, deck);
    int jack_c = poker::find_card(poker::Jack, poker::CLUB, deck);
    int queen_c = poker::find_card(poker::Queen, poker::CLUB, deck);
    int king_c = poker::find_card(poker::King, poker::CLUB, deck);
    int ace_c = poker::find_card(poker::Ace, poker::CLUB, deck);

    std::println("Found cards at indices: {} {} {} {} {}", ten_c, jack_c, queen_c, king_c, ace_c);

    std::array<int, 5> royal_flush = { deck[ten_c], deck[jack_c], deck[queen_c], deck[king_c], deck[ace_c] };

    // Check flush detection
    int flush_check = royal_flush[0] & royal_flush[1] & royal_flush[2] & royal_flush[3] & royal_flush[4] & 0xF000;
    std::println("Flush check (should be non-zero): 0x{:04X}", flush_check);

    // Check bit pattern
    int q = (royal_flush[0] | royal_flush[1] | royal_flush[2] | royal_flush[3] | royal_flush[4]) >> 16;
    std::println("Bit pattern q = 0x{:04X} ({}), should be 0x1F00 ({})", q, q, 0x1F00, 0x1F00);
    std::println("flushes[q] = {}", poker::flushes[q]);

    unsigned short value = poker::eval_5hand(royal_flush);
    int rank = poker::hand_rank(value);

    std::println("Royal Flush value: {}, rank: {} ({})", value, rank, poker::value_str[rank]);

    return 0;
}