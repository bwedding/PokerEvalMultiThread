#include <array>
#include <vector>
#include <algorithm>
#include <print>
#include <cstdint>

// Check if bit pattern represents a straight
constexpr bool is_straight(uint16_t bits) {
    // Check for wheel (A-2-3-4-5): bits 0,1,2,3,12
    if (bits == 0x100F) return true;

    // Check for regular straights (5 consecutive bits)
    for (int i = 0; i <= 8; ++i) {
        if ((bits & (0x1F << i)) == (0x1F << i)) {
            return true;
        }
    }
    return false;
}

// Count number of set bits
constexpr int popcount(uint16_t x) {
    int count = 0;
    while (x) {
        count += x & 1;
        x >>= 1;
    }
    return count;
}

// Compare: higher bit patterns (higher cards) come first
bool compare_hands(uint16_t a, uint16_t b) {
    return a > b;
}

int main() {
    constexpr int ARRAY_SIZE = 7937;
    std::array<uint16_t, ARRAY_SIZE> flushes{};

    // Collect all valid 5-card combinations
    std::vector<uint16_t> straight_flushes;
    std::vector<uint16_t> regular_flushes;

    for (uint16_t bits = 0; bits < 0x2000; ++bits) {
        if (popcount(bits) == 5) {
            if (is_straight(bits)) {
                straight_flushes.push_back(bits);
            }
            else {
                regular_flushes.push_back(bits);
            }
        }
    }

    // Sort both lists: best hands (highest bit patterns) first
    std::ranges::sort(straight_flushes, compare_hands);
    std::ranges::sort(regular_flushes, compare_hands);

    // Assign straight flush values: 1-10
    uint16_t sf_value = 1;
    for (uint16_t bits : straight_flushes) {
        flushes[bits] = sf_value++;
    }

    // Assign regular flush values: 323-1599
    uint16_t flush_value = 323;
    for (uint16_t bits : regular_flushes) {
        flushes[bits] = flush_value++;
    }

    // Print the array
    std::println("inline constexpr std::array<uint16_t, {}> flushes = {{{{", ARRAY_SIZE);

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        if (i % 16 == 0) std::print("    ");
        std::print("{:4}", flushes[i]);
        if (i < ARRAY_SIZE - 1) std::print(",");
        if (i % 16 == 15) std::println("");
    }

    std::println("}}}};");
    std::println("");

    // Verification output
    std::println("// Verification:");
    std::println("// Royal Flush (AKQJT): bits=0x1F00 ({}), value={}", 0x1F00, flushes[0x1F00]);
    std::println("// Five-high SF (5432A): bits=0x100F ({}), value={}", 0x100F, flushes[0x100F]);
    std::println("// Six-high SF (65432): bits=0x001F ({}), value={}", 0x001F, flushes[0x001F]);
    std::println("// AKQJ9 Flush: bits=0x1E80 ({}), value={}", 0x1E80, flushes[0x1E80]);
    std::println("// 76432 Flush: bits=0x002F ({}), value={}", 0x002F, flushes[0x002F]);
    std::println("");
    std::println("// Total straight flushes: {}", sf_value - 1);
    std::println("// Total flushes: {} (should be 1277)", flush_value - 323);

    return 0;
}