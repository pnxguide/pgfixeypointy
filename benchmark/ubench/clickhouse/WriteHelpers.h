#include <algorithm>
#include <bit>
#include <concepts>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <limits>

#include "DecimalFunctions.h"
#include "WriteBuffer.h"
#include "WriteIntText.h"

namespace DB {

inline void writeChar(char x, WriteBuffer& buf) {
    buf.nextIfAtEnd();
    *buf.position() = x;
    ++buf.position();
}

template <typename T>
void writeDecimalFractional(const T& x, UInt32 scale, WriteBuffer& ostr,
                            bool trailing_zeros, bool fixed_fractional_length,
                            UInt32 fractional_length) {
    /// If it's big integer, but the number of digits is small,
    /// use the implementation for smaller integers for more efficient
    /// arithmetic.
    if constexpr (std::is_same_v<T, Int256>) {
        if (x <= std::numeric_limits<UInt32>::max()) {
            writeDecimalFractional(static_cast<UInt32>(x), scale, ostr,
                                   trailing_zeros, fixed_fractional_length,
                                   fractional_length);
            return;
        } else if (x <= std::numeric_limits<UInt64>::max()) {
            writeDecimalFractional(static_cast<UInt64>(x), scale, ostr,
                                   trailing_zeros, fixed_fractional_length,
                                   fractional_length);
            return;
        } else if (x <= std::numeric_limits<UInt128>::max()) {
            writeDecimalFractional(static_cast<UInt128>(x), scale, ostr,
                                   trailing_zeros, fixed_fractional_length,
                                   fractional_length);
            return;
        }
    } else if constexpr (std::is_same_v<T, Int128>) {
        if (x <= std::numeric_limits<UInt32>::max()) {
            writeDecimalFractional(static_cast<UInt32>(x), scale, ostr,
                                   trailing_zeros, fixed_fractional_length,
                                   fractional_length);
            return;
        } else if (x <= std::numeric_limits<UInt64>::max()) {
            writeDecimalFractional(static_cast<UInt64>(x), scale, ostr,
                                   trailing_zeros, fixed_fractional_length,
                                   fractional_length);
            return;
        }
    }

    constexpr size_t max_digits = std::numeric_limits<UInt256>::digits10;
    assert(scale <= max_digits);
    assert(fractional_length <= max_digits);

    char buf[max_digits];
    memset(buf, '0', std::max(scale, fractional_length));

    T value = x;
    Int32 last_nonzero_pos = 0;

    if (fixed_fractional_length && fractional_length < scale) {
        T new_value = value / DecimalUtils::scaleMultiplier<Int256>(
                                  scale - fractional_length - 1);
        auto round_carry = new_value % 10;
        value = new_value / 10;
        if (round_carry >= 5) value += 1;
    }

    for (Int32 pos = fixed_fractional_length
                         ? std::min(scale - 1, fractional_length - 1)
                         : scale - 1;
         pos >= 0; --pos) {
        auto remainder = value % 10;
        value /= 10;

        if (remainder != 0 && last_nonzero_pos == 0) last_nonzero_pos = pos;

        buf[pos] += static_cast<char>(remainder);
    }

    writeChar('.', ostr);
    ostr.write(buf, fixed_fractional_length
                        ? fractional_length
                        : (trailing_zeros ? scale : last_nonzero_pos + 1));
}

template <typename T>
void writeText(Decimal<T> x, UInt32 scale, WriteBuffer& ostr,
               bool trailing_zeros, bool fixed_fractional_length = false,
               UInt32 fractional_length = 0) {
    T part = DecimalUtils::getWholePart(x, scale);

    if (x.value < 0 && part == 0) {
        writeChar('-',
                  ostr);  /// avoid crop leading minus when whole part is zero
    }

    writeIntText(part, ostr);

    if (scale || (fixed_fractional_length && fractional_length > 0)) {
        part = DecimalUtils::getFractionalPart(x, scale);
        if (part || trailing_zeros) {
            if (part < 0) part *= T(-1);

            writeDecimalFractional(part, scale, ostr, trailing_zeros,
                                   fixed_fractional_length, fractional_length);
        }
    }
}

};  // namespace DB