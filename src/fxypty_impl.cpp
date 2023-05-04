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
#include "utils/array.h"

/// @brief
struct FxyPty_Decimal {
    libfixeypointy::Decimal *decimal;
    uint32_t scale;
};

/// @brief
/// @param input The string represented the decimal
/// @return The pointer to the decimal struct
extern "C" void *_fxypty_in(char *input, uint64_t scale) {
    FxyPty_Decimal *wrapped_decimal =
        (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    wrapped_decimal->scale = (libfixeypointy::Decimal::ScaleType)scale;
    wrapped_decimal->decimal =
        new libfixeypointy::Decimal(std::string(input), wrapped_decimal->scale);
    return wrapped_decimal;
}

/// @brief
/// @param out
/// @param in
extern "C" void _fxypty_out(char out[64], void *in) {
    FxyPty_Decimal *decimal = (FxyPty_Decimal *)in;
    std::strncpy(out, decimal->decimal->ToString(decimal->scale).c_str(), 64);
}

/// @brief
/// @param a
/// @param b
/// @return
extern "C" void *_fxypty_add(void *a, void *b) {
    FxyPty_Decimal *wrapped_a = (FxyPty_Decimal *)a;
    FxyPty_Decimal *wrapped_b = (FxyPty_Decimal *)b;
    assert(wrapped_a->scale == wrapped_b->scale);

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->decimal = new libfixeypointy::Decimal(*(wrapped_a->decimal));
    result->scale = wrapped_a->scale;
    result->decimal->Add(*(wrapped_b->decimal));

    return (void *)result;
}

extern "C" void *_fxypty_subtract(void *a, void *b) {
    FxyPty_Decimal *wrapped_a = (FxyPty_Decimal *)a;
    FxyPty_Decimal *wrapped_b = (FxyPty_Decimal *)b;
    assert(wrapped_a->scale == wrapped_b->scale);

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->decimal = new libfixeypointy::Decimal(*(wrapped_a->decimal));
    result->scale = wrapped_a->scale;
    result->decimal->Subtract(*(wrapped_b->decimal));

    return (void *)result;
}

extern "C" void *_fxypty_multiply(void *a, void *b) {
    FxyPty_Decimal *wrapped_a = (FxyPty_Decimal *)a;
    FxyPty_Decimal *wrapped_b = (FxyPty_Decimal *)b;
    assert(wrapped_a->scale == wrapped_b->scale);

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->decimal = new libfixeypointy::Decimal(*(wrapped_a->decimal));
    result->scale = wrapped_a->scale;
    result->decimal->Multiply(*(wrapped_b->decimal), wrapped_a->scale);

    return (void *)result;
}

extern "C" void *_fxypty_divide(void *a, void *b) {
    FxyPty_Decimal *wrapped_a = (FxyPty_Decimal *)a;
    FxyPty_Decimal *wrapped_b = (FxyPty_Decimal *)b;
    assert(wrapped_a->scale == wrapped_b->scale);

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->decimal = new libfixeypointy::Decimal(*(wrapped_a->decimal));
    result->scale = wrapped_a->scale;
    result->decimal->Divide(*(wrapped_b->decimal), wrapped_a->scale);

    return (void *)result;
}

extern "C" int _fxypty_compare(void *a, void *b) {
    libfixeypointy::Decimal::NativeType native_value;
    int compare_result;
    
    FxyPty_Decimal *wrapped_a = (FxyPty_Decimal *)a;
    FxyPty_Decimal *wrapped_b = (FxyPty_Decimal *)b;
    assert(wrapped_a->scale == wrapped_b->scale);

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->decimal = new libfixeypointy::Decimal(*(wrapped_a->decimal));
    result->scale = wrapped_a->scale;
    result->decimal->Subtract(*(wrapped_b->decimal));

    native_value = result->decimal->ToNative();
    if (native_value > 0) {
        compare_result = 1;
    } else if (native_value < 0) {
        compare_result = -1;
    } else {
        compare_result = 0;
    }

    pfree(result);

    return compare_result;
}

// extern "C" void *_fxypty_accum(ArrayType *transarray, void *val) {
//     FxyPty_Decimal *wrapped_arr = (FxyPty_Decimal *) ARR_DATA_PTR(transarray);
//     FxyPty_Decimal *wrapped_val = (FxyPty_Decimal *) val;
//     assert(wrapped_arr[0].scale == wrapped_val->scale);
//     assert(wrapped_arr[1].scale == wrapped_val->scale);

//     wrapped_arr[0].decimal->Add(*(wrapped_val->decimal));

//     char * const_one_char = (char*)palloc(1);
//     memset(const_one_char, 1, sizeof(const_one_char));
//     FxyPty_Decimal *const_one = (FxyPty_Decimal *)_fxypty_in(const_one_char, wrapped_val->scale);
//     wrapped_arr[1].decimal->Add(*const_one->decimal);
//     pfree(const_one_char);
//     // would be better if we have +1

//     // assemble result

//     return (void *)result;
// }