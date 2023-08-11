#include <chrono>
#include <iostream>
#include <vector>

#include "../../third_party/libfixeypointy/src/decimal.h"

constexpr int N = 10000;
constexpr int EPOCH = 100;

int main() {
    std::vector<libfixeypointy::Decimal> A(N);
    std::vector<libfixeypointy::Decimal> B(N);

    for (int i = 0; i < N; i++) {
        A[i] = libfixeypointy::Decimal(((__int128_t)rand() << 64) |
                                       ((__int128_t)rand()));
        B[i] = libfixeypointy::Decimal(((__int128_t)rand() << 64) |
                                       ((__int128_t)rand()));
    }

    libfixeypointy::Decimal X("0", 1);

    // for (int frac_digit = 12; frac_digit < 12; frac_digit++) {
    for (int frac_digit = 0; frac_digit < 34; frac_digit++) {
        for (int j = 0; j < EPOCH; j++) {
            std::vector<libfixeypointy::Decimal> C = A;
            for (int i = 0; i < N; i++) {
                C[i].Divide(B[i], frac_digit);
                X.Add(C[i]);
            }
        }
    }

    X.ToString(1);
    return 0;
}