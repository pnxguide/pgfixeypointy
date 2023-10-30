
#include <cstdint>
#include <string>

using Int8 = int8_t;
using Int16 = int16_t;
using Int32 = int32_t;
using Int64 = int64_t;

#ifndef __cpp_char8_t
using char8_t = unsigned char;
#endif

/// This is needed for more strict aliasing. https://godbolt.org/z/xpJBSb
/// https://stackoverflow.com/a/57453713
using UInt8 = char8_t;

using UInt16 = uint16_t;
using UInt32 = uint32_t;
using UInt64 = uint64_t;

using String = std::string;

namespace DB {

using UInt8 = ::UInt8;
using UInt16 = ::UInt16;
using UInt32 = ::UInt32;
using UInt64 = ::UInt64;

using Int8 = ::Int8;
using Int16 = ::Int16;
using Int32 = ::Int32;
using Int64 = ::Int64;

using Float32 = float;
using Float64 = double;

using String = std::string;

}  // namespace DB

///////////////////////////////////////////////////////////////
//  Distributed under the Boost Software License, Version 1.0.
//  (See at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////

/*  Divide and multiply
 *
 *
 * Copyright (c) 2008
 * Evan Teran
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appears in all copies and that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the same name not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission. We make no representations about the
 * suitability this software for any purpose. It is provided "as is"
 * without express or implied warranty.
 */

#include <cstdint>
#include <limits>
#include <type_traits>
#include <initializer_list>

// NOLINTBEGIN(*)

namespace wide
{
template <size_t Bits, typename Signed>
class integer;
}

namespace std
{

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
struct common_type<wide::integer<Bits, Signed>, wide::integer<Bits2, Signed2>>;

template <size_t Bits, typename Signed, typename Arithmetic>
struct common_type<wide::integer<Bits, Signed>, Arithmetic>;

template <typename Arithmetic, size_t Bits, typename Signed>
struct common_type<Arithmetic, wide::integer<Bits, Signed>>;

}

namespace wide
{

template <size_t Bits, typename Signed>
class integer
{
public:
    using base_type = uint64_t;
    using signed_base_type = int64_t;

    // ctors
    constexpr integer() noexcept = default;

    template <typename T>
    constexpr integer(T rhs) noexcept;

    template <typename T>
    constexpr integer(std::initializer_list<T> il) noexcept;

    // assignment
    template <size_t Bits2, typename Signed2>
    constexpr integer<Bits, Signed> & operator=(const integer<Bits2, Signed2> & rhs) noexcept;

    template <typename Arithmetic>
    constexpr integer<Bits, Signed> & operator=(Arithmetic rhs) noexcept;

    template <typename Arithmetic>
    constexpr integer<Bits, Signed> & operator*=(const Arithmetic & rhs);

    template <typename Arithmetic>
    constexpr integer<Bits, Signed> & operator/=(const Arithmetic & rhs);

    template <typename Arithmetic>
    constexpr integer<Bits, Signed> & operator+=(const Arithmetic & rhs) noexcept(std::is_same_v<Signed, unsigned>);

    template <typename Arithmetic>
    constexpr integer<Bits, Signed> & operator-=(const Arithmetic & rhs) noexcept(std::is_same_v<Signed, unsigned>);

    template <typename Integral>
    constexpr integer<Bits, Signed> & operator%=(const Integral & rhs);

    template <typename Integral>
    constexpr integer<Bits, Signed> & operator&=(const Integral & rhs) noexcept;

    template <typename Integral>
    constexpr integer<Bits, Signed> & operator|=(const Integral & rhs) noexcept;

    template <typename Integral>
    constexpr integer<Bits, Signed> & operator^=(const Integral & rhs) noexcept;

    constexpr integer<Bits, Signed> & operator<<=(int n) noexcept;
    constexpr integer<Bits, Signed> & operator>>=(int n) noexcept;

    constexpr integer<Bits, Signed> & operator++() noexcept(std::is_same_v<Signed, unsigned>);
    constexpr integer<Bits, Signed> operator++(int) noexcept(std::is_same_v<Signed, unsigned>);
    constexpr integer<Bits, Signed> & operator--() noexcept(std::is_same_v<Signed, unsigned>);
    constexpr integer<Bits, Signed> operator--(int) noexcept(std::is_same_v<Signed, unsigned>);

    // observers

    constexpr explicit operator bool() const noexcept;

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>, T>>
    constexpr operator T() const noexcept;

    constexpr operator long double() const noexcept;
    constexpr operator double() const noexcept;
    constexpr operator float() const noexcept;

    struct _impl;

    base_type items[_impl::item_count];

private:
    template <size_t Bits2, typename Signed2>
    friend class integer;

    friend class std::numeric_limits<integer<Bits, signed>>;
    friend class std::numeric_limits<integer<Bits, unsigned>>;
};

template <typename T>
static constexpr bool ArithmeticConcept() noexcept;

template <class T1, class T2>
using _only_arithmetic = typename std::enable_if<ArithmeticConcept<T1>() && ArithmeticConcept<T2>()>::type;

template <typename T>
static constexpr bool IntegralConcept() noexcept;

template <class T, class T2>
using _only_integer = typename std::enable_if<IntegralConcept<T>() && IntegralConcept<T2>()>::type;

// Unary operators
template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> operator~(const integer<Bits, Signed> & lhs) noexcept;

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> operator-(const integer<Bits, Signed> & lhs) noexcept(std::is_same_v<Signed, unsigned>);

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> operator+(const integer<Bits, Signed> & lhs) noexcept(std::is_same_v<Signed, unsigned>);

// Binary operators
template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator*(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Arithmetic, typename Arithmetic2, class = _only_arithmetic<Arithmetic, Arithmetic2>>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator*(const Arithmetic & rhs, const Arithmetic2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator/(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Arithmetic, typename Arithmetic2, class = _only_arithmetic<Arithmetic, Arithmetic2>>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator/(const Arithmetic & rhs, const Arithmetic2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator+(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Arithmetic, typename Arithmetic2, class = _only_arithmetic<Arithmetic, Arithmetic2>>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator+(const Arithmetic & rhs, const Arithmetic2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator-(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Arithmetic, typename Arithmetic2, class = _only_arithmetic<Arithmetic, Arithmetic2>>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator-(const Arithmetic & rhs, const Arithmetic2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator%(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Integral, typename Integral2, class = _only_integer<Integral, Integral2>>
std::common_type_t<Integral, Integral2> constexpr operator%(const Integral & rhs, const Integral2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator&(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Integral, typename Integral2, class = _only_integer<Integral, Integral2>>
std::common_type_t<Integral, Integral2> constexpr operator&(const Integral & rhs, const Integral2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator|(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Integral, typename Integral2, class = _only_integer<Integral, Integral2>>
std::common_type_t<Integral, Integral2> constexpr operator|(const Integral & rhs, const Integral2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator^(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Integral, typename Integral2, class = _only_integer<Integral, Integral2>>
std::common_type_t<Integral, Integral2> constexpr operator^(const Integral & rhs, const Integral2 & lhs);

// TODO: Integral
template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> operator<<(const integer<Bits, Signed> & lhs, int n) noexcept;

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> operator>>(const integer<Bits, Signed> & lhs, int n) noexcept;

template <size_t Bits, typename Signed, typename Int, typename = std::enable_if_t<!std::is_same_v<Int, int>>>
constexpr integer<Bits, Signed> operator<<(const integer<Bits, Signed> & lhs, Int n) noexcept
{
    return lhs << int(n);
}
template <size_t Bits, typename Signed, typename Int, typename = std::enable_if_t<!std::is_same_v<Int, int>>>
constexpr integer<Bits, Signed> operator>>(const integer<Bits, Signed> & lhs, Int n) noexcept
{
    return lhs >> int(n);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator<(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Arithmetic, typename Arithmetic2, class = _only_arithmetic<Arithmetic, Arithmetic2>>
constexpr bool operator<(const Arithmetic & rhs, const Arithmetic2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator>(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Arithmetic, typename Arithmetic2, class = _only_arithmetic<Arithmetic, Arithmetic2>>
constexpr bool operator>(const Arithmetic & rhs, const Arithmetic2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator<=(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Arithmetic, typename Arithmetic2, class = _only_arithmetic<Arithmetic, Arithmetic2>>
constexpr bool operator<=(const Arithmetic & rhs, const Arithmetic2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator>=(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Arithmetic, typename Arithmetic2, class = _only_arithmetic<Arithmetic, Arithmetic2>>
constexpr bool operator>=(const Arithmetic & rhs, const Arithmetic2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator==(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Arithmetic, typename Arithmetic2, class = _only_arithmetic<Arithmetic, Arithmetic2>>
constexpr bool operator==(const Arithmetic & rhs, const Arithmetic2 & lhs);

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator!=(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs);
template <typename Arithmetic, typename Arithmetic2, class = _only_arithmetic<Arithmetic, Arithmetic2>>
constexpr bool operator!=(const Arithmetic & rhs, const Arithmetic2 & lhs);

}

namespace std
{

template <size_t Bits, typename Signed>
struct hash<wide::integer<Bits, Signed>>;

}

/// Original is here https://github.com/cerevra/int
/// Distributed under the Boost Software License, Version 1.0.
/// (See at http://www.boost.org/LICENSE_1_0.txt)

/// Throw DB::Exception-like exception before its definition.
/// DB::Exception derived from Poco::Exception derived from std::exception.
/// DB::Exception generally caught as Poco::Exception. std::exception generally has other catch blocks and could lead to other outcomes.
/// DB::Exception is not defined yet. It'd better to throw Poco::Exception but we do not want to include any big header here, even <string>.
/// So we throw some std::exception instead in the hope its catch block is the same as DB::Exception one.
[[noreturn]] void throwError(const char * err);

#include <cmath>
#include <cfloat>
#include <cassert>
#include <tuple>
#include <limits>

// #include <boost/math/special_functions/fpclassify.hpp>

// NOLINTBEGIN(*)

/// Use same extended double for all platforms
// #if (LDBL_MANT_DIG == 64)
#define CONSTEXPR_FROM_DOUBLE constexpr
using FromDoubleIntermediateType = long double;
// #else
// #include <boost/multiprecision/cpp_bin_float.hpp>
// /// `wide_integer_from_builtin` can't be constexpr with non-literal `cpp_bin_float_double_extended`
// #define CONSTEXPR_FROM_DOUBLE
// using FromDoubleIntermediateType = boost::multiprecision::cpp_bin_float_double_extended;
// #endif

namespace CityHash_v1_0_2 { struct uint128; }

namespace wide
{

template <typename T>
struct IsWideInteger
{
    static const constexpr bool value = false;
};

template <size_t Bits, typename Signed>
struct IsWideInteger<wide::integer<Bits, Signed>>
{
    static const constexpr bool value = true;
};

template <typename T>
static constexpr bool ArithmeticConcept() noexcept
{
    return std::is_arithmetic_v<T> || IsWideInteger<T>::value;
}

template <typename T>
static constexpr bool IntegralConcept() noexcept
{
    return std::is_integral_v<T> || IsWideInteger<T>::value;
}

template <typename T>
class IsTupleLike
{
    template <typename U>
    static auto check(U * p) -> decltype(std::tuple_size<U>::value, int());
    template <typename>
    static void check(...);

public:
    static constexpr const bool value = !std::is_void<decltype(check<T>(nullptr))>::value;
};

}

namespace std
{

// numeric limits
template <size_t Bits, typename Signed>
class numeric_limits<wide::integer<Bits, Signed>>
{
public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = is_same<Signed, signed>::value;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = true;
    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr std::float_round_style round_style = std::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;
    static constexpr int digits = Bits - (is_same<Signed, signed>::value ? 1 : 0);
    static constexpr int digits10 = digits * 0.30103 /*std::log10(2)*/;
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = true;
    static constexpr bool tinyness_before = false;

    static constexpr wide::integer<Bits, Signed> min() noexcept
    {
        if (is_same<Signed, signed>::value)
        {
            using T = wide::integer<Bits, signed>;
            T res{};
            res.items[T::_impl::big(0)] = std::numeric_limits<typename wide::integer<Bits, Signed>::signed_base_type>::min();
            return res;
        }
        return wide::integer<Bits, Signed>(0);
    }

    static constexpr wide::integer<Bits, Signed> max() noexcept
    {
        using T = wide::integer<Bits, Signed>;
        T res{};
        res.items[T::_impl::big(0)] = is_same<Signed, signed>::value
            ? std::numeric_limits<typename wide::integer<Bits, Signed>::signed_base_type>::max()
            : std::numeric_limits<typename wide::integer<Bits, Signed>::base_type>::max();
        for (unsigned i = 1; i < wide::integer<Bits, Signed>::_impl::item_count; ++i)
        {
            res.items[T::_impl::big(i)] = std::numeric_limits<typename wide::integer<Bits, Signed>::base_type>::max();
        }
        return res;
    }

    static constexpr wide::integer<Bits, Signed> lowest() noexcept { return min(); }
    static constexpr wide::integer<Bits, Signed> epsilon() noexcept { return 0; }
    static constexpr wide::integer<Bits, Signed> round_error() noexcept { return 0; }
    static constexpr wide::integer<Bits, Signed> infinity() noexcept { return 0; }
    static constexpr wide::integer<Bits, Signed> quiet_NaN() noexcept { return 0; }
    static constexpr wide::integer<Bits, Signed> signaling_NaN() noexcept { return 0; }
    static constexpr wide::integer<Bits, Signed> denorm_min() noexcept { return 0; }
};

// type traits
template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
struct common_type<wide::integer<Bits, Signed>, wide::integer<Bits2, Signed2>>
{
    using type = std::conditional_t<Bits == Bits2,
        wide::integer<
            Bits,
            std::conditional_t<(std::is_same_v<Signed, Signed2> && std::is_same_v<Signed2, signed>), signed, unsigned>>,
        std::conditional_t<Bits2<Bits, wide::integer<Bits, Signed>, wide::integer<Bits2, Signed2>>>;
};

template <size_t Bits, typename Signed, typename Arithmetic>
struct common_type<wide::integer<Bits, Signed>, Arithmetic>
{
    static_assert(wide::ArithmeticConcept<Arithmetic>());

    using type = std::conditional_t<
        std::is_floating_point_v<Arithmetic>,
        Arithmetic,
        std::conditional_t<
            sizeof(Arithmetic) * 8 < Bits,
            wide::integer<Bits, Signed>,
            std::conditional_t<
                Bits < sizeof(Arithmetic) * 8,
                Arithmetic,
                std::conditional_t<
                    Bits == sizeof(Arithmetic) * 8 && (std::is_same_v<Signed, signed> || std::is_signed_v<Arithmetic>),
                    Arithmetic,
                    wide::integer<Bits, Signed>>>>>;
};

template <typename Arithmetic, size_t Bits, typename Signed>
struct common_type<Arithmetic, wide::integer<Bits, Signed>> : common_type<wide::integer<Bits, Signed>, Arithmetic>
{
};

}

#include <bit>

namespace wide
{

template <size_t Bits, typename Signed>
struct integer<Bits, Signed>::_impl
{
    static constexpr size_t _bits = Bits;
    static constexpr const unsigned byte_count = Bits / 8;
    static constexpr const unsigned item_count = byte_count / sizeof(base_type);
    static constexpr const unsigned base_bits = sizeof(base_type) * 8;

    static_assert(Bits % base_bits == 0);

    /// Simple iteration in both directions
    static constexpr unsigned little(unsigned idx)
    {
        if constexpr (std::endian::native == std::endian::little)
            return idx;
        else
            return item_count - 1 - idx;
    }
    static constexpr unsigned big(unsigned idx)
    {
        if constexpr (std::endian::native == std::endian::little)
            return item_count - 1 - idx;
        else
            return idx;
    }
    static constexpr unsigned any(unsigned idx) { return idx; }

    template <class T>
    constexpr static bool is_negative(const T & n) noexcept
    {
        if constexpr (std::is_signed_v<T>)
            return n < 0;
        else
            return false;
    }

    template <size_t B, class T>
    constexpr static bool is_negative(const integer<B, T> & n) noexcept
    {
        if constexpr (std::is_same_v<T, signed>)
            return static_cast<signed_base_type>(n.items[integer<B, T>::_impl::big(0)]) < 0;
        else
            return false;
    }

    template <typename T>
    constexpr static auto make_positive(const T & n) noexcept
    {
        if constexpr (std::is_signed_v<T>)
            return n < 0 ? -n : n;
        else
            return n;
    }

    template <size_t B, class S>
    constexpr static integer<B, S> make_positive(const integer<B, S> & n) noexcept
    {
        return is_negative(n) ? integer<B, S>(operator_unary_minus(n)) : n;
    }

    template <typename T>
    __attribute__((no_sanitize("undefined"))) constexpr static auto to_Integral(T f) noexcept
    {
        /// NOTE: this can be called with DB::Decimal, and in this case, result
        /// will be wrong
        if constexpr (std::is_signed_v<T>)
            return static_cast<int64_t>(f);
        else
            return static_cast<uint64_t>(f);
    }

    template <typename Integral>
    constexpr static void wide_integer_from_builtin(integer<Bits, Signed> & self, Integral rhs) noexcept
    {
        static_assert(sizeof(Integral) <= sizeof(base_type));

        self.items[little(0)] = _impl::to_Integral(rhs);

        if constexpr (std::is_signed_v<Integral>)
        {
            if (rhs < 0)
            {
                for (unsigned i = 1; i < item_count; ++i)
                    self.items[little(i)] = -1;
                return;
            }
        }

        for (unsigned i = 1; i < item_count; ++i)
            self.items[little(i)] = 0;
    }

    template <typename TupleLike, size_t i = 0>
    constexpr static void wide_integer_from_tuple_like(integer<Bits, Signed> & self, const TupleLike & tuple) noexcept
    {
        if constexpr (i < item_count)
        {
            if constexpr (i < std::tuple_size_v<TupleLike>)
                self.items[i] = std::get<i>(tuple);
            else
                self.items[i] = 0;
            wide_integer_from_tuple_like<TupleLike, i + 1>(self, tuple);
        }
    }

    template <typename CityHashUInt128 = CityHash_v1_0_2::uint128>
    constexpr static void wide_integer_from_cityhash_uint128(integer<Bits, Signed> & self, const CityHashUInt128 & value) noexcept
    {
        static_assert(sizeof(item_count) >= 2);

        if constexpr (std::endian::native == std::endian::little)
            wide_integer_from_tuple_like(self, std::make_pair(value.low64, value.high64));
        else
            wide_integer_from_tuple_like(self, std::make_pair(value.high64, value.low64));
    }

    /**
     * N.B. t is constructed from double, so max(t) = max(double) ~ 2^310
     * the recursive call happens when t / 2^64 > 2^64, so there won't be more than 5 of them.
     *
     * t = a1 * max_int + b1,   a1 > max_int, b1 < max_int
     * a1 = a2 * max_int + b2,  a2 > max_int, b2 < max_int
     * a_(n - 1) = a_n * max_int + b2, a_n <= max_int <- base case.
     */
    template <class T>
    constexpr static void set_multiplier(integer<Bits, Signed> & self, T t) noexcept
    {
        constexpr uint64_t max_int = std::numeric_limits<uint64_t>::max();
        static_assert(std::is_same_v<T, double> || std::is_same_v<T, FromDoubleIntermediateType>);
        /// Implementation specific behaviour on overflow (if we don't check here, stack overflow will triggered in bigint_cast).
        if constexpr (std::is_same_v<T, double>)
        {
            if (!std::isfinite(t))
            {
                self = 0;
                return;
            }
        }
        else
        {
            if (!std::isfinite(t))
            // if (!boost::math::isfinite(t))
            {
                self = 0;
                return;
            }
        }

        const T alpha = t / static_cast<T>(max_int);

        /** Here we have to use strict comparison.
          * The max_int is 2^64 - 1.
          * When casted to floating point type, it will be rounded to the closest representable number,
          * which is 2^64.
          * But 2^64 is not representable in uint64_t,
          * so the maximum representable number will be strictly less.
          */
        if (alpha < static_cast<T>(max_int))
            self = static_cast<uint64_t>(alpha);
        else // max(double) / 2^64 will surely contain less than 52 precision bits, so speed up computations.
            set_multiplier<double>(self, static_cast<double>(alpha));

        self *= max_int;
        self += static_cast<uint64_t>(t - floor(alpha) * static_cast<T>(max_int)); // += b_i
    }

    CONSTEXPR_FROM_DOUBLE static void wide_integer_from_builtin(integer<Bits, Signed> & self, double rhs) noexcept
    {
        constexpr int64_t max_int = std::numeric_limits<int64_t>::max();
        constexpr int64_t min_int = std::numeric_limits<int64_t>::lowest();

        /// There are values in int64 that have more than 53 significant bits (in terms of double
        /// representation). Such values, being promoted to double, are rounded up or down. If they are rounded up,
        /// the result may not fit in 64 bits.
        /// The example of such a number is 9.22337e+18.
        /// As to_Integral does a static_cast to int64_t, it may result in UB.
        /// The necessary check here is that FromDoubleIntermediateType has enough significant (mantissa) bits to store the
        /// int64_t max value precisely.

        if (rhs > static_cast<FromDoubleIntermediateType>(min_int) && rhs < static_cast<FromDoubleIntermediateType>(max_int))
        {
            self = static_cast<int64_t>(rhs);
            return;
        }

        const FromDoubleIntermediateType rhs_long_double = (static_cast<FromDoubleIntermediateType>(rhs) < 0)
            ? -static_cast<FromDoubleIntermediateType>(rhs)
            : rhs;

        set_multiplier(self, rhs_long_double);

        if (rhs < 0)
            self = -self;
    }

    template <size_t Bits2, typename Signed2>
    constexpr static void
    wide_integer_from_wide_integer(integer<Bits, Signed> & self, const integer<Bits2, Signed2> & rhs) noexcept
    {
        constexpr const unsigned min_bits = (Bits < Bits2) ? Bits : Bits2;
        constexpr const unsigned to_copy = min_bits / base_bits;

        for (unsigned i = 0; i < to_copy; ++i)
            self.items[little(i)] = rhs.items[integer<Bits2, Signed2>::_impl::little(i)];

        if constexpr (Bits > Bits2)
        {
            if constexpr (std::is_signed_v<Signed2>)
            {
                if (rhs < 0)
                {
                    for (unsigned i = to_copy; i < item_count; ++i)
                        self.items[little(i)] = -1;
                    return;
                }
            }

            for (unsigned i = to_copy; i < item_count; ++i)
                self.items[little(i)] = 0;
        }
    }

    template <typename T>
    constexpr static bool should_keep_size()
    {
        return sizeof(T) <= byte_count;
    }

    constexpr static integer<Bits, Signed> shift_left(const integer<Bits, Signed> & rhs, unsigned n) noexcept
    {
        integer<Bits, Signed> lhs;
        unsigned items_shift = n / base_bits;

        if (unsigned bit_shift = n % base_bits)
        {
            unsigned overflow_shift = base_bits - bit_shift;

            lhs.items[big(0)] = rhs.items[big(items_shift)] << bit_shift;
            for (unsigned i = 1; i < item_count - items_shift; ++i)
            {
                lhs.items[big(i - 1)] |= rhs.items[big(items_shift + i)] >> overflow_shift;
                lhs.items[big(i)] = rhs.items[big(items_shift + i)] << bit_shift;
            }
        }
        else
        {
            for (unsigned i = 0; i < item_count - items_shift; ++i)
                lhs.items[big(i)] = rhs.items[big(items_shift + i)];
        }

        for (unsigned i = 0; i < items_shift; ++i)
            lhs.items[little(i)] = 0;
        return lhs;
    }

    constexpr static integer<Bits, Signed> shift_right(const integer<Bits, Signed> & rhs, unsigned n) noexcept
    {
        integer<Bits, Signed> lhs;
        unsigned items_shift = n / base_bits;
        unsigned bit_shift = n % base_bits;

        if (bit_shift)
        {
            unsigned overflow_shift = base_bits - bit_shift;

            lhs.items[little(0)] = rhs.items[little(items_shift)] >> bit_shift;
            for (unsigned i = 1; i < item_count - items_shift; ++i)
            {
                lhs.items[little(i - 1)] |= rhs.items[little(items_shift + i)] << overflow_shift;
                lhs.items[little(i)] = rhs.items[little(items_shift + i)] >> bit_shift;
            }
        }
        else
        {
            for (unsigned i = 0; i < item_count - items_shift; ++i)
                lhs.items[little(i)] = rhs.items[little(items_shift + i)];
        }

        if (is_negative(rhs))
        {
            if (bit_shift)
                lhs.items[big(items_shift)] |= std::numeric_limits<base_type>::max() << (base_bits - bit_shift);

            for (unsigned i = 0; i < items_shift; ++i)
                lhs.items[big(i)] = std::numeric_limits<base_type>::max();
        }
        else
        {
            for (unsigned i = 0; i < items_shift; ++i)
                lhs.items[big(i)] = 0;
        }

        return lhs;
    }

private:
    template <typename T>
    constexpr static base_type get_item(const T & x, unsigned idx)
    {
        if constexpr (IsWideInteger<T>::value)
        {
            if (idx < T::_impl::item_count)
                return x.items[idx];
            return 0;
        }
        else
        {
            if constexpr (sizeof(T) <= sizeof(base_type))
            {
                if (little(0) == idx)
                    return static_cast<base_type>(x);
            }
            else if (idx * sizeof(base_type) < sizeof(T))
                return x >> (idx * base_bits); // & std::numeric_limits<base_type>::max()
            return 0;
        }
    }

    template <typename T>
    constexpr static integer<Bits, Signed>
    minus(const integer<Bits, Signed> & lhs, T rhs)
    {
        constexpr const unsigned rhs_items = (sizeof(T) > sizeof(base_type)) ? (sizeof(T) / sizeof(base_type)) : 1;
        constexpr const unsigned op_items = (item_count < rhs_items) ? item_count : rhs_items;

        integer<Bits, Signed> res(lhs);
        bool underflows[item_count] = {};

        for (unsigned i = 0; i < op_items; ++i)
        {
            base_type rhs_item = get_item(rhs, little(i));
            base_type & res_item = res.items[little(i)];

            underflows[i] = res_item < rhs_item;
            res_item -= rhs_item;
        }

        for (unsigned i = 1; i < item_count; ++i)
        {
            if (underflows[i - 1])
            {
                base_type & res_item = res.items[little(i)];
                if (res_item == 0)
                    underflows[i] = true;
                --res_item;
            }
        }

        return res;
    }

    template <typename T>
    constexpr static integer<Bits, Signed>
    plus(const integer<Bits, Signed> & lhs, T rhs)
    {
        constexpr const unsigned rhs_items = (sizeof(T) > sizeof(base_type)) ? (sizeof(T) / sizeof(base_type)) : 1;
        constexpr const unsigned op_items = (item_count < rhs_items) ? item_count : rhs_items;

        integer<Bits, Signed> res(lhs);
        bool overflows[item_count] = {};

        for (unsigned i = 0; i < op_items; ++i)
        {
            base_type rhs_item = get_item(rhs, little(i));
            base_type & res_item = res.items[little(i)];

            res_item += rhs_item;
            overflows[i] = res_item < rhs_item;
        }

        for (unsigned i = 1; i < item_count; ++i)
        {
            if (overflows[i - 1])
            {
                base_type & res_item = res.items[little(i)];
                ++res_item;
                if (res_item == 0)
                    overflows[i] = true;
            }
        }

        return res;
    }

    template <typename T>
    constexpr static integer<Bits, Signed>
    multiply(const integer<Bits, Signed> & lhs, const T & rhs)
    {
        if constexpr (Bits == 256 && sizeof(base_type) == 8)
        {
            /// @sa https://github.com/abseil/abseil-cpp/blob/master/absl/numeric/int128.h
            using HalfType = unsigned __int128;

            HalfType a01 = (HalfType(lhs.items[little(1)]) << 64) + lhs.items[little(0)];
            HalfType a23 = (HalfType(lhs.items[little(3)]) << 64) + lhs.items[little(2)];
            HalfType a0 = lhs.items[little(0)];
            HalfType a1 = lhs.items[little(1)];

            HalfType b01 = rhs;
            uint64_t b0 = b01;
            uint64_t b1 = 0;
            HalfType b23 = 0;
            if constexpr (sizeof(T) > 8)
                b1 = b01 >> 64;
            if constexpr (sizeof(T) > 16)
                b23 = (HalfType(rhs.items[little(3)]) << 64) + rhs.items[little(2)];

            HalfType r23 = a23 * b01 + a01 * b23 + a1 * b1;
            HalfType r01 = a0 * b0;
            HalfType r12 = (r01 >> 64) + (r23 << 64);
            HalfType r12_x = a1 * b0;

            integer<Bits, Signed> res;
            res.items[little(0)] = r01;
            res.items[little(3)] = r23 >> 64;

            if constexpr (sizeof(T) > 8)
            {
                HalfType r12_y = a0 * b1;
                r12_x += r12_y;
                if (r12_x < r12_y)
                    ++res.items[little(3)];
            }

            r12 += r12_x;
            if (r12 < r12_x)
                ++res.items[little(3)];

            res.items[little(1)] = r12;
            res.items[little(2)] = r12 >> 64;
            return res;
        }
        else if constexpr (Bits == 128 && sizeof(base_type) == 8)
        {
            using CompilerUInt128 = unsigned __int128;
            CompilerUInt128 a = (CompilerUInt128(lhs.items[little(1)]) << 64) + lhs.items[little(0)]; // NOLINT(clang-analyzer-core.UndefinedBinaryOperatorResult)
            CompilerUInt128 b = (CompilerUInt128(rhs.items[little(1)]) << 64) + rhs.items[little(0)]; // NOLINT(clang-analyzer-core.UndefinedBinaryOperatorResult)
            CompilerUInt128 c = a * b;
            integer<Bits, Signed> res;
            res.items[little(0)] = c;
            res.items[little(1)] = c >> 64;
            return res;
        }
        else
        {
            integer<Bits, Signed> res{};
#if 1
            integer<Bits, Signed> lhs2 = plus(lhs, shift_left(lhs, 1));
            integer<Bits, Signed> lhs3 = plus(lhs2, shift_left(lhs, 2));
#endif
            for (unsigned i = 0; i < item_count; ++i)
            {
                base_type rhs_item = get_item(rhs, little(i));
                unsigned pos = i * base_bits;

                while (rhs_item)
                {
#if 1 /// optimization
                    if ((rhs_item & 0x7) == 0x7)
                    {
                        res = plus(res, shift_left(lhs3, pos));
                        rhs_item >>= 3;
                        pos += 3;
                        continue;
                    }

                    if ((rhs_item & 0x3) == 0x3)
                    {
                        res = plus(res, shift_left(lhs2, pos));
                        rhs_item >>= 2;
                        pos += 2;
                        continue;
                    }
#endif
                    if (rhs_item & 1)
                        res = plus(res, shift_left(lhs, pos));

                    rhs_item >>= 1;
                    ++pos;
                }
            }

            return res;
        }
    }

public:
    constexpr static integer<Bits, Signed> operator_unary_tilda(const integer<Bits, Signed> & lhs) noexcept
    {
        integer<Bits, Signed> res;

        for (unsigned i = 0; i < item_count; ++i)
            res.items[any(i)] = ~lhs.items[any(i)];
        return res;
    }

    constexpr static integer<Bits, Signed>
    operator_unary_minus(const integer<Bits, Signed> & lhs) noexcept(std::is_same_v<Signed, unsigned>)
    {
        return plus(operator_unary_tilda(lhs), 1);
    }

    template <typename T>
    constexpr static auto operator_plus(const integer<Bits, Signed> & lhs, const T & rhs) noexcept(std::is_same_v<Signed, unsigned>)
    {
        if constexpr (should_keep_size<T>())
        {
            if (is_negative(rhs))
                return minus(lhs, -rhs);
            else
                return plus(lhs, rhs);
        }
        else
        {
            static_assert(IsWideInteger<T>::value);
            return std::common_type_t<integer<Bits, Signed>, integer<T::_impl::_bits, Signed>>::_impl::operator_plus(
                integer<T::_impl::_bits, Signed>(lhs), rhs);
        }
    }

    template <typename T>
    constexpr static auto operator_minus(const integer<Bits, Signed> & lhs, const T & rhs) noexcept(std::is_same_v<Signed, unsigned>)
    {
        if constexpr (should_keep_size<T>())
        {
            if (is_negative(rhs))
                return plus(lhs, -rhs);
            else
                return minus(lhs, rhs);
        }
        else
        {
            static_assert(IsWideInteger<T>::value);
            return std::common_type_t<integer<Bits, Signed>, integer<T::_impl::_bits, Signed>>::_impl::operator_minus(
                integer<T::_impl::_bits, Signed>(lhs), rhs);
        }
    }

    template <typename T>
    constexpr static auto operator_star(const integer<Bits, Signed> & lhs, const T & rhs)
    {
        if constexpr (should_keep_size<T>())
        {
            integer<Bits, Signed> res;

            if constexpr (std::is_signed_v<Signed>)
            {
                res = multiply((is_negative(lhs) ? make_positive(lhs) : lhs),
                                  (is_negative(rhs) ? make_positive(rhs) : rhs));
            }
            else
            {
                res = multiply(lhs, (is_negative(rhs) ? make_positive(rhs) : rhs));
            }

            if (std::is_same_v<Signed, signed> && is_negative(lhs) != is_negative(rhs))
                res = operator_unary_minus(res);

            return res;
        }
        else
        {
            static_assert(IsWideInteger<T>::value);
            return std::common_type_t<integer<Bits, Signed>, T>::_impl::operator_star(T(lhs), rhs);
        }
    }

    template <typename T>
    constexpr static bool operator_greater(const integer<Bits, Signed> & lhs, const T & rhs) noexcept
    {
        if constexpr (should_keep_size<T>())
        {
            if (std::numeric_limits<T>::is_signed && (is_negative(lhs) != is_negative(rhs)))
                return is_negative(rhs);

            integer<Bits, Signed> t = rhs;
            for (unsigned i = 0; i < item_count; ++i)
            {
                base_type rhs_item = get_item(t, big(i));

                if (lhs.items[big(i)] != rhs_item)
                    return lhs.items[big(i)] > rhs_item;
            }

            return false;
        }
        else
        {
            static_assert(IsWideInteger<T>::value);
            return std::common_type_t<integer<Bits, Signed>, T>::_impl::operator_greater(T(lhs), rhs);
        }
    }

    template <typename T>
    constexpr static bool operator_less(const integer<Bits, Signed> & lhs, const T & rhs) noexcept
    {
        if constexpr (should_keep_size<T>())
        {
            if (std::numeric_limits<T>::is_signed && (is_negative(lhs) != is_negative(rhs)))
                return is_negative(lhs);

            integer<Bits, Signed> t = rhs;
            for (unsigned i = 0; i < item_count; ++i)
            {
                base_type rhs_item = get_item(t, big(i));

                if (lhs.items[big(i)] != rhs_item)
                    return lhs.items[big(i)] < rhs_item;
            }

            return false;
        }
        else
        {
            static_assert(IsWideInteger<T>::value);
            return std::common_type_t<integer<Bits, Signed>, T>::_impl::operator_less(T(lhs), rhs);
        }
    }

    template <typename T>
    constexpr static bool operator_eq(const integer<Bits, Signed> & lhs, const T & rhs) noexcept
    {
        if constexpr (should_keep_size<T>())
        {
            integer<Bits, Signed> t = rhs;
            for (unsigned i = 0; i < item_count; ++i)
            {
                base_type rhs_item = get_item(t, any(i));

                if (lhs.items[any(i)] != rhs_item)
                    return false;
            }

            return true;
        }
        else
        {
            static_assert(IsWideInteger<T>::value);
            return std::common_type_t<integer<Bits, Signed>, T>::_impl::operator_eq(T(lhs), rhs);
        }
    }

    template <typename T>
    constexpr static auto operator_pipe(const integer<Bits, Signed> & lhs, const T & rhs) noexcept
    {
        if constexpr (should_keep_size<T>())
        {
            integer<Bits, Signed> res;

            for (unsigned i = 0; i < item_count; ++i)
                res.items[little(i)] = lhs.items[little(i)] | get_item(rhs, little(i));
            return res;
        }
        else
        {
            static_assert(IsWideInteger<T>::value);
            return std::common_type_t<integer<Bits, Signed>, T>::_impl::operator_pipe(T(lhs), rhs);
        }
    }

    template <typename T>
    constexpr static auto operator_amp(const integer<Bits, Signed> & lhs, const T & rhs) noexcept
    {
        if constexpr (should_keep_size<T>())
        {
            integer<Bits, Signed> res;

            for (unsigned i = 0; i < item_count; ++i)
                res.items[little(i)] = lhs.items[little(i)] & get_item(rhs, little(i));
            return res;
        }
        else
        {
            static_assert(IsWideInteger<T>::value);
            return std::common_type_t<integer<Bits, Signed>, T>::_impl::operator_amp(T(lhs), rhs);
        }
    }

    template <typename T>
    constexpr static bool is_zero(const T & x)
    {
        bool is_zero = true;
        for (auto item : x.items)
        {
            if (item != 0)
            {
                is_zero = false;
                break;
            }
        }
        return is_zero;
    }

    /// returns quotient as result and remainder in numerator.
    template <size_t Bits2>
    constexpr static integer<Bits2, unsigned> divide(integer<Bits2, unsigned> & numerator, integer<Bits2, unsigned> denominator)
    {
        static_assert(std::is_unsigned_v<Signed>);

        if constexpr (Bits == 128 && sizeof(base_type) == 8)
        {
            using CompilerUInt128 = unsigned __int128;

            CompilerUInt128 a = (CompilerUInt128(numerator.items[little(1)]) << 64) + numerator.items[little(0)]; // NOLINT(clang-analyzer-core.UndefinedBinaryOperatorResult)
            CompilerUInt128 b = (CompilerUInt128(denominator.items[little(1)]) << 64) + denominator.items[little(0)]; // NOLINT(clang-analyzer-core.UndefinedBinaryOperatorResult)
            CompilerUInt128 c = a / b; // NOLINT

            integer<Bits, Signed> res;
            res.items[little(0)] = c;
            res.items[little(1)] = c >> 64;

            CompilerUInt128 remainder = a - b * c;
            numerator.items[little(0)] = remainder;
            numerator.items[little(1)] = remainder >> 64;

            return res;
        }

        if (is_zero(denominator))
            throwError("Division by zero");

        integer<Bits2, unsigned> x = 1;
        integer<Bits2, unsigned> quotient = 0;

        while (!operator_greater(denominator, numerator) && is_zero(operator_amp(shift_right(denominator, Bits2 - 1), 1)))
        {
            x = shift_left(x, 1);
            denominator = shift_left(denominator, 1);
        }

        while (!is_zero(x))
        {
            if (!operator_greater(denominator, numerator))
            {
                numerator = operator_minus(numerator, denominator);
                quotient = operator_pipe(quotient, x);
            }

            x = shift_right(x, 1);
            denominator = shift_right(denominator, 1);
        }

        return quotient;
    }

    template <typename T>
    constexpr static auto operator_slash(const integer<Bits, Signed> & lhs, const T & rhs)
    {
        if constexpr (should_keep_size<T>())
        {
            integer<Bits, unsigned> numerator = make_positive(lhs);
            integer<Bits, unsigned> denominator = make_positive(integer<Bits, Signed>(rhs));
            integer<Bits, unsigned> quotient = integer<Bits, unsigned>::_impl::divide(numerator, std::move(denominator));

            if (std::is_same_v<Signed, signed> && is_negative(rhs) != is_negative(lhs))
                quotient = operator_unary_minus(quotient);
            return quotient;
        }
        else
        {
            static_assert(IsWideInteger<T>::value);
            return std::common_type_t<integer<Bits, Signed>, integer<T::_impl::_bits, Signed>>::operator_slash(T(lhs), rhs);
        }
    }

    template <typename T>
    constexpr static auto operator_percent(const integer<Bits, Signed> & lhs, const T & rhs)
    {
        if constexpr (should_keep_size<T>())
        {
            integer<Bits, unsigned> remainder = make_positive(lhs);
            integer<Bits, unsigned> denominator = make_positive(integer<Bits, Signed>(rhs));
            integer<Bits, unsigned>::_impl::divide(remainder, std::move(denominator));

            if (std::is_same_v<Signed, signed> && is_negative(lhs))
                remainder = operator_unary_minus(remainder);
            return remainder;
        }
        else
        {
            static_assert(IsWideInteger<T>::value);
            return std::common_type_t<integer<Bits, Signed>, integer<T::_impl::_bits, Signed>>::operator_percent(T(lhs), rhs);
        }
    }

    // ^
    template <typename T>
    constexpr static auto operator_circumflex(const integer<Bits, Signed> & lhs, const T & rhs) noexcept
    {
        if constexpr (should_keep_size<T>())
        {
            integer<Bits, Signed> t(rhs);
            integer<Bits, Signed> res = lhs;

            for (unsigned i = 0; i < item_count; ++i)
                res.items[any(i)] ^= t.items[any(i)];
            return res;
        }
        else
        {
            static_assert(IsWideInteger<T>::value);
            return T::operator_circumflex(T(lhs), rhs);
        }
    }

    constexpr static integer<Bits, Signed> from_str(const char * c)
    {
        integer<Bits, Signed> res = 0;

        bool is_neg = std::is_same_v<Signed, signed> && *c == '-';
        if (is_neg)
            ++c;

        if (*c == '0' && (*(c + 1) == 'x' || *(c + 1) == 'X'))
        { // hex
            ++c;
            ++c;
            while (*c)
            {
                if (*c >= '0' && *c <= '9')
                {
                    res = multiply(res, 16U);
                    res = plus(res, *c - '0');
                    ++c;
                }
                else if (*c >= 'a' && *c <= 'f')
                {
                    res = multiply(res, 16U);
                    res = plus(res, *c - 'a' + 10U);
                    ++c;
                }
                else if (*c >= 'A' && *c <= 'F')
                { // tolower must be used, but it is not constexpr
                    res = multiply(res, 16U);
                    res = plus(res, *c - 'A' + 10U);
                    ++c;
                }
                else
                    throwError("Invalid char from");
            }
        }
        else
        { // dec
            while (*c)
            {
                if (*c < '0' || *c > '9')
                    throwError("Invalid char from");

                res = multiply(res, 10U);
                res = plus(res, *c - '0');
                ++c;
            }
        }

        if (is_neg)
            res = operator_unary_minus(res);

        return res;
    }
};

// Members

template <size_t Bits, typename Signed>
template <typename T>
constexpr integer<Bits, Signed>::integer(T rhs) noexcept
    : items{}
{
    if constexpr (IsWideInteger<T>::value)
        _impl::wide_integer_from_wide_integer(*this, rhs);
    else if  constexpr (IsTupleLike<T>::value)
        _impl::wide_integer_from_tuple_like(*this, rhs);
    else if constexpr (std::is_same_v<std::remove_cvref_t<T>, CityHash_v1_0_2::uint128>)
        _impl::wide_integer_from_cityhash_uint128(*this, rhs);
    else
        _impl::wide_integer_from_builtin(*this, rhs);
}

template <size_t Bits, typename Signed>
template <typename T>
constexpr integer<Bits, Signed>::integer(std::initializer_list<T> il) noexcept
    : items{}
{
    if (il.size() == 1)
    {
        if constexpr (IsWideInteger<T>::value)
            _impl::wide_integer_from_wide_integer(*this, *il.begin());
        else if  constexpr (IsTupleLike<T>::value)
            _impl::wide_integer_from_tuple_like(*this, *il.begin());
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, CityHash_v1_0_2::uint128>)
            _impl::wide_integer_from_cityhash_uint128(*this, *il.begin());
        else
            _impl::wide_integer_from_builtin(*this, *il.begin());
    }
    else if (il.size() == 0)
    {
        _impl::wide_integer_from_builtin(*this, 0);
    }
    else
    {
        auto it = il.begin();
        for (unsigned i = 0; i < _impl::item_count; ++i)
        {
            if (it < il.end())
            {
                items[_impl::little(i)] = *it;
                ++it;
            }
            else
                items[_impl::little(i)] = 0;
        }
    }
}

template <size_t Bits, typename Signed>
template <size_t Bits2, typename Signed2>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator=(const integer<Bits2, Signed2> & rhs) noexcept
{
    _impl::wide_integer_from_wide_integer(*this, rhs);
    return *this;
}

template <size_t Bits, typename Signed>
template <typename T>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator=(T rhs) noexcept
{
    if  constexpr (IsTupleLike<T>::value)
        _impl::wide_integer_from_tuple_like(*this, rhs);
    else if constexpr (std::is_same_v<std::remove_cvref_t<T>, CityHash_v1_0_2::uint128>)
        _impl::wide_integer_from_cityhash_uint128(*this, rhs);
    else
        _impl::wide_integer_from_builtin(*this, rhs);
    return *this;
}

template <size_t Bits, typename Signed>
template <typename T>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator*=(const T & rhs)
{
    *this = *this * rhs;
    return *this;
}

template <size_t Bits, typename Signed>
template <typename T>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator/=(const T & rhs)
{
    *this = *this / rhs;
    return *this;
}

template <size_t Bits, typename Signed>
template <typename T>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator+=(const T & rhs) noexcept(std::is_same_v<Signed, unsigned>)
{
    *this = *this + rhs;
    return *this;
}

template <size_t Bits, typename Signed>
template <typename T>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator-=(const T & rhs) noexcept(std::is_same_v<Signed, unsigned>)
{
    *this = *this - rhs;
    return *this;
}

template <size_t Bits, typename Signed>
template <typename T>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator%=(const T & rhs)
{
    *this = *this % rhs;
    return *this;
}

template <size_t Bits, typename Signed>
template <typename T>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator&=(const T & rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

template <size_t Bits, typename Signed>
template <typename T>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator|=(const T & rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}

template <size_t Bits, typename Signed>
template <typename T>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator^=(const T & rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator<<=(int n) noexcept
{
    if (static_cast<size_t>(n) >= Bits)
        *this = 0;
    else if (n > 0)
        *this = _impl::shift_left(*this, n);
    return *this;
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator>>=(int n) noexcept
{
    if (static_cast<size_t>(n) >= Bits)
    {
        if (_impl::is_negative(*this))
            *this = -1;
        else
            *this = 0;
    }
    else if (n > 0)
        *this = _impl::shift_right(*this, n);
    return *this;
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator++() noexcept(std::is_same_v<Signed, unsigned>)
{
    *this = _impl::operator_plus(*this, 1);
    return *this;
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> integer<Bits, Signed>::operator++(int) noexcept(std::is_same_v<Signed, unsigned>)
{
    auto tmp = *this;
    *this = _impl::operator_plus(*this, 1);
    return tmp;
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> & integer<Bits, Signed>::operator--() noexcept(std::is_same_v<Signed, unsigned>)
{
    *this = _impl::operator_minus(*this, 1);
    return *this;
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> integer<Bits, Signed>::operator--(int) noexcept(std::is_same_v<Signed, unsigned>)
{
    auto tmp = *this;
    *this = _impl::operator_minus(*this, 1);
    return tmp;
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed>::operator bool() const noexcept
{
    return !_impl::operator_eq(*this, 0);
}

template <size_t Bits, typename Signed>
template <class T, class>
constexpr integer<Bits, Signed>::operator T() const noexcept
{
    static_assert(std::numeric_limits<T>::is_integer);

    /// NOTE: memcpy will suffice, but unfortunately, this function is constexpr.

    using UnsignedT = std::make_unsigned_t<T>;

    UnsignedT res{};
    for (unsigned i = 0; i < _impl::item_count && i < (sizeof(T) + sizeof(base_type) - 1) / sizeof(base_type); ++i)
        res += UnsignedT(items[_impl::little(i)]) << (sizeof(base_type) * 8 * i); // NOLINT(clang-analyzer-core.UndefinedBinaryOperatorResult)

    return res;
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed>::operator long double() const noexcept
{
    if (_impl::operator_eq(*this, 0))
        return 0;

    integer<Bits, Signed> tmp = *this;
    if (_impl::is_negative(*this))
        tmp = -tmp;

    long double res = 0;
    for (unsigned i = 0; i < _impl::item_count; ++i)
    {
        long double t = res;
        res *= static_cast<long double>(std::numeric_limits<base_type>::max());
        res += t;
        res += tmp.items[_impl::big(i)];
    }

    if (_impl::is_negative(*this))
        res = -res;

    return res;
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed>::operator double() const noexcept
{
    return static_cast<double>(static_cast<long double>(*this));
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed>::operator float() const noexcept
{
    return static_cast<float>(static_cast<long double>(*this));
}

// Unary operators
template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> operator~(const integer<Bits, Signed> & lhs) noexcept
{
    return integer<Bits, Signed>::_impl::operator_unary_tilda(lhs);
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> operator-(const integer<Bits, Signed> & lhs) noexcept(std::is_same_v<Signed, unsigned>)
{
    return integer<Bits, Signed>::_impl::operator_unary_minus(lhs);
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> operator+(const integer<Bits, Signed> & lhs) noexcept(std::is_same_v<Signed, unsigned>)
{
    return lhs;
}

#define CT(x) \
    std::common_type_t<std::decay_t<decltype(rhs)>, std::decay_t<decltype(lhs)>> { x }

// Binary operators
template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator*(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_star(lhs, rhs);
}

template <typename Arithmetic, typename Arithmetic2, class>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator*(const Arithmetic & lhs, const Arithmetic2 & rhs)
{
    return CT(lhs) * CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator/(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_slash(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator/(const Arithmetic & lhs, const Arithmetic2 & rhs)
{
    return CT(lhs) / CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator+(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_plus(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator+(const Arithmetic & lhs, const Arithmetic2 & rhs)
{
    return CT(lhs) + CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator-(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_minus(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator-(const Arithmetic & lhs, const Arithmetic2 & rhs)
{
    return CT(lhs) - CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator%(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_percent(lhs, rhs);
}
template <typename Integral, typename Integral2, class>
std::common_type_t<Integral, Integral2> constexpr operator%(const Integral & lhs, const Integral2 & rhs)
{
    return CT(lhs) % CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator&(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_amp(lhs, rhs);
}
template <typename Integral, typename Integral2, class>
std::common_type_t<Integral, Integral2> constexpr operator&(const Integral & lhs, const Integral2 & rhs)
{
    return CT(lhs) & CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator|(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_pipe(lhs, rhs);
}
template <typename Integral, typename Integral2, class>
std::common_type_t<Integral, Integral2> constexpr operator|(const Integral & lhs, const Integral2 & rhs)
{
    return CT(lhs) | CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>> constexpr
operator^(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_circumflex(lhs, rhs);
}
template <typename Integral, typename Integral2, class>
std::common_type_t<Integral, Integral2> constexpr operator^(const Integral & lhs, const Integral2 & rhs)
{
    return CT(lhs) ^ CT(rhs);
}

template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> operator<<(const integer<Bits, Signed> & lhs, int n) noexcept
{
    if (static_cast<size_t>(n) >= Bits)
        return integer<Bits, Signed>(0);
    if (n <= 0)
        return lhs;
    return integer<Bits, Signed>::_impl::shift_left(lhs, n);
}
template <size_t Bits, typename Signed>
constexpr integer<Bits, Signed> operator>>(const integer<Bits, Signed> & lhs, int n) noexcept
{
    if (static_cast<size_t>(n) >= Bits)
        return integer<Bits, Signed>(0);
    if (n <= 0)
        return lhs;
    return integer<Bits, Signed>::_impl::shift_right(lhs, n);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator<(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_less(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator<(const Arithmetic & lhs, const Arithmetic2 & rhs)
{
    return CT(lhs) < CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator>(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_greater(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator>(const Arithmetic & lhs, const Arithmetic2 & rhs)
{
    return CT(lhs) > CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator<=(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_less(lhs, rhs)
        || std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_eq(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator<=(const Arithmetic & lhs, const Arithmetic2 & rhs)
{
    return CT(lhs) <= CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator>=(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_greater(lhs, rhs)
        || std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_eq(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator>=(const Arithmetic & lhs, const Arithmetic2 & rhs)
{
    return CT(lhs) >= CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator==(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_eq(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator==(const Arithmetic & lhs, const Arithmetic2 & rhs)
{
    return CT(lhs) == CT(rhs);
}

template <size_t Bits, typename Signed, size_t Bits2, typename Signed2>
constexpr bool operator!=(const integer<Bits, Signed> & lhs, const integer<Bits2, Signed2> & rhs)
{
    return !std::common_type_t<integer<Bits, Signed>, integer<Bits2, Signed2>>::_impl::operator_eq(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator!=(const Arithmetic & lhs, const Arithmetic2 & rhs)
{
    return CT(lhs) != CT(rhs);
}

#undef CT

}

namespace std
{

template <size_t Bits, typename Signed>
struct hash<wide::integer<Bits, Signed>>
{
    std::size_t operator()(const wide::integer<Bits, Signed> & lhs) const
    {
        static_assert(Bits % (sizeof(size_t) * 8) == 0);

        const auto * ptr = reinterpret_cast<const size_t *>(lhs.items);
        unsigned count = Bits / (sizeof(size_t) * 8);

        size_t res = 0;
        for (unsigned i = 0; i < count; ++i)
            res ^= ptr[i];
        return res;
    }
};

}

// NOLINTEND(*)

#include <type_traits>

using Int128 = wide::integer<128, signed>;
using UInt128 = wide::integer<128, unsigned>;
using Int256 = wide::integer<256, signed>;
using UInt256 = wide::integer<256, unsigned>;

static_assert(sizeof(Int256) == 32);
static_assert(sizeof(UInt256) == 32);

/// The standard library type traits, such as std::is_arithmetic, with one
/// exception (std::common_type), are "set in stone". Attempting to specialize
/// them causes undefined behavior. So instead of using the std type_traits, we
/// use our own version which allows extension.
template <typename T>
struct is_signed  // NOLINT(readability-identifier-naming)
{
    static constexpr bool value = std::is_signed_v<T>;
};

template <>
struct is_signed<Int128> {
    static constexpr bool value = true;
};
template <>
struct is_signed<Int256> {
    static constexpr bool value = true;
};

template <typename T>
inline constexpr bool is_signed_v = is_signed<T>::value;

template <typename T>
struct is_unsigned  // NOLINT(readability-identifier-naming)
{
    static constexpr bool value = std::is_unsigned_v<T>;
};

template <>
struct is_unsigned<UInt128> {
    static constexpr bool value = true;
};
template <>
struct is_unsigned<UInt256> {
    static constexpr bool value = true;
};

template <typename T>
inline constexpr bool is_unsigned_v = is_unsigned<T>::value;

template <class T>
concept is_integer = std::is_integral_v<T> || std::is_same_v<T, Int128> ||
                     std::is_same_v<T, UInt128> || std::is_same_v<T, Int256> ||
                     std::is_same_v<T, UInt256>;

template <class T>
concept is_floating_point = std::is_floating_point_v<T>;

template <typename T>
struct is_arithmetic  // NOLINT(readability-identifier-naming)
{
    static constexpr bool value = std::is_arithmetic_v<T>;
};

template <>
struct is_arithmetic<Int128> {
    static constexpr bool value = true;
};
template <>
struct is_arithmetic<UInt128> {
    static constexpr bool value = true;
};
template <>
struct is_arithmetic<Int256> {
    static constexpr bool value = true;
};
template <>
struct is_arithmetic<UInt256> {
    static constexpr bool value = true;
};

template <typename T>
inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

template <typename T>
struct make_unsigned  // NOLINT(readability-identifier-naming)
{
    using type = std::make_unsigned_t<T>;
};

template <>
struct make_unsigned<Int128> {
    using type = UInt128;
};
template <>
struct make_unsigned<UInt128> {
    using type = UInt128;
};
template <>
struct make_unsigned<Int256> {
    using type = UInt256;
};
template <>
struct make_unsigned<UInt256> {
    using type = UInt256;
};

template <typename T>
using make_unsigned_t = typename make_unsigned<T>::type;

template <typename T>
struct make_signed  // NOLINT(readability-identifier-naming)
{
    using type = std::make_signed_t<T>;
};

template <>
struct make_signed<Int128> {
    using type = Int128;
};
template <>
struct make_signed<UInt128> {
    using type = Int128;
};
template <>
struct make_signed<Int256> {
    using type = Int256;
};
template <>
struct make_signed<UInt256> {
    using type = Int256;
};

template <typename T>
using make_signed_t = typename make_signed<T>::type;

template <typename T>
struct is_big_int  // NOLINT(readability-identifier-naming)
{
    static constexpr bool value = false;
};

template <>
struct is_big_int<Int128> {
    static constexpr bool value = true;
};
template <>
struct is_big_int<UInt128> {
    static constexpr bool value = true;
};
template <>
struct is_big_int<Int256> {
    static constexpr bool value = true;
};
template <>
struct is_big_int<UInt256> {
    static constexpr bool value = true;
};

template <typename T>
inline constexpr bool is_big_int_v = is_big_int<T>::value;

namespace wide {

template <size_t Bits, typename Signed>
class integer;

}

using Int128 = wide::integer<128, signed>;
using UInt128 = wide::integer<128, unsigned>;
using Int256 = wide::integer<256, signed>;
using UInt256 = wide::integer<256, unsigned>;

namespace DB {

template <class>
struct Decimal;

using Decimal32 = Decimal<Int32>;
using Decimal64 = Decimal<Int64>;
using Decimal128 = Decimal<Int128>;
using Decimal256 = Decimal<Int256>;

class DateTime64;

template <class T>
concept is_decimal =
    std::is_same_v<T, Decimal32> || std::is_same_v<T, Decimal64> ||
    std::is_same_v<T, Decimal128> || std::is_same_v<T, Decimal256> ||
    std::is_same_v<T, DateTime64>;

template <class T>
concept is_over_big_int =
    std::is_same_v<T, Int128> || std::is_same_v<T, UInt128> ||
    std::is_same_v<T, Int256> || std::is_same_v<T, UInt256> ||
    std::is_same_v<T, Decimal128> || std::is_same_v<T, Decimal256>;
}  // namespace DB

#if !defined(NO_SANITIZE_UNDEFINED)
#if defined(__clang__)
#define NO_SANITIZE_UNDEFINED __attribute__((__no_sanitize__("undefined")))
#else
#define NO_SANITIZE_UNDEFINED
#endif
#endif

namespace DB {
template <class>
struct Decimal;
class DateTime64;

using Decimal32 = Decimal<Int32>;
using Decimal64 = Decimal<Int64>;
using Decimal128 = Decimal<Int128>;
using Decimal256 = Decimal<Int256>;

template <class T>
struct NativeTypeT {
    using Type = T;
};
template <is_decimal T>
struct NativeTypeT<T> {
    using Type = typename T::NativeType;
};
template <class T>
using NativeType = typename NativeTypeT<T>::Type;

/// Own FieldType for Decimal.
/// It is only a "storage" for decimal.
/// To perform operations, you also have to provide a scale (number of digits
/// after point).
template <typename T>
struct Decimal {
    using NativeType = T;

    constexpr Decimal() = default;
    constexpr Decimal(Decimal<T> &&) noexcept = default;
    constexpr Decimal(const Decimal<T> &) = default;

    constexpr Decimal(const T &value_)
        : value(value_) {}  // NOLINT(google-explicit-constructor)

    template <typename U>
    constexpr Decimal(const Decimal<U> &x)
        : value(x.value) {}  // NOLINT(google-explicit-constructor)

    constexpr Decimal<T> &operator=(Decimal<T> &&) noexcept = default;
    constexpr Decimal<T> &operator=(const Decimal<T> &) = default;

    constexpr operator T() const {
        return value;
    }  // NOLINT(google-explicit-constructor)

    template <typename U>
    constexpr U convertTo() const {
        if constexpr (is_decimal<U>)
            return convertTo<typename U::NativeType>();
        else
            return static_cast<U>(value);
    }

    const Decimal<T> &operator+=(const T &x) {
        value += x;
        return *this;
    }
    const Decimal<T> &operator-=(const T &x) {
        value -= x;
        return *this;
    }
    const Decimal<T> &operator*=(const T &x) {
        value *= x;
        return *this;
    }
    const Decimal<T> &operator/=(const T &x) {
        value /= x;
        return *this;
    }
    const Decimal<T> &operator%=(const T &x) {
        value %= x;
        return *this;
    }

    template <typename U>
    const Decimal<T> &operator+=(const Decimal<U> &x) {
        value += x.value;
        return *this;
    }
    template <typename U>
    const Decimal<T> &operator-=(const Decimal<U> &x) {
        value -= x.value;
        return *this;
    }
    template <typename U>
    const Decimal<T> &operator*=(const Decimal<U> &x) {
        value *= x.value;
        return *this;
    }
    template <typename U>
    const Decimal<T> &operator/=(const Decimal<U> &x) {
        value /= x.value;
        return *this;
    }
    template <typename U>
    const Decimal<T> &operator%=(const Decimal<U> &x) {
        value %= x.value;
        return *this;
    }

    /// This is to avoid UB for sumWithOverflow()
    void NO_SANITIZE_UNDEFINED addOverflow(const T &x) { value += x; }

    T value;
};

template <typename T>
inline bool operator<(const Decimal<T> &x, const Decimal<T> &y) {
    return x.value < y.value;
}
template <typename T>
inline bool operator>(const Decimal<T> &x, const Decimal<T> &y) {
    return x.value > y.value;
}
template <typename T>
inline bool operator<=(const Decimal<T> &x, const Decimal<T> &y) {
    return x.value <= y.value;
}
template <typename T>
inline bool operator>=(const Decimal<T> &x, const Decimal<T> &y) {
    return x.value >= y.value;
}
template <typename T>
inline bool operator==(const Decimal<T> &x, const Decimal<T> &y) {
    return x.value == y.value;
}
template <typename T>
inline bool operator!=(const Decimal<T> &x, const Decimal<T> &y) {
    return x.value != y.value;
}

template <typename T>
inline Decimal<T> operator+(const Decimal<T> &x, const Decimal<T> &y) {
    return x.value + y.value;
}
template <typename T>
inline Decimal<T> operator-(const Decimal<T> &x, const Decimal<T> &y) {
    return x.value - y.value;
}
template <typename T>
inline Decimal<T> operator*(const Decimal<T> &x, const Decimal<T> &y) {
    return x.value * y.value;
}
template <typename T>
inline Decimal<T> operator/(const Decimal<T> &x, const Decimal<T> &y) {
    return x.value / y.value;
}
template <typename T>
inline Decimal<T> operator-(const Decimal<T> &x) {
    return -x.value;
}

/// Distinguishable type to allow function resolution/deduction based on value
/// type, but also relatively easy to convert to/from Decimal64.
class DateTime64 : public Decimal64 {
   public:
    using Base = Decimal64;
    using Base::Base;
    using NativeType = Base::NativeType;

    constexpr DateTime64(const Base &v)
        : Base(v) {}  // NOLINT(google-explicit-constructor)
};
}  // namespace DB

constexpr DB::UInt64 max_uint_mask = std::numeric_limits<DB::UInt64>::max();

namespace std {
template <typename T>
struct hash<DB::Decimal<T>> {
    size_t operator()(const DB::Decimal<T> &x) const {
        return hash<T>()(x.value);
    }
};

template <>
struct hash<DB::Decimal128> {
    size_t operator()(const DB::Decimal128 &x) const {
        return std::hash<DB::Int64>()(x.value >> 64) ^
               std::hash<DB::Int64>()(x.value & max_uint_mask);
    }
};

template <>
struct hash<DB::DateTime64> {
    size_t operator()(const DB::DateTime64 &x) const {
        return std::hash<DB::DateTime64::NativeType>()(x);
    }
};

template <>
struct hash<DB::Decimal256> {
    size_t operator()(const DB::Decimal256 &x) const {
        // FIXME temp solution
        return std::hash<DB::Int64>()(
                   static_cast<DB::Int64>(x.value >> 64 & max_uint_mask)) ^
               std::hash<DB::Int64>()(
                   static_cast<DB::Int64>(x.value & max_uint_mask));
    }
};
}  // namespace std

#pragma once

#include <Core/TypeId.h>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>
#include <base/strong_typedef.h>
#include <base/Decimal.h>

#pragma once

/// __has_feature supported only by clang.
///
/// But libcxx/libcxxabi overrides it to 0,
/// thus the checks for __has_feature will be wrong.
///
/// NOTE:
/// - __has_feature cannot be simply undefined,
///   since this will be broken if some C++ header will be included after
///   including <base/defines.h>
/// - it should not have fallback to 0,
///   since this may create false-positive detection (common problem)
#if defined(__clang__) && defined(__has_feature)
#    define ch_has_feature __has_feature
#endif

#if !defined(likely)
#    define likely(x)   (__builtin_expect(!!(x), 1))
#endif
#if !defined(unlikely)
#    define unlikely(x) (__builtin_expect(!!(x), 0))
#endif

// more aliases: https://mailman.videolan.org/pipermail/x264-devel/2014-May/010660.html

#define ALWAYS_INLINE __attribute__((__always_inline__))
#define NO_INLINE __attribute__((__noinline__))
#define MAY_ALIAS __attribute__((__may_alias__))

#if !defined(__x86_64__) && !defined(__aarch64__) && !defined(__PPC__) && !defined(__s390x__) && !(defined(__riscv) && (__riscv_xlen == 64))
#    error "The only supported platforms are x86_64 and AArch64, PowerPC (work in progress), s390x (work in progress) and RISC-V 64 (experimental)"
#endif

/// Check for presence of address sanitizer
#if !defined(ADDRESS_SANITIZER)
#    if defined(ch_has_feature)
#        if ch_has_feature(address_sanitizer)
#            define ADDRESS_SANITIZER 1
#        endif
#    elif defined(__SANITIZE_ADDRESS__)
#        define ADDRESS_SANITIZER 1
#    endif
#endif

#if !defined(THREAD_SANITIZER)
#    if defined(ch_has_feature)
#        if ch_has_feature(thread_sanitizer)
#            define THREAD_SANITIZER 1
#        endif
#    elif defined(__SANITIZE_THREAD__)
#        define THREAD_SANITIZER 1
#    endif
#endif

#if !defined(MEMORY_SANITIZER)
#    if defined(ch_has_feature)
#        if ch_has_feature(memory_sanitizer)
#            define MEMORY_SANITIZER 1
#        endif
#    elif defined(__MEMORY_SANITIZER__)
#        define MEMORY_SANITIZER 1
#    endif
#endif

#if !defined(UNDEFINED_BEHAVIOR_SANITIZER)
#    if defined(__has_feature)
#        if __has_feature(undefined_behavior_sanitizer)
#            define UNDEFINED_BEHAVIOR_SANITIZER 1
#        endif
#    elif defined(__UNDEFINED_BEHAVIOR_SANITIZER__)
#        define UNDEFINED_BEHAVIOR_SANITIZER 1
#    endif
#endif

/// Explicitly allow undefined behaviour for certain functions. Use it as a function attribute.
/// It is useful in case when compiler cannot see (and exploit) it, but UBSan can.
/// Example: multiplication of signed integers with possibility of overflow when both sides are from user input.
#if defined(__clang__)
#    define NO_SANITIZE_UNDEFINED __attribute__((__no_sanitize__("undefined")))
#    define NO_SANITIZE_ADDRESS __attribute__((__no_sanitize__("address")))
#    define NO_SANITIZE_THREAD __attribute__((__no_sanitize__("thread")))
#    define ALWAYS_INLINE_NO_SANITIZE_UNDEFINED __attribute__((__always_inline__, __no_sanitize__("undefined")))
#else  /// It does not work in GCC. GCC 7 cannot recognize this attribute and GCC 8 simply ignores it.
#    define NO_SANITIZE_UNDEFINED
#    define NO_SANITIZE_ADDRESS
#    define NO_SANITIZE_THREAD
#    define ALWAYS_INLINE_NO_SANITIZE_UNDEFINED ALWAYS_INLINE
#endif

#if defined(__clang__) && defined(__clang_major__) && __clang_major__ >= 14
#    define DISABLE_SANITIZER_INSTRUMENTATION __attribute__((disable_sanitizer_instrumentation))
#else
#    define DISABLE_SANITIZER_INSTRUMENTATION
#endif


#if !__has_include(<sanitizer/asan_interface.h>) || !defined(ADDRESS_SANITIZER)
#   define ASAN_UNPOISON_MEMORY_REGION(a, b)
#   define ASAN_POISON_MEMORY_REGION(a, b)
#endif

#if !defined(ABORT_ON_LOGICAL_ERROR)
    #if !defined(NDEBUG) || defined(ADDRESS_SANITIZER) || defined(THREAD_SANITIZER) || defined(MEMORY_SANITIZER) || defined(UNDEFINED_BEHAVIOR_SANITIZER)
        #define ABORT_ON_LOGICAL_ERROR
    #endif
#endif

/// chassert(x) is similar to assert(x), but:
///     - works in builds with sanitizers, not only in debug builds
///     - tries to print failed assertion into server log
/// It can be used for all assertions except heavy ones.
/// Heavy assertions (that run loops or call complex functions) are allowed in debug builds only.
/// Also it makes sense to call abort() instead of __builtin_unreachable() in debug builds,
/// because SIGABRT is easier to debug than SIGTRAP (the second one makes gdb crazy)
#if !defined(chassert)
    #if defined(ABORT_ON_LOGICAL_ERROR)
        // clang-format off
        #include <base/types.h>
        namespace DB
        {
            void abortOnFailedAssertion(const String & description);
        }
        #define chassert(x) static_cast<bool>(x) ? void(0) : ::DB::abortOnFailedAssertion(#x)
        #define UNREACHABLE() abort()
        // clang-format off
    #else
        /// Here sizeof() trick is used to suppress unused warning for result,
        /// since simple "(void)x" will evaluate the expression, while
        /// "sizeof(!(x))" will not.
        #define NIL_EXPRESSION(x) (void)sizeof(!(x))
        #define chassert(x) NIL_EXPRESSION(x)
        #define UNREACHABLE() __builtin_unreachable()
    #endif
#endif

/// Macros for Clang Thread Safety Analysis (TSA). They can be safely ignored by other compilers.
/// Feel free to extend, but please stay close to https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#mutexheader
#if defined(__clang__)
#    define TSA_GUARDED_BY(...) __attribute__((guarded_by(__VA_ARGS__)))                       /// data is protected by given capability
#    define TSA_PT_GUARDED_BY(...) __attribute__((pt_guarded_by(__VA_ARGS__)))                 /// pointed-to data is protected by the given capability
#    define TSA_REQUIRES(...) __attribute__((requires_capability(__VA_ARGS__)))                /// thread needs exclusive possession of given capability
#    define TSA_REQUIRES_SHARED(...) __attribute__((requires_shared_capability(__VA_ARGS__)))  /// thread needs shared possession of given capability
#    define TSA_ACQUIRED_AFTER(...) __attribute__((acquired_after(__VA_ARGS__)))               /// annotated lock must be locked after given lock
#    define TSA_NO_THREAD_SAFETY_ANALYSIS __attribute__((no_thread_safety_analysis))           /// disable TSA for a function
#    define TSA_CAPABILITY(...) __attribute__((capability(__VA_ARGS__)))                       /// object of a class can be used as capability
#    define TSA_ACQUIRE(...) __attribute__((acquire_capability(__VA_ARGS__)))                        /// function acquires a capability, but does not release it
#    define TSA_TRY_ACQUIRE(...) __attribute__((try_acquire_capability(__VA_ARGS__)))                /// function tries to acquire a capability and returns a boolean value indicating success or failure
#    define TSA_RELEASE(...) __attribute__((release_capability(__VA_ARGS__)))                        /// function releases the given capability
#    define TSA_ACQUIRE_SHARED(...) __attribute__((acquire_shared_capability(__VA_ARGS__)))          /// function acquires a shared capability, but does not release it
#    define TSA_TRY_ACQUIRE_SHARED(...) __attribute__((try_acquire_shared_capability(__VA_ARGS__)))  /// function tries to acquire a shared capability and returns a boolean value indicating success or failure
#    define TSA_RELEASE_SHARED(...) __attribute__((release_shared_capability(__VA_ARGS__)))          /// function releases the given shared capability
#    define TSA_SCOPED_LOCKABLE __attribute__((scoped_lockable)) /// object of a class has scoped lockable capability

/// Macros for suppressing TSA warnings for specific reads/writes (instead of suppressing it for the whole function)
/// They use a lambda function to apply function attribute to a single statement. This enable us to suppress warnings locally instead of
/// suppressing them in the whole function
/// Consider adding a comment when using these macros.
#   define TSA_SUPPRESS_WARNING_FOR_READ(x) ([&]() TSA_NO_THREAD_SAFETY_ANALYSIS -> const auto & { return (x); }())
#   define TSA_SUPPRESS_WARNING_FOR_WRITE(x) ([&]() TSA_NO_THREAD_SAFETY_ANALYSIS -> auto & { return (x); }())

/// This macro is useful when only one thread writes to a member
/// and you want to read this member from the same thread without locking a mutex.
/// It's safe (because no concurrent writes are possible), but TSA generates a warning.
/// (Seems like there's no way to verify it, but it makes sense to distinguish it from TSA_SUPPRESS_WARNING_FOR_READ for readability)
#   define TSA_READ_ONE_THREAD(x) TSA_SUPPRESS_WARNING_FOR_READ(x)

#else
#    define TSA_GUARDED_BY(...)
#    define TSA_PT_GUARDED_BY(...)
#    define TSA_REQUIRES(...)
#    define TSA_REQUIRES_SHARED(...)
#    define TSA_NO_THREAD_SAFETY_ANALYSIS
#    define TSA_CAPABILITY(...)
#    define TSA_ACQUIRE(...)
#    define TSA_TRY_ACQUIRE(...)
#    define TSA_RELEASE(...)
#    define TSA_ACQUIRE_SHARED(...)
#    define TSA_TRY_ACQUIRE_SHARED(...)
#    define TSA_RELEASE_SHARED(...)
#    define TSA_SCOPED_LOCKABLE

#    define TSA_SUPPRESS_WARNING_FOR_READ(x) (x)
#    define TSA_SUPPRESS_WARNING_FOR_WRITE(x) (x)
#    define TSA_READ_ONE_THREAD(x) TSA_SUPPRESS_WARNING_FOR_READ(x)
#endif

/// A template function for suppressing warnings about unused variables or function results.
template <typename... Args>
constexpr void UNUSED(Args &&... args [[maybe_unused]]) // NOLINT(cppcoreguidelines-missing-std-forward)
{
}

#include <base/UUID.h>
#include <base/IPv4andIPv6.h>


namespace DB
{

/// Data types for representing elementary values from a database in RAM.

/// Hold a null value for untyped calculation. It can also store infinities to handle nullable
/// comparison which is used for nullable KeyCondition.
struct Null
{
    enum class Value
    {
        Null,
        PositiveInfinity,
        NegativeInfinity,
    };

    Value value{Value::Null};

    bool isNull() const { return value == Value::Null; }
    bool isPositiveInfinity() const { return value == Value::PositiveInfinity; }
    bool isNegativeInfinity() const { return value == Value::NegativeInfinity; }

    bool operator==(const Null & other) const
    {
        return value == other.value;
    }

    bool operator!=(const Null & other) const
    {
        return !(*this == other);
    }
};

using UInt128 = ::UInt128;
using UInt256 = ::UInt256;
using Int128 = ::Int128;
using Int256 = ::Int256;

/// Not a data type in database, defined just for convenience.
using Strings = std::vector<String>;
using TypeIndexesSet = std::unordered_set<TypeIndex>;
}

#pragma once

#include <cstdint>
#include <limits>

// Also defined in Core/Defines.h
#if !defined(NO_SANITIZE_UNDEFINED)
#if defined(__clang__)
    #define NO_SANITIZE_UNDEFINED __attribute__((__no_sanitize__("undefined")))
#else
    #define NO_SANITIZE_UNDEFINED
#endif
#endif


/// On overflow, the function returns unspecified value.
inline NO_SANITIZE_UNDEFINED uint64_t intExp2(int x)
{
    return 1ULL << x;
}

constexpr inline uint64_t intExp10(int x)
{
    if (x < 0)
        return 0;
    if (x > 19)
        return std::numeric_limits<uint64_t>::max();

    constexpr uint64_t table[20] =
    {
        1ULL,                   10ULL,                   100ULL,
        1000ULL,                10000ULL,                100000ULL,
        1000000ULL,             10000000ULL,             100000000ULL,
        1000000000ULL,          10000000000ULL,          100000000000ULL,
        1000000000000ULL,       10000000000000ULL,       100000000000000ULL,
        1000000000000000ULL,    10000000000000000ULL,    100000000000000000ULL,
        1000000000000000000ULL, 10000000000000000000ULL
    };

    return table[x];
}

namespace common
{

constexpr inline int exp10_i32(int x)
{
    if (x < 0)
        return 0;
    if (x > 9)
        return std::numeric_limits<int>::max();

    constexpr int values[] =
    {
        1,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000
    };
    return values[x];
}

constexpr inline int64_t exp10_i64(int x)
{
    if (x < 0)
        return 0;
    if (x > 18)
        return std::numeric_limits<int64_t>::max();

    constexpr int64_t values[] =
    {
        1LL,
        10LL,
        100LL,
        1000LL,
        10000LL,
        100000LL,
        1000000LL,
        10000000LL,
        100000000LL,
        1000000000LL,
        10000000000LL,
        100000000000LL,
        1000000000000LL,
        10000000000000LL,
        100000000000000LL,
        1000000000000000LL,
        10000000000000000LL,
        100000000000000000LL,
        1000000000000000000LL
    };
    return values[x];
}

constexpr inline Int128 exp10_i128(int x)
{
    if (x < 0)
        return 0;
    if (x > 38)
        return std::numeric_limits<Int128>::max();

    constexpr Int128 values[] =
    {
        static_cast<Int128>(1LL),
        static_cast<Int128>(10LL),
        static_cast<Int128>(100LL),
        static_cast<Int128>(1000LL),
        static_cast<Int128>(10000LL),
        static_cast<Int128>(100000LL),
        static_cast<Int128>(1000000LL),
        static_cast<Int128>(10000000LL),
        static_cast<Int128>(100000000LL),
        static_cast<Int128>(1000000000LL),
        static_cast<Int128>(10000000000LL),
        static_cast<Int128>(100000000000LL),
        static_cast<Int128>(1000000000000LL),
        static_cast<Int128>(10000000000000LL),
        static_cast<Int128>(100000000000000LL),
        static_cast<Int128>(1000000000000000LL),
        static_cast<Int128>(10000000000000000LL),
        static_cast<Int128>(100000000000000000LL),
        static_cast<Int128>(1000000000000000000LL),
        static_cast<Int128>(1000000000000000000LL) * 10LL,
        static_cast<Int128>(1000000000000000000LL) * 100LL,
        static_cast<Int128>(1000000000000000000LL) * 1000LL,
        static_cast<Int128>(1000000000000000000LL) * 10000LL,
        static_cast<Int128>(1000000000000000000LL) * 100000LL,
        static_cast<Int128>(1000000000000000000LL) * 1000000LL,
        static_cast<Int128>(1000000000000000000LL) * 10000000LL,
        static_cast<Int128>(1000000000000000000LL) * 100000000LL,
        static_cast<Int128>(1000000000000000000LL) * 1000000000LL,
        static_cast<Int128>(1000000000000000000LL) * 10000000000LL,
        static_cast<Int128>(1000000000000000000LL) * 100000000000LL,
        static_cast<Int128>(1000000000000000000LL) * 1000000000000LL,
        static_cast<Int128>(1000000000000000000LL) * 10000000000000LL,
        static_cast<Int128>(1000000000000000000LL) * 100000000000000LL,
        static_cast<Int128>(1000000000000000000LL) * 1000000000000000LL,
        static_cast<Int128>(1000000000000000000LL) * 10000000000000000LL,
        static_cast<Int128>(1000000000000000000LL) * 100000000000000000LL,
        static_cast<Int128>(1000000000000000000LL) * 100000000000000000LL * 10LL,
        static_cast<Int128>(1000000000000000000LL) * 100000000000000000LL * 100LL,
        static_cast<Int128>(1000000000000000000LL) * 100000000000000000LL * 1000LL
    };
    return values[x];
}


inline Int256 exp10_i256(int x)
{
    if (x < 0)
        return 0;
    if (x > 76)
        return std::numeric_limits<Int256>::max();

    using Int256 = Int256;
    static constexpr Int256 i10e18{1000000000000000000ll};
    static const Int256 values[] = {
        static_cast<Int256>(1ll),
        static_cast<Int256>(10ll),
        static_cast<Int256>(100ll),
        static_cast<Int256>(1000ll),
        static_cast<Int256>(10000ll),
        static_cast<Int256>(100000ll),
        static_cast<Int256>(1000000ll),
        static_cast<Int256>(10000000ll),
        static_cast<Int256>(100000000ll),
        static_cast<Int256>(1000000000ll),
        static_cast<Int256>(10000000000ll),
        static_cast<Int256>(100000000000ll),
        static_cast<Int256>(1000000000000ll),
        static_cast<Int256>(10000000000000ll),
        static_cast<Int256>(100000000000000ll),
        static_cast<Int256>(1000000000000000ll),
        static_cast<Int256>(10000000000000000ll),
        static_cast<Int256>(100000000000000000ll),
        i10e18,
        i10e18 * 10ll,
        i10e18 * 100ll,
        i10e18 * 1000ll,
        i10e18 * 10000ll,
        i10e18 * 100000ll,
        i10e18 * 1000000ll,
        i10e18 * 10000000ll,
        i10e18 * 100000000ll,
        i10e18 * 1000000000ll,
        i10e18 * 10000000000ll,
        i10e18 * 100000000000ll,
        i10e18 * 1000000000000ll,
        i10e18 * 10000000000000ll,
        i10e18 * 100000000000000ll,
        i10e18 * 1000000000000000ll,
        i10e18 * 10000000000000000ll,
        i10e18 * 100000000000000000ll,
        i10e18 * 100000000000000000ll * 10ll,
        i10e18 * 100000000000000000ll * 100ll,
        i10e18 * 100000000000000000ll * 1000ll,
        i10e18 * 100000000000000000ll * 10000ll,
        i10e18 * 100000000000000000ll * 100000ll,
        i10e18 * 100000000000000000ll * 1000000ll,
        i10e18 * 100000000000000000ll * 10000000ll,
        i10e18 * 100000000000000000ll * 100000000ll,
        i10e18 * 100000000000000000ll * 1000000000ll,
        i10e18 * 100000000000000000ll * 10000000000ll,
        i10e18 * 100000000000000000ll * 100000000000ll,
        i10e18 * 100000000000000000ll * 1000000000000ll,
        i10e18 * 100000000000000000ll * 10000000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000ll,
        i10e18 * 100000000000000000ll * 1000000000000000ll,
        i10e18 * 100000000000000000ll * 10000000000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 10ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 1000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 10000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 1000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 10000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 1000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 10000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 1000000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 10000000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 1000000000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 10000000000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000000000000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000000000000000ll * 10ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000000000000000ll * 100ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000000000000000ll * 1000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000000000000000ll * 10000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000000000000000ll * 100000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000000000000000ll * 1000000ll,
        i10e18 * 100000000000000000ll * 100000000000000000ll * 100000000000000000ll * 10000000ll,
    };
    return values[x];
}

}


/// intExp10 returning the type T.
template <typename T>
constexpr inline T intExp10OfSize(int x)
{
    if constexpr (sizeof(T) <= 4)
        return static_cast<T>(common::exp10_i32(x));
    else if constexpr (sizeof(T) <= 8)
        return common::exp10_i64(x);
    else if constexpr (sizeof(T) <= 16)
        return common::exp10_i128(x);
    else
        return common::exp10_i256(x);
}

// NOLINTBEGIN(google-runtime-int)

namespace common
{
    /// Multiply and ignore overflow.
    template <typename T1, typename T2>
    inline auto NO_SANITIZE_UNDEFINED mulIgnoreOverflow(T1 x, T2 y)
    {
        return x * y;
    }

    template <typename T1, typename T2>
    inline auto NO_SANITIZE_UNDEFINED addIgnoreOverflow(T1 x, T2 y)
    {
        return x + y;
    }

    template <typename T1, typename T2>
    inline auto NO_SANITIZE_UNDEFINED subIgnoreOverflow(T1 x, T2 y)
    {
        return x - y;
    }

    template <typename T>
    inline auto NO_SANITIZE_UNDEFINED negateIgnoreOverflow(T x)
    {
        return -x;
    }

    template <typename T>
    inline bool addOverflow(T x, T y, T & res)
    {
        return __builtin_add_overflow(x, y, &res);
    }

    template <>
    inline bool addOverflow(int x, int y, int & res)
    {
        return __builtin_sadd_overflow(x, y, &res);
    }

    template <>
    inline bool addOverflow(long x, long y, long & res)
    {
        return __builtin_saddl_overflow(x, y, &res);
    }

    template <>
    inline bool addOverflow(long long x, long long y, long long & res)
    {
        return __builtin_saddll_overflow(x, y, &res);
    }

    template <>
    inline bool addOverflow(Int128 x, Int128 y, Int128 & res)
    {
        res = addIgnoreOverflow(x, y);
        return (y > 0 && x > std::numeric_limits<Int128>::max() - y) ||
            (y < 0 && x < std::numeric_limits<Int128>::min() - y);
    }

    template <>
    inline bool addOverflow(UInt128 x, UInt128 y, UInt128 & res)
    {
        res = addIgnoreOverflow(x, y);
        return x > std::numeric_limits<UInt128>::max() - y;
    }

    template <>
    inline bool addOverflow(Int256 x, Int256 y, Int256 & res)
    {
        res = addIgnoreOverflow(x, y);
        return (y > 0 && x > std::numeric_limits<Int256>::max() - y) ||
            (y < 0 && x < std::numeric_limits<Int256>::min() - y);
    }

    template <>
    inline bool addOverflow(UInt256 x, UInt256 y, UInt256 & res)
    {
        res = addIgnoreOverflow(x, y);
        return x > std::numeric_limits<UInt256>::max() - y;
    }

    template <typename T>
    inline bool subOverflow(T x, T y, T & res)
    {
        return __builtin_sub_overflow(x, y, &res);
    }

    template <>
    inline bool subOverflow(int x, int y, int & res)
    {
        return __builtin_ssub_overflow(x, y, &res);
    }

    template <>
    inline bool subOverflow(long x, long y, long & res)
    {
        return __builtin_ssubl_overflow(x, y, &res);
    }

    template <>
    inline bool subOverflow(long long x, long long y, long long & res)
    {
        return __builtin_ssubll_overflow(x, y, &res);
    }

    template <>
    inline bool subOverflow(Int128 x, Int128 y, Int128 & res)
    {
        res = subIgnoreOverflow(x, y);
        return (y < 0 && x > std::numeric_limits<Int128>::max() + y) ||
            (y > 0 && x < std::numeric_limits<Int128>::min() + y);
    }

    template <>
    inline bool subOverflow(UInt128 x, UInt128 y, UInt128 & res)
    {
        res = subIgnoreOverflow(x, y);
        return x < y;
    }

    template <>
    inline bool subOverflow(Int256 x, Int256 y, Int256 & res)
    {
        res = subIgnoreOverflow(x, y);
        return (y < 0 && x > std::numeric_limits<Int256>::max() + y) ||
            (y > 0 && x < std::numeric_limits<Int256>::min() + y);
    }

    template <>
    inline bool subOverflow(UInt256 x, UInt256 y, UInt256 & res)
    {
        res = subIgnoreOverflow(x, y);
        return x < y;
    }

    template <typename T>
    inline bool mulOverflow(T x, T y, T & res)
    {
        return __builtin_mul_overflow(x, y, &res);
    }

    template <typename T, typename U, typename R>
    inline bool mulOverflow(T x, U y, R & res)
    {
        // not built in type, wide integer
        if constexpr (is_big_int_v<T>  || is_big_int_v<R> || is_big_int_v<U>)
        {
            res = mulIgnoreOverflow<R>(x, y);
            return false;
        }
        else
            return __builtin_mul_overflow(x, y, &res);
    }

    template <>
    inline bool mulOverflow(int x, int y, int & res)
    {
        return __builtin_smul_overflow(x, y, &res);
    }

    template <>
    inline bool mulOverflow(long x, long y, long & res)
    {
        return __builtin_smull_overflow(x, y, &res);
    }

    template <>
    inline bool mulOverflow(long long x, long long y, long long & res)
    {
        return __builtin_smulll_overflow(x, y, &res);
    }

    /// Overflow check is not implemented for big integers.

    template <>
    inline bool mulOverflow(Int128 x, Int128 y, Int128 & res)
    {
        res = mulIgnoreOverflow(x, y);
        return false;
    }

    template <>
    inline bool mulOverflow(Int256 x, Int256 y, Int256 & res)
    {
        res = mulIgnoreOverflow(x, y);
        return false;
    }

    template <>
    inline bool mulOverflow(UInt128 x, UInt128 y, UInt128 & res)
    {
        res = mulIgnoreOverflow(x, y);
        return false;
    }

    template <>
    inline bool mulOverflow(UInt256 x, UInt256 y, UInt256 & res)
    {
        res = mulIgnoreOverflow(x, y);
        return false;
    }
}

// NOLINTEND(google-runtime-int)

#include <limits>
#include <type_traits>


namespace DB
{

template <typename T>
class DataTypeNumber;

namespace ErrorCodes
{
    extern const int DECIMAL_OVERFLOW;
    extern const int ARGUMENT_OUT_OF_BOUND;
}

namespace DecimalUtils
{

inline constexpr size_t min_precision = 1;
template <typename T> inline constexpr size_t max_precision = 0;
template <> inline constexpr size_t max_precision<Decimal32> = 9;
template <> inline constexpr size_t max_precision<Decimal64> = 18;
template <> inline constexpr size_t max_precision<DateTime64> = 18;
template <> inline constexpr size_t max_precision<Decimal128> = 38;
template <> inline constexpr size_t max_precision<Decimal256> = 76;

template <typename T>
inline auto scaleMultiplier(UInt32 scale)
{
    if constexpr (std::is_same_v<T, Int32> || std::is_same_v<T, Decimal32>)
        return common::exp10_i32(scale);
    else if constexpr (std::is_same_v<T, Int64> || std::is_same_v<T, Decimal64> || std::is_same_v<T, DateTime64>)
        return common::exp10_i64(scale);
    else if constexpr (std::is_same_v<T, Int128> || std::is_same_v<T, Decimal128>)
        return common::exp10_i128(scale);
    else if constexpr (std::is_same_v<T, Int256> || std::is_same_v<T, Decimal256>)
        return common::exp10_i256(scale);
}


/** Components of DecimalX value:
 * whole - represents whole part of decimal, can be negative or positive.
 * fractional - for fractional part of decimal.
 *
 *  0.123 represents  0 /  0.123
 * -0.123 represents  0 / -0.123
 * -1.123 represents -1 /  0.123
 */
template <typename DecimalType>
struct DecimalComponents
{
    using T = typename DecimalType::NativeType;
    T whole;
    T fractional;
};

/// Traits used for determining final Type/Precision/Scale for certain math operations on decimals.
template <typename T>
struct DataTypeDecimalTrait
{
    using FieldType = T;
    const UInt32 precision;
    const UInt32 scale;

    DataTypeDecimalTrait(UInt32 precision_, UInt32 scale_)
        : precision(precision_),
          scale(scale_)
    {}

    /// @returns multiplier for U to become T with correct scale
    template <typename U>
    T scaleFactorFor(const DataTypeDecimalTrait<U> & x, bool) const
    {
        if (scale < x.scale)
            throw "Decimal result's scale is less than argument's one";
        const UInt32 scale_delta = scale - x.scale; /// scale_delta >= 0
        return DecimalUtils::scaleMultiplier<typename T::NativeType>(scale_delta);
    }
};

/// Calculates result = x * multiplier + delta.
/// If the multiplication or the addition overflows, returns false or throws DECIMAL_OVERFLOW.
template <typename T, bool throw_on_error>
inline bool multiplyAdd(const T & x, const T & multiplier, const T & delta, T & result)
{
    T multiplied = 0;
    if (common::mulOverflow(x, multiplier, multiplied))
    {
        if constexpr (throw_on_error)
            throw "Decimal math overflow";
        return false;
    }

    if (common::addOverflow(multiplied, delta, result))
    {
        if constexpr (throw_on_error)
            throw "Decimal math overflow";
        return false;
    }

    return true;
}

template <typename T>
inline T multiplyAdd(const T & x, const T & multiplier, const T & delta)
{
    T res;
    multiplyAdd<T, true>(x, multiplier, delta, res);
    return res;
}

/** Make a decimal value from whole and fractional components with given scale multiplier.
  * where scale_multiplier = scaleMultiplier<T>(scale)
  * this is to reduce number of calls to scaleMultiplier when scale is known.
  *
  * Sign of `whole` controls sign of result: negative whole => negative result, positive whole => positive result.
  * Sign of `fractional` is expected to be positive, otherwise result is undefined.
  * If `scale` is to big (scale > max_precision<DecimalType::NativeType>), result is undefined.
  */

template <typename DecimalType, bool throw_on_error>
inline bool decimalFromComponentsWithMultiplierImpl(
    const typename DecimalType::NativeType & whole,
    const typename DecimalType::NativeType & fractional,
    typename DecimalType::NativeType scale_multiplier,
    DecimalType & result)
{
    using T = typename DecimalType::NativeType;
    const auto fractional_sign = whole < 0 ? -1 : 1;
    T value;
    if (!multiplyAdd<T, throw_on_error>(
            whole, scale_multiplier, fractional_sign * (fractional % scale_multiplier), value))
        return false;
    result = DecimalType(value);
    return true;
}

template <typename DecimalType>
inline DecimalType decimalFromComponentsWithMultiplier(
        const typename DecimalType::NativeType & whole,
        const typename DecimalType::NativeType & fractional,
        typename DecimalType::NativeType scale_multiplier)
{
    DecimalType result;
    decimalFromComponentsWithMultiplierImpl<DecimalType, true>(whole, fractional, scale_multiplier, result);
    return result;
}

template <typename DecimalType>
inline bool tryGetDecimalFromComponentsWithMultiplier(
    const typename DecimalType::NativeType & whole,
    const typename DecimalType::NativeType & fractional,
    typename DecimalType::NativeType scale_multiplier,
    DecimalType & result)
{
    return decimalFromComponentsWithMultiplierImpl<DecimalType, false>(whole, fractional, scale_multiplier, result);
}

template <typename DecimalType>
inline DecimalType decimalFromComponentsWithMultiplier(
        const DecimalComponents<DecimalType> & components,
        typename DecimalType::NativeType scale_multiplier)
{
    return decimalFromComponentsWithMultiplier<DecimalType>(components.whole, components.fractional, scale_multiplier);
}

template <typename DecimalType>
inline bool tryGetDecimalFromComponentsWithMultiplier(
    const DecimalComponents<DecimalType> & components,
    typename DecimalType::NativeType scale_multiplier,
    DecimalType & result)
{
    return tryGetDecimalFromComponentsWithMultiplier<DecimalType>(components.whole, components.fractional, scale_multiplier, result);
}


/** Make a decimal value from whole and fractional components with given scale.
 *
 * @see `decimalFromComponentsWithMultiplier` for details.
 */
template <typename DecimalType>
inline DecimalType decimalFromComponents(
        const typename DecimalType::NativeType & whole,
        const typename DecimalType::NativeType & fractional,
        UInt32 scale)
{
    using T = typename DecimalType::NativeType;

    return decimalFromComponentsWithMultiplier<DecimalType>(whole, fractional, scaleMultiplier<T>(scale));
}

template <typename DecimalType>
inline bool tryGetDecimalFromComponents(
    const typename DecimalType::NativeType & whole,
    const typename DecimalType::NativeType & fractional,
    UInt32 scale,
    DecimalType & result)
{
    using T = typename DecimalType::NativeType;

    return tryGetDecimalFromComponentsWithMultiplier<DecimalType>(whole, fractional, scaleMultiplier<T>(scale), result);
}

/** Make a decimal value from whole and fractional components with given scale.
 * @see `decimalFromComponentsWithMultiplier` for details.
 */
template <typename DecimalType>
inline DecimalType decimalFromComponents(
        const DecimalComponents<DecimalType> & components,
        UInt32 scale)
{
    return decimalFromComponents<DecimalType>(components.whole, components.fractional, scale);
}

template <typename DecimalType>
inline bool tryGetDecimalFromComponents(
    const DecimalComponents<DecimalType> & components,
    UInt32 scale,
    DecimalType & result)
{
    return tryGetDecimalFromComponents<DecimalType>(components.whole, components.fractional, scale, result);
}

/** Split decimal into whole and fractional parts with given scale_multiplier.
 * This is an optimization to reduce number of calls to scaleMultiplier on known scale.
 */
template <typename DecimalType>
inline DecimalComponents<DecimalType> splitWithScaleMultiplier(
        const DecimalType & decimal,
        typename DecimalType::NativeType scale_multiplier)
{
    using T = typename DecimalType::NativeType;
    const auto whole = decimal.value / scale_multiplier;
    auto fractional = decimal.value % scale_multiplier;
    if (whole && fractional < T(0))
        fractional *= T(-1);

    return {whole, fractional};
}

/// Split decimal into components: whole and fractional part, @see `DecimalComponents` for details.
template <typename DecimalType>
inline DecimalComponents<DecimalType> split(const DecimalType & decimal, UInt32 scale)
{
    if (scale == 0)
    {
        return {decimal.value, 0};
    }
    return splitWithScaleMultiplier(decimal, scaleMultiplier<typename DecimalType::NativeType>(scale));
}

/** Get whole part from decimal.
 *
 * Sign of result follows sign of `decimal` value.
 * If scale is to big, result is undefined.
 */
template <typename DecimalType>
inline typename DecimalType::NativeType getWholePart(const DecimalType & decimal, UInt32 scale)
{
    if (scale == 0)
        return decimal.value;

    return decimal.value / scaleMultiplier<typename DecimalType::NativeType>(scale);
}


template <typename DecimalType, bool keep_sign = false>
inline typename DecimalType::NativeType getFractionalPartWithScaleMultiplier(
        const DecimalType & decimal,
        typename DecimalType::NativeType scale_multiplier)
{
    using T = typename DecimalType::NativeType;

    /// There's UB with min integer value here. But it does not matter for Decimals cause they use not full integer ranges.
    /// Anycase we make modulo before compare to make scale_multiplier > 1 unaffected.
    T result = decimal.value % scale_multiplier;
    if constexpr (!keep_sign)
        if (decimal.value / scale_multiplier && result < T(0))
            result = -result;

    return result;
}

/** Get fractional part from decimal
 *
 * Result is always positive.
 * If scale is to big, result is undefined.
 */
template <typename DecimalType>
inline typename DecimalType::NativeType getFractionalPart(const DecimalType & decimal, UInt32 scale)
{
    if (scale == 0)
        return 0;

    return getFractionalPartWithScaleMultiplier(decimal, scaleMultiplier<typename DecimalType::NativeType>(scale));
}

/// Decimal to integer/float conversion
template <typename To, typename DecimalType, typename ReturnType>
ReturnType convertToImpl(const DecimalType & decimal, UInt32 scale, To & result)
{
    using DecimalNativeType = typename DecimalType::NativeType;
    static constexpr bool throw_exception = std::is_void_v<ReturnType>;

    if constexpr (std::is_floating_point_v<To>)
    {
        result = static_cast<To>(decimal.value) / static_cast<To>(scaleMultiplier<DecimalNativeType>(scale));
    }
    else if constexpr (is_integer<To> && (sizeof(To) >= sizeof(DecimalNativeType)))
    {
        DecimalNativeType whole = getWholePart(decimal, scale);

        if constexpr (is_unsigned_v<To>)
        {
            if (whole < 0)
            {
                if constexpr (throw_exception)
                    throw "Convert overflow";
                else
                    return ReturnType(true);
            }
        }

        result = static_cast<To>(whole);
    }
    else if constexpr (is_integer<To>)
    {
        using CastTo = std::conditional_t<(is_big_int_v<DecimalNativeType> && std::is_same_v<To, UInt8>), uint8_t, To>;

        const DecimalNativeType whole = getWholePart(decimal, scale);

        static const constexpr CastTo min_to = std::numeric_limits<To>::min();
        static const constexpr CastTo max_to = std::numeric_limits<To>::max();

        if (whole < min_to || whole > max_to)
        {
            if constexpr (throw_exception)
                throw "Convert overflow";
            else
                return ReturnType(true);
        }

        result = static_cast<CastTo>(whole);
    }

    return ReturnType(true);
}


template <typename To, typename DecimalType>
To convertTo(const DecimalType & decimal, UInt32 scale)
{
    To result;
    convertToImpl<To, DecimalType, void>(decimal, scale, result);
    return result;
}

template <typename To, typename DecimalType>
bool tryConvertTo(const DecimalType & decimal, UInt32 scale, To & result)
{
    return convertToImpl<To, DecimalType, bool>(decimal, scale, result);
}

template <bool is_multiply, bool is_division, typename T, typename U, template <typename> typename DecimalType>
inline auto binaryOpResult(const DecimalType<T> & tx, const DecimalType<U> & ty)
{
    UInt32 scale{};
    if constexpr (is_multiply)
        scale = tx.getScale() + ty.getScale();
    else if constexpr (is_division)
        scale = tx.getScale();
    else
        scale = (tx.getScale() > ty.getScale() ? tx.getScale() : ty.getScale());

    if constexpr (sizeof(T) < sizeof(U))
        return DataTypeDecimalTrait<U>(DecimalUtils::max_precision<U>, scale);
    else
        return DataTypeDecimalTrait<T>(DecimalUtils::max_precision<T>, scale);
}

template <bool, bool, typename T, typename U, template <typename> typename DecimalType>
inline DataTypeDecimalTrait<T> binaryOpResult(const DecimalType<T> & tx, const DataTypeNumber<U> &)
{
    return DataTypeDecimalTrait<T>(DecimalUtils::max_precision<T>, tx.getScale());
}

template <bool, bool, typename T, typename U, template <typename> typename DecimalType>
inline DataTypeDecimalTrait<U> binaryOpResult(const DataTypeNumber<T> &, const DecimalType<U> & ty)
{
    return DataTypeDecimalTrait<U>(DecimalUtils::max_precision<U>, ty.getScale());
}

}

}

#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    DB::Decimal128 a = DB::Decimal128(10000);
    DB::Decimal128 b = DB::Decimal128(20000);
    DB::Decimal128 c = a + b;
    std::cout << c.convertTo<Int64>() << std::endl;

    return 0;
}