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

#include <iostream>

#include "../third_party/libfixeypointy/src/decimal.h"

/// @brief
struct FxyPty_Decimal {
    int64_t bytes[2];
    uint32_t scale;
};

/// @brief
/// @param x
/// @return
inline libfixeypointy::Decimal::NativeType _pack128(FxyPty_Decimal *x) {
    return (((__int128_t)x->bytes[1]) << 64) | (__int128_t)x->bytes[0];
}

/// @brief Parse an input string and generate a fxypty object based on the
/// string.
/// @param input The input string.
/// @param scale The scale.
/// @return The pointer to the fxypty object.
extern "C" void *_fxypty_in(char *input, uint64_t scale) {
    FxyPty_Decimal *wrapped_decimal =
        (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));

    wrapped_decimal->scale = (libfixeypointy::Decimal::ScaleType)scale;
    try {
        libfixeypointy::Decimal tmp(std::string(input), wrapped_decimal->scale);
        std::memcpy(wrapped_decimal->bytes, &tmp,
                    sizeof(libfixeypointy::Decimal));
    } catch (std::runtime_error e) {
        return NULL;
    }

    return wrapped_decimal;
}

/// @brief Generate a string from the fxypty object.
/// @param out The output string.
/// @param in The input pointer to the fxypty object.
extern "C" void _fxypty_out(char out[40], void *in) {
    FxyPty_Decimal *decimal = (FxyPty_Decimal *)in;
    __int128_t native_value = _pack128(decimal);

    // Get absolute value
    bool is_negative = native_value < 0;
    __int128_t abs_value = is_negative ? -native_value : native_value;

    // Get fractional part
    __int128_t integral_part = abs_value / decimal->scale;
    __int128_t fractional_part = abs_value % decimal->scale;

    bool is_fractional_zero = fractional_part == 0;
    if (is_fractional_zero) {
        if (is_negative) {
            snprintf(out, 40, "-%lld", integral_part);
        } else {
            snprintf(out, 40, "%lld", integral_part);
        }
    } else {
        if (is_negative) {
            snprintf(out, 40, "-%lld.%lld", integral_part, fractional_part);
        } else {
            snprintf(out, 40, "%lld.%lld", integral_part, fractional_part);
        }
    }
}

/// @brief Add two fxypty objects.
/// @param a The pointer to the first fxypty object.
/// @param b The pointer to the second fxypty object.
/// @return The pointer to the new fxypty object containing the sum of both the
/// fxypty objects.
extern "C" void *_fxypty_add(void *a, void *b) {
    FxyPty_Decimal *wrapped_a = (FxyPty_Decimal *)a;
    FxyPty_Decimal *wrapped_b = (FxyPty_Decimal *)b;

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->scale = wrapped_a->scale;

    try {
        libfixeypointy::Decimal tmp(_pack128(wrapped_a));
        tmp.Add(libfixeypointy::Decimal(_pack128(wrapped_b)));
        std::memcpy(result->bytes, &tmp, sizeof(libfixeypointy::Decimal));
    } catch (std::runtime_error e) {
        return NULL;
    }

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

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->scale = wrapped_a->scale;

    try {
        libfixeypointy::Decimal tmp(_pack128(wrapped_a));
        tmp.Subtract(libfixeypointy::Decimal(_pack128(wrapped_b)));
        std::memcpy(result->bytes, &tmp, sizeof(libfixeypointy::Decimal));
    } catch (std::runtime_error e) {
        return NULL;
    }

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

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->scale = wrapped_a->scale;

    try {
        libfixeypointy::Decimal tmp(_pack128(wrapped_a));
        tmp.Multiply(libfixeypointy::Decimal(_pack128(wrapped_b)),
                     result->scale);
        std::memcpy(result->bytes, &tmp, sizeof(libfixeypointy::Decimal));
    } catch (std::runtime_error e) {
        return NULL;
    }

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

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->scale = wrapped_a->scale;

    try {
        libfixeypointy::Decimal tmp(_pack128(wrapped_a));
        tmp.Divide(libfixeypointy::Decimal(_pack128(wrapped_b)), result->scale);
        std::memcpy(result->bytes, &tmp, sizeof(libfixeypointy::Decimal));
    } catch (std::runtime_error e) {
        return NULL;
    }

    return (void *)result;
}

/// @brief Compare two fxypty objects.
/// @param a The pointer to the first fxypty object.
/// @param b The pointer to the second fxypty object.
/// @return Return 0 when both the values are equivalent. Return 1 when the
/// first value are greater. Return -1 when the first value are lesser.
extern "C" int _fxypty_compare(void *a, void *b) {
    FxyPty_Decimal *wrapped_a = (FxyPty_Decimal *)a;
    FxyPty_Decimal *wrapped_b = (FxyPty_Decimal *)b;

    int compare_result = 0;
    try {
        libfixeypointy::Decimal tmp(_pack128(wrapped_a));
        compare_result =
            tmp.Compare(libfixeypointy::Decimal(_pack128(wrapped_b)));
    } catch (std::runtime_error e) {
        return 0;
    }

    return compare_result;
}