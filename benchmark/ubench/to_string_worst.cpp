#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#include "../../third_party/libfixeypointy/src/decimal.h"
#include "suffix128.h"

extern "C" {
#include "../../third_party/pg_numeric/pg_numeric.h"
}

constexpr __int128_t HI_128 = 1000000000000000000_128;
constexpr int EPOCH = 10000;

int main() {
    int precision = 21;
    {
    // for (int precision = 23; precision < 23; precision++) {
        std::ifstream in_file("../workload/data_" + std::to_string(precision) +
                              ".in");
        int N = 0;
        in_file >> N;

        std::vector<libfixeypointy::Decimal> A(N);
        std::vector<std::string> B(N);

        long A_hi, A_lo, B_hi, B_lo;
        for (int i = 0; i < N; i++) {
            in_file >> A_hi >> A_lo >> B_hi >> B_lo;
            __int128_t A_128 =
                (((__int128_t)(A_hi)) * HI_128) + ((__int128_t)(A_lo));
            A[i] = libfixeypointy::Decimal(A_128);
            B[i] = std::to_string(A_hi) + std::to_string(A_lo);
        }

        std::chrono::_V2::steady_clock::time_point start, end;
        std::chrono::duration<double> elapsed;

        int frac_digit = 15;
        {
        // for (int frac_digit = 16; frac_digit < 16; frac_digit++) {
            std::vector<Numeric> B_numeric(N);
            for (int i = 0; i < N; i++) {
                B_numeric[i] = numeric_in(A[i].ToString(frac_digit).c_str());
            }

            for (int j = 0; j < EPOCH; j++) {
                for (int i = 0; i < N; i++) {
                    // start = std::chrono::steady_clock::now();
                    A[i].m_ToCharArray(frac_digit);
                    // end = std::chrono::steady_clock::now();
                    // elapsed = end - start;
                    // runtime1 += elapsed.count();

                    // start = std::chrono::steady_clock::now();
                    // A[i].m_ToString(frac_digit);
                    // end = std::chrono::steady_clock::now();
                    // elapsed = end - start;
                    // runtime2 += elapsed.count();

                    // start = std::chrono::steady_clock::now();
                    Numeric_ToString(B_numeric[i]);
                    // end = std::chrono::steady_clock::now();
                    // elapsed = end - start;
                    // runtime2 += elapsed.count();
                }
            }
        }
    }

    return 0;
}