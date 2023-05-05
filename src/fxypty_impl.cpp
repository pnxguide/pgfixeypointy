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
    int64_t hi, lo;
    uint32_t scale;
};

/// @brief
/// @param x
/// @return
inline libfixeypointy::Decimal::NativeType _pack128(FxyPty_Decimal *x) {
    return (((__int128_t)x->hi) << 64) | (__int128_t)x->lo;
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
    libfixeypointy::Decimal tmp(std::string(input), wrapped_decimal->scale);
    libfixeypointy::Decimal::NativeType lo =
        tmp.ToNative() &
        (libfixeypointy::Decimal::NativeType)0xffffffffffffffff;
    libfixeypointy::Decimal::NativeType hi =
        (tmp.ToNative() >> 64) &
        (libfixeypointy::Decimal::NativeType)0xffffffffffffffff;
    wrapped_decimal->lo = lo;
    wrapped_decimal->hi = hi;

    return wrapped_decimal;
}

/// @brief Generate a string from the fxypty object.
/// @param out The output string.
/// @param in The input pointer to the fxypty object.
extern "C" void _fxypty_out(char out[64], void *in) {
    FxyPty_Decimal *decimal = (FxyPty_Decimal *)in;
    libfixeypointy::Decimal tmp(_pack128(decimal));
    std::strncpy(out, tmp.ToString(decimal->scale).c_str(), 64);
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
    result->scale = wrapped_a->scale;

    try {
        libfixeypointy::Decimal tmp(_pack128(wrapped_a));
        tmp.Add(libfixeypointy::Decimal(_pack128(wrapped_b)));
        result->lo = tmp.ToNative() &
                     (libfixeypointy::Decimal::NativeType)0xffffffffffffffff;
        result->hi = (tmp.ToNative() >> 64) &
                     (libfixeypointy::Decimal::NativeType)0xffffffffffffffff;
    } catch (std::runtime_error e) {
        return nullptr;
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
    assert(wrapped_a->scale == wrapped_b->scale);

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->scale = wrapped_a->scale;

    try {
        libfixeypointy::Decimal tmp(_pack128(wrapped_a));
        tmp.Subtract(libfixeypointy::Decimal(_pack128(wrapped_b)));
        result->lo = tmp.ToNative() &
                     (libfixeypointy::Decimal::NativeType)0xffffffffffffffff;
        result->hi = (tmp.ToNative() >> 64) &
                     (libfixeypointy::Decimal::NativeType)0xffffffffffffffff;
    } catch (std::runtime_error e) {
        return nullptr;
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
    assert(wrapped_a->scale == wrapped_b->scale);

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->scale = wrapped_a->scale;

    try {
        libfixeypointy::Decimal tmp(_pack128(wrapped_a));
        tmp.Multiply(libfixeypointy::Decimal(_pack128(wrapped_b)), result->scale);
        result->lo = tmp.ToNative() &
                     (libfixeypointy::Decimal::NativeType)0xffffffffffffffff;
        result->hi = (tmp.ToNative() >> 64) &
                     (libfixeypointy::Decimal::NativeType)0xffffffffffffffff;
    } catch (std::runtime_error e) {
        return nullptr;
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
    assert(wrapped_a->scale == wrapped_b->scale);

    FxyPty_Decimal *result = (FxyPty_Decimal *)palloc(sizeof(FxyPty_Decimal));
    result->scale = wrapped_a->scale;

    try {
        libfixeypointy::Decimal tmp(_pack128(wrapped_a));
        tmp.Divide(libfixeypointy::Decimal(_pack128(wrapped_b)), result->scale);
        result->lo = tmp.ToNative() &
                     (libfixeypointy::Decimal::NativeType)0xffffffffffffffff;
        result->hi = (tmp.ToNative() >> 64) &
                     (libfixeypointy::Decimal::NativeType)0xffffffffffffffff;
    } catch (std::runtime_error e) {
        return nullptr;
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
    assert(wrapped_a->scale == wrapped_b->scale);

    libfixeypointy::Decimal::NativeType native_value;

    try {
        libfixeypointy::Decimal tmp(_pack128(wrapped_a));
        tmp.Subtract(libfixeypointy::Decimal(_pack128(wrapped_b)));
        native_value = tmp.ToNative();
    } catch (std::runtime_error e) {
        return 0;
    }

    int compare_result = 0;
    if (native_value > 0) {
        compare_result = 1;
    } else if (native_value < 0) {
        compare_result = -1;
    }

    return compare_result;
}