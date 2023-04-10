#include "decimal_impl.h"

#include "../third_party/libfixeypointy/src/decimal.h"

extern "C" void *decimal_in_impl(char *input, uint8_t scale) {
    return new Decimal(std::string(input), scale);
}

extern "C" const char *decimal_out_impl(void *in) {
    Decimal *in_decimal = (Decimal *)in;
    return in_decimal->ToString().cstr();
}