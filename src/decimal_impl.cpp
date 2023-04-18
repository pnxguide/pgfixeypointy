#include "decimal_impl.h"

#include "../third_party/libfixeypointy/src/decimal.h"

// using namespace libfixeypointy;

// struct TypeDecimal {
//     Decimal *decimal = nullptr;
//     uint32_t scale;

//     TypeDecimal(Decimal *data, uint32_t scale) {
//         this->decimal = data;
//         this->scale = scale;
//     }

//     TypeDecimal(char *input, uint32_t scale) {
//         this->decimal = new Decimal(std::string(input), scale);
//         this->scale = scale;
//     }

//     ~TypeDecimal() {
//         if (decimal != nullptr) delete decimal;
//     }
// }

extern "C" void *_decimal_in(char *input, uint32_t scale) {
    return new libfixeypointy::Decimal(input, scale);
}

extern "C" const char *_decimal_out(void *in) {
    libfixeypointy::Decimal *in_decimal = (libfixeypointy::Decimal *)in;
    std::string in_string = in_decimal->ToString(libfixeypointy::Decimal::DEFAULT_SCALE);
    return in_string.c_str();
}

// extern "C" void *decimal_add_impl(void *a, void *b) {
//     TypeDecimal *a_val = (TypeDecimal *)a;
//     TypeDecimal *b_val = (TypeDecimal *)b;

//     uint32_t new_scale = Decimal::MatchScales(a_val->decimal, b_val->decimal, a_val->scale, b_val->scale);
//     TypeDecimal *result = new TypeDecimal(*(a_val->decimal), new_scale);
//     *(result->decimal) += *(b_val->decimal);

//     return (void *) result;
// }

// extern "C" void *decimal_sub_impl(void *a, void *b) {
//     TypeDecimal *a_val = (TypeDecimal *)a;
//     TypeDecimal *b_val = (TypeDecimal *)b;

//     uint32_t new_scale = Decimal::MatchScales(a_val->decimal, b_val->decimal, a_val->scale, b_val->scale);
//     TypeDecimal *result = new TypeDecimal(*(a_val->decimal), new_scale);
//     *(result->decimal) -= *(b_val->decimal);

//     return (void *) result;
// }

// extern "C" void *decimal_mul_impl(void *a, void *b) {
//     TypeDecimal *a_val = (TypeDecimal *)a;
//     TypeDecimal *b_val = (TypeDecimal *)b;

//     uint32_t new_scale = Decimal::MatchScales(a_val->decimal, b_val->decimal, a_val->scale, b_val->scale);
//     TypeDecimal *result = new TypeDecimal(*(a_val->decimal), new_scale);
//     *(result->decimal).Multiply(*(b_val->decimal), new_scale);

//     return (void *) result;
// }

// extern "C" void *decimal_div_impl(void *a, void *b) {
//     TypeDecimal *a_val = (TypeDecimal *)a;
//     TypeDecimal *b_val = (TypeDecimal *)b;

//     uint32_t new_scale = Decimal::MatchScales(a_val->decimal, b_val->decimal, a_val->scale, b_val->scale);
//     TypeDecimal *result = new TypeDecimal(*(a_val->decimal), new_scale);
//     *(result->decimal).Divide(*(b_val->decimal), new_scale);

//     return (void *) result;
// }

// extern "C" int decimal_cmp_impl(void *a, void *b) {
//     TypeDecimal *a_val = (TypeDecimal *)a;
//     TypeDecimal *b_val = (TypeDecimal *)b;

//     return *(a_val->decimal).Compare(*(b_val->decimal));
// }