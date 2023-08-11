#include <chrono>
#include <iostream>
#include <vector>

#include "../../third_party/libfixeypointy/src/decimal.h"

constexpr int N = 10000;
constexpr int EPOCH = 100;

int main() {
    std::vector<libfixeypointy::Decimal> data(N);

    for (int i = 0; i < N; i++) {
        data[i] = libfixeypointy::Decimal(((__int128_t)rand() << 64) |
                                          ((__int128_t)rand()));
    }

    for (int frac_digit = 0; frac_digit < 38; frac_digit++) {
        for (int j = 0; j < EPOCH; j++) {
            for (int i = 0; i < N; i++) {
                data[i].ToString(frac_digit);
                data[i].m_ToString(frac_digit);
            }
        }
    }

    return 0;
}