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

/// @brief
struct FxyPty_Decimal {
    libfixeypointy::Decimal *decimal;
    uint32_t scale;
};

/// @brief Parse an input string and generate a fxypty object based on the
/// string.
/// @param input The input string.
/// @param scale The scale.
/// @return The pointer to the fxypty object.
extern "C" void *_fxypty_in(char *input, uint64_t scale) {
    FxyPty_Decimal *wrapped_decimal =
        (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    wrapped_decimal->scale = (libfixeypointy::Decimal::ScaleType)scale;
    wrapped_decimal->decimal =
        new libfixeypointy::Decimal(std::string(input), wrapped_decimal->scale);
    return wrapped_decimal;
}

/// @brief Generate a string from the fxypty object.
/// @param out The output string.
/// @param in The input pointer to the fxypty object.
extern "C" void _fxypty_out(char out[64], void *in) {
    FxyPty_Decimal *decimal = (FxyPty_Decimal *)in;
    std::strncpy(out, decimal->decimal->ToString(decimal->scale).c_str(), 64);
}

/// @brief Add two fxypty objects.
/// @param a The pointer to the first fxypty object.
/// @param b The pointer to the second fxypty object.
/// @return The pointer to the new fxypty object containing the sum of both the
/// fxypty objects.
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

/// @brief Subtract two fxypty objects.
/// @param a The pointer to the first fxypty object.
/// @param b The pointer to the second fxypty object.
/// @return The pointer to the new fxypty object containing the difference of
/// both the fxypty objects.
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

/// @brief Multiply two fxypty objects.
/// @param a The pointer to the first fxypty object.
/// @param b The pointer to the second fxypty object.
/// @return The pointer to the new fxypty object containing the product of both
/// the fxypty objects.
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

/// @brief Divide two fxypty objects.
/// @param a The pointer to the first fxypty object.
/// @param b The pointer to the second fxypty object.
/// @return The pointer to the new fxypty object containing the quotient of both
/// the fxypty objects.
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

/// @brief Compare two fxypty objects.
/// @param a The pointer to the first fxypty object.
/// @param b The pointer to the second fxypty object.
/// @return Return 0 when both the values are equivalent. Return 1 when the
/// first value are greater. Return -1 when the first value are lesser.
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