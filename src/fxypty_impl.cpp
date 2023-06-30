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

#include "digits.hpp"

#include <iostream>

#include "../third_party/jeaiii_to_text.h"
#include "../third_party/libfixeypointy/src/decimal.h"
#include "../third_party/suffix128.h"

/// @brief
struct FxyPty_Decimal {
    int64_t bytes[2];
    uint32_t scale;
};

/// @brief
/// @param x
/// @return
inline __int128_t _pack128(FxyPty_Decimal *x) {
    return (((__int128_t)x->bytes[0] << 0) & 0xFFFFFFFFFFFFFFFF) |
           (((__int128_t)x->bytes[1] << 64));
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                      \
    ((byte)&0x8000 ? '1' : '0'), ((byte)&0x4000 ? '1' : '0'),     \
        ((byte)&0x2000 ? '1' : '0'), ((byte)&0x1000 ? '1' : '0'), \
        ((byte)&0x800 ? '1' : '0'), ((byte)&0x400 ? '1' : '0'),   \
        ((byte)&0x200 ? '1' : '0'), ((byte)&0x100 ? '1' : '0'),   \
        ((byte)&0x80 ? '1' : '0'), ((byte)&0x40 ? '1' : '0'),     \
        ((byte)&0x20 ? '1' : '0'), ((byte)&0x10 ? '1' : '0'),     \
        ((byte)&0x08 ? '1' : '0'), ((byte)&0x04 ? '1' : '0'),     \
        ((byte)&0x02 ? '1' : '0'), ((byte)&0x01 ? '1' : '0')

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
        std::memcpy(wrapped_decimal->bytes, &(tmp.value_), sizeof(__int128_t));
    } catch (std::runtime_error e) {
        return NULL;
    }

    return wrapped_decimal;
}

// TODO: Flame graph
/// @brief Generate a string from the fxypty object.
/// @param out The output string.
/// @param in The input pointer to the fxypty object.
extern "C" void _fxypty_out(char out[42], void *in) {
    FxyPty_Decimal *decimal = (FxyPty_Decimal *)in;
    __int128_t native_value = _pack128(decimal);

    // Get absolute value
    bool is_negative = native_value < 0;
    __int128_t abs_value = is_negative ? -native_value : native_value;

    // TODO: Type 7 to do fast division and modulo 
    int64_t hi = (int64_t)(abs_value / POWER_OF_TEN[18]);
    int64_t lo = (int64_t)(abs_value % POWER_OF_TEN[18]);

    // temporary buffer
    char tmp[42];
    tmp[41] = '\0';

    // digit pointer
    int cur_digit = 41;

    // Case 1 - hi == 0; only process lo
    if (hi == 0) {
        // deplete lo
        int fractional_digit = decimal->scale;

        while (fractional_digit % 4 != 0) {
            tmp[--cur_digit] = (lo % 10) + '0';
            lo /= 10;
            fractional_digit--;
        }

        while (fractional_digit > 0) {
            cur_digit -= 4;
            std::memcpy(&(tmp[cur_digit]), FOUR_DIGITS[(lo % 10000)], 4);
            lo /= 10000;
            fractional_digit -= 4;
        }

        // if there is no integral part
        if (lo == 0) {
            if (is_negative) {
                out[0] = '-';
                out[1] = '0';
                out[2] = '.';
                std::memcpy(&(out[3]), &(tmp[cur_digit]), 42 - cur_digit);
            } else {
                out[0] = '0';
                out[1] = '.';
                std::memcpy(&(out[2]), &(tmp[cur_digit]), 42 - cur_digit);
            }
            return;
        }

        tmp[--cur_digit] = '.';

        while (lo > 10000) {
            cur_digit -= 4;
            std::memcpy(&(tmp[cur_digit]), FOUR_DIGITS[(lo % 10000)], 4);
            lo /= 10000;
        }

        while (lo > 0) {
            tmp[--cur_digit] = (lo % 10) + '0';
            lo /= 10;
        }

        if (is_negative) {
            out[0] = '-';
            std::memcpy(&(out[1]), &(tmp[cur_digit]), 42 - cur_digit);
        } else {
            std::memcpy(&(out[0]), &(tmp[cur_digit]), 42 - cur_digit);
        }
    }
    // Case 2 - hi != 0; process lo first (18 digits)
    else {
        // process the fractional part
        uint32_t lo_scale = 18;

        // Case 2A - decimal->scale > lo_scale; use all lo then switch to hi
        if (decimal->scale > lo_scale) {
            // deplete lo
            int fractional_digit = lo_scale;

            while (fractional_digit % 4 != 0) {
                tmp[--cur_digit] = (lo % 10) + '0';
                lo /= 10;
                fractional_digit--;
            }

            while (fractional_digit > 0) {
                cur_digit -= 4;
                std::memcpy(&(tmp[cur_digit]), FOUR_DIGITS[(lo % 10000)], 4);
                lo /= 10000;
                fractional_digit -= 4;
            }

            // deplete hi until decimal->scale is gone
            fractional_digit = decimal->scale - lo_scale;

            while (fractional_digit % 4 != 0) {
                tmp[--cur_digit] = (hi % 10) + '0';
                hi /= 10;
                fractional_digit--;
            }

            while (fractional_digit > 0) {
                cur_digit -= 4;
                std::memcpy(&(tmp[cur_digit]), FOUR_DIGITS[(hi % 10000)], 4);
                hi /= 10000;
                fractional_digit -= 4;
            }

            // if there is no integral part
            if (hi == 0) {
                if (is_negative) {
                    out[0] = '-';
                    out[1] = '0';
                    out[2] = '.';
                    std::memcpy(&(out[3]), &(tmp[cur_digit]), 42 - cur_digit);
                } else {
                    out[0] = '0';
                    out[1] = '.';
                    std::memcpy(&(out[2]), &(tmp[cur_digit]), 42 - cur_digit);
                }
                return;
            }

            tmp[--cur_digit] = '.';
        }
        
        // Case 2B - decimal->scale <= lo_scale; use all decimal->scale and stay lo
        else {
            int fractional_digit = decimal->scale;

            while (fractional_digit % 4 != 0) {
                tmp[--cur_digit] = (lo % 10) + '0';
                lo /= 10;
                fractional_digit--;
            }

            while (fractional_digit > 0) {
                cur_digit -= 4;
                std::memcpy(&(tmp[cur_digit]), FOUR_DIGITS[(lo % 10000)], 4);
                lo /= 10000;
                fractional_digit -= 4;
            }

            // dot
            tmp[--cur_digit] = '.';

            // deplete lo
            lo_scale = 18 - decimal->scale;

            while (lo_scale % 4 != 0) {
                tmp[--cur_digit] = (lo % 10) + '0';
                lo /= 10;
                lo_scale--;
            }

            while (lo_scale > 0) {
                cur_digit -= 4;
                std::memcpy(&(tmp[cur_digit]), FOUR_DIGITS[(lo % 10000)], 4);
                lo /= 10000;
                lo_scale -= 4;
            }
        }

        while (hi > 10000) {
            cur_digit -= 4;
            std::memcpy(&(tmp[cur_digit]), FOUR_DIGITS[(hi % 10000)], 4);
            hi /= 10000;
        }

        while (hi > 0) {
            tmp[--cur_digit] = (hi % 10) + '0';
            hi /= 10;
        }

        if (is_negative) {
            out[0] = '-';
            std::memcpy(&(out[1]), &(tmp[cur_digit]), 42 - cur_digit);
        } else {
            std::memcpy(&(out[0]), &(tmp[cur_digit]), 42 - cur_digit);
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
