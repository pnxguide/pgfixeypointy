#include "Decimal.h"
#include <iostream>

int main() {
    DB::Decimal128 a(10000);
    DB::Decimal128 b(20000);
    DB::Decimal128 c = a * b;

    std::cout << c.convertTo<Int64>() << std::endl;

    return 0;
}