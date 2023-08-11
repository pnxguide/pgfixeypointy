#include <iostream>

#include <arrow/util/decimal.h>

// export LD_LIBRARY_PATH=/home/ykerdcha/arrow/cpp/build/release/;

int main() {
    std::cout << "Hello" << std::endl;

    arrow::Decimal128 test("54.12356");
    std::cout << test.ToString(5) << std::endl;

    return 0;
}