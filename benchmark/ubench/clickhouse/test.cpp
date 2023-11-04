#include <iostream>

#include "Decimal.h"
#include "WriteHelpers.h"

int main() {
    DB::Decimal128 a(12345);
    DB::Decimal128 b(54321);
    DB::Decimal128 c = a * b;

    std::cout << c.convertTo<Int64>() << std::endl;

    char *buffer = (char *)malloc(128 * sizeof(char));
    DB::WriteBufferFromPointer ostr(buffer, 128);
    DB::writeText<Int128>(c, 5, ostr, false, false, 0);

    std::cout << buffer << std::endl;

    return 0;
}