#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#include "../../third_party/libfixeypointy/src/decimal.h"
#include "suffix128.h"

constexpr __int128_t HI_128 = 1000000000000000000_128;
constexpr int EPOCH = 10;

int main() {
    printf("precision frac_digit runtime1 speedup1 runtime2 speedup2\n");

    for (int precision = 0; precision < 33; precision++) {
        std::ifstream in_file("../workload/data_" + std::to_string(precision) +
                              ".in");
        int N = 0;
        in_file >> N;

        std::vector<libfixeypointy::Decimal> A(N);
        std::vector<libfixeypointy::Decimal> B(N);

        long A_hi, A_lo, B_hi, B_lo;
        for (int i = 0; i < N; i++) {
            in_file >> A_hi >> A_lo >> B_hi >> B_lo;
            __int128_t A_128 =
                (((__int128_t)(A_hi)) * HI_128) + ((__int128_t)(A_lo));
            A[i] = libfixeypointy::Decimal(A_128);
            __int128_t B_128 =
                (((__int128_t)(B_hi)) * HI_128) + ((__int128_t)(B_lo));
            B[i] = libfixeypointy::Decimal(B_128);
        }

        std::chrono::_V2::steady_clock::time_point start, end;
        std::chrono::duration<double> elapsed;

        int practical_precision = precision > 34 ? 34 : precision;
        for (int frac_digit = 0; frac_digit < practical_precision;
             frac_digit++) {
            double runtime1 = 0.0, runtime2 = 0.0;

            for (int j = 0; j < EPOCH; j++) {
                libfixeypointy::Decimal X1("0", 1), X2("0", 1);
                std::vector<libfixeypointy::Decimal> C1 = A, C2 = A;
                for (int i = 0; i < N; i++) {
                    start = std::chrono::steady_clock::now();
                    C1[i].Divide(B[i], frac_digit);
                    end = std::chrono::steady_clock::now();
                    X1.Add(A[i]);
                    elapsed = end - start;
                    runtime1 += elapsed.count();

                    start = std::chrono::steady_clock::now();
                    C2[i].m_Divide(B[i], frac_digit);
                    end = std::chrono::steady_clock::now();
                    X2.Add(A[i]);
                    elapsed = end - start;
                    runtime2 += elapsed.count();
                }
                X1.ToString(frac_digit);
                X2.ToString(frac_digit);
            }

            // printf("Total Digit: %d / Fractional Digit: %d digits\n",
            // precision,
            //        frac_digit);
            // printf("Runtime (Old): %.12f seconds (speedup %fx)\n", runtime1,
            //        runtime1 / runtime1);
            // printf("Runtime (Optimized): %.12f seconds (speedup %fx)\n",
            //        runtime2, runtime1 / runtime2);
            printf("%d %d %.12f %.12f %f %f\n", precision, frac_digit, runtime1,
                   runtime1 / runtime1, runtime2, runtime1 / runtime2);
        }
    }

    return 0;
}