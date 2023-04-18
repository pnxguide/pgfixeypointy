extern "C" {
#include "postgres.h"
}

#undef vsnprintf
#undef snprintf
#undef vsprintf
#undef sprintf
#undef vfprintf
#undef fprintf
#undef vprintf
#undef printf
#undef gettext
#undef dgettext
#undef ngettext
#undef dngettext

#include "../third_party/libfixeypointy/src/decimal.h"

struct FxyPty_Decimal {
    libfixeypointy::Decimal *decimal;
    uint32_t scale;
};

extern "C" void *_fxypty_in(char *input) {
    FxyPty_Decimal *decimal = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    decimal->decimal = new libfixeypointy::Decimal(input, libfixeypointy::Decimal::DEFAULT_SCALE);
    decimal->scale = libfixeypointy::Decimal::DEFAULT_SCALE;
    return decimal;
}

extern "C" const char *_fxypty_out(void *in) {
    FxyPty_Decimal *decimal = (FxyPty_Decimal *)in;
    std::string in_string = decimal->decimal->ToString(decimal->scale);
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