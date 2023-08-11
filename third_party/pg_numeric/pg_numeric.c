#include "pg_numeric.h"

#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "varatt.h"

#define Assert assert

/* ----------
 * Uncomment the following to enable compilation of dump_numeric()
 * and dump_var() and to get a dump of any result produced by make_result().
 * ----------
#define NUMERIC_DEBUG
 */

/* ----------
 * Local data types
 *
 * Numeric values are represented in a base-NBASE floating point format.
 * Each "digit" ranges from 0 to NBASE-1.  The type NumericDigit is signed
 * and wide enough to store a digit.  We assume that NBASE*NBASE can fit in
 * an int.  Although the purely calculational routines could handle any even
 * NBASE that's less than sqrt(INT_MAX), in practice we are only interested
 * in NBASE a power of ten, so that I/O conversions and decimal rounding
 * are easy.  Also, it's actually more efficient if NBASE is rather less than
 * sqrt(INT_MAX), so that there is "headroom" for mul_var and div_var_fast to
 * postpone processing carries.
 *
 * Values of NBASE other than 10000 are considered of historical interest only
 * and are no longer supported in any sense; no mechanism exists for the client
 * to discover the base, so every client supporting binary mode expects the
 * base-10000 format.  If you plan to change this, also note the numeric
 * abbreviation code, which assumes NBASE=10000.
 * ----------
 */

#if 0
#define NBASE 10
#define HALF_NBASE 5
#define DEC_DIGITS 1       /* decimal digits per NBASE digit */
#define MUL_GUARD_DIGITS 4 /* these are measured in NBASE digits */
#define DIV_GUARD_DIGITS 8

typedef signed char NumericDigit;
#endif

#if 0
#define NBASE 100
#define HALF_NBASE 50
#define DEC_DIGITS 2       /* decimal digits per NBASE digit */
#define MUL_GUARD_DIGITS 3 /* these are measured in NBASE digits */
#define DIV_GUARD_DIGITS 6

typedef signed char NumericDigit;
#endif

#if 1
#define NBASE 10000
#define HALF_NBASE 5000
#define DEC_DIGITS 4       /* decimal digits per NBASE digit */
#define MUL_GUARD_DIGITS 2 /* these are measured in NBASE digits */
#define DIV_GUARD_DIGITS 4

typedef int16 NumericDigit;
#endif

/*
 * The Numeric type as stored on disk.
 *
 * If the high bits of the first word of a NumericChoice (n_header, or
 * n_short.n_header, or n_long.n_sign_dscale) are NUMERIC_SHORT, then the
 * numeric follows the NumericShort format; if they are NUMERIC_POS or
 * NUMERIC_NEG, it follows the NumericLong format. If they are NUMERIC_SPECIAL,
 * the value is a NaN or Infinity.  We currently always store SPECIAL values
 * using just two bytes (i.e. only n_header), but previous releases used only
 * the NumericLong format, so we might find 4-byte NaNs (though not infinities)
 * on disk if a database has been migrated using pg_upgrade.  In either case,
 * the low-order bits of a special value's header are reserved and currently
 * should always be set to zero.
 *
 * In the NumericShort format, the remaining 14 bits of the header word
 * (n_short.n_header) are allocated as follows: 1 for sign (positive or
 * negative), 6 for dynamic scale, and 7 for weight.  In practice, most
 * commonly-encountered values can be represented this way.
 *
 * In the NumericLong format, the remaining 14 bits of the header word
 * (n_long.n_sign_dscale) represent the display scale; and the weight is
 * stored separately in n_weight.
 *
 * NOTE: by convention, values in the packed form have been stripped of
 * all leading and trailing zero digits (where a "digit" is of base NBASE).
 * In particular, if the value is zero, there will be no digits at all!
 * The weight is arbitrary in that case, but we normally set it to zero.
 */

struct NumericShort {
    uint16 n_header; /* Sign + display scale + weight */
    NumericDigit n_data[FLEXIBLE_ARRAY_MEMBER]; /* Digits */
};

struct NumericLong {
    uint16 n_sign_dscale;                       /* Sign + display scale */
    int16 n_weight;                             /* Weight of 1st digit	*/
    NumericDigit n_data[FLEXIBLE_ARRAY_MEMBER]; /* Digits */
};

union NumericChoice {
    uint16 n_header;             /* Header word */
    struct NumericLong n_long;   /* Long form (4-byte header) */
    struct NumericShort n_short; /* Short form (2-byte header) */
};

struct NumericData {
    int32 vl_len_;              /* varlena header (do not touch directly!) */
    union NumericChoice choice; /* choice of format */
};

/*
 * Interpretation of high bits.
 */

#define VARHDRSZ ((int32)sizeof(int32))

#define NUMERIC_SIGN_MASK 0xC000
#define NUMERIC_POS 0x0000
#define NUMERIC_NEG 0x4000
#define NUMERIC_SHORT 0x8000
#define NUMERIC_SPECIAL 0xC000

#define NUMERIC_FLAGBITS(n) ((n)->choice.n_header & NUMERIC_SIGN_MASK)
#define NUMERIC_IS_SHORT(n) (NUMERIC_FLAGBITS(n) == NUMERIC_SHORT)
#define NUMERIC_IS_SPECIAL(n) (NUMERIC_FLAGBITS(n) == NUMERIC_SPECIAL)

#define NUMERIC_HDRSZ (VARHDRSZ + sizeof(uint16) + sizeof(int16))
#define NUMERIC_HDRSZ_SHORT (VARHDRSZ + sizeof(uint16))

/*
 * If the flag bits are NUMERIC_SHORT or NUMERIC_SPECIAL, we want the short
 * header; otherwise, we want the long one.  Instead of testing against each
 * value, we can just look at the high bit, for a slight efficiency gain.
 */
#define NUMERIC_HEADER_IS_SHORT(n) (((n)->choice.n_header & 0x8000) != 0)
#define NUMERIC_HEADER_SIZE(n)   \
    (VARHDRSZ + sizeof(uint16) + \
     (NUMERIC_HEADER_IS_SHORT(n) ? 0 : sizeof(int16)))

/*
 * Definitions for special values (NaN, positive infinity, negative infinity).
 *
 * The two bits after the NUMERIC_SPECIAL bits are 00 for NaN, 01 for positive
 * infinity, 11 for negative infinity.  (This makes the sign bit match where
 * it is in a short-format value, though we make no use of that at present.)
 * We could mask off the remaining bits before testing the active bits, but
 * currently those bits must be zeroes, so masking would just add cycles.
 */
#define NUMERIC_EXT_SIGN_MASK 0xF000 /* high bits plus NaN/Inf flag bits */
#define NUMERIC_NAN 0xC000
#define NUMERIC_PINF 0xD000
#define NUMERIC_NINF 0xF000
#define NUMERIC_INF_SIGN_MASK 0x2000

#define NUMERIC_EXT_FLAGBITS(n) ((n)->choice.n_header & NUMERIC_EXT_SIGN_MASK)
#define NUMERIC_IS_NAN(n) ((n)->choice.n_header == NUMERIC_NAN)
#define NUMERIC_IS_PINF(n) ((n)->choice.n_header == NUMERIC_PINF)
#define NUMERIC_IS_NINF(n) ((n)->choice.n_header == NUMERIC_NINF)
#define NUMERIC_IS_INF(n) \
    (((n)->choice.n_header & ~NUMERIC_INF_SIGN_MASK) == NUMERIC_PINF)

/*
 * Short format definitions.
 */

#define NUMERIC_SHORT_SIGN_MASK 0x2000
#define NUMERIC_SHORT_DSCALE_MASK 0x1F80
#define NUMERIC_SHORT_DSCALE_SHIFT 7
#define NUMERIC_SHORT_DSCALE_MAX \
    (NUMERIC_SHORT_DSCALE_MASK >> NUMERIC_SHORT_DSCALE_SHIFT)
#define NUMERIC_SHORT_WEIGHT_SIGN_MASK 0x0040
#define NUMERIC_SHORT_WEIGHT_MASK 0x003F
#define NUMERIC_SHORT_WEIGHT_MAX NUMERIC_SHORT_WEIGHT_MASK
#define NUMERIC_SHORT_WEIGHT_MIN (-(NUMERIC_SHORT_WEIGHT_MASK + 1))

/*
 * Extract sign, display scale, weight.  These macros extract field values
 * suitable for the NumericVar format from the Numeric (on-disk) format.
 *
 * Note that we don't trouble to ensure that dscale and weight read as zero
 * for an infinity; however, that doesn't matter since we never convert
 * "special" numerics to NumericVar form.  Only the constants defined below
 * (const_nan, etc) ever represent a non-finite value as a NumericVar.
 */

#define NUMERIC_DSCALE_MASK 0x3FFF
#define NUMERIC_DSCALE_MAX NUMERIC_DSCALE_MASK

#define NUMERIC_SIGN(n)                                              \
    (NUMERIC_IS_SHORT(n)                                             \
         ? (((n)->choice.n_short.n_header & NUMERIC_SHORT_SIGN_MASK) \
                ? NUMERIC_NEG                                        \
                : NUMERIC_POS)                                       \
         : (NUMERIC_IS_SPECIAL(n) ? NUMERIC_EXT_FLAGBITS(n)          \
                                  : NUMERIC_FLAGBITS(n)))
#define NUMERIC_DSCALE(n)                                                \
    (NUMERIC_HEADER_IS_SHORT((n))                                        \
         ? ((n)->choice.n_short.n_header & NUMERIC_SHORT_DSCALE_MASK) >> \
               NUMERIC_SHORT_DSCALE_SHIFT                                \
         : ((n)->choice.n_long.n_sign_dscale & NUMERIC_DSCALE_MASK))
#define NUMERIC_WEIGHT(n)                                                  \
    (NUMERIC_HEADER_IS_SHORT((n))                                          \
         ? (((n)->choice.n_short.n_header & NUMERIC_SHORT_WEIGHT_SIGN_MASK \
                 ? ~NUMERIC_SHORT_WEIGHT_MASK                              \
                 : 0) |                                                    \
            ((n)->choice.n_short.n_header & NUMERIC_SHORT_WEIGHT_MASK))    \
         : ((n)->choice.n_long.n_weight))

/* ----------
 * NumericVar is the format we use for arithmetic.  The digit-array part
 * is the same as the NumericData storage format, but the header is more
 * complex.
 *
 * The value represented by a NumericVar is determined by the sign, weight,
 * ndigits, and digits[] array.  If it is a "special" value (NaN or Inf)
 * then only the sign field matters; ndigits should be zero, and the weight
 * and dscale fields are ignored.
 *
 * Note: the first digit of a NumericVar's value is assumed to be multiplied
 * by NBASE ** weight.  Another way to say it is that there are weight+1
 * digits before the decimal point.  It is possible to have weight < 0.
 *
 * buf points at the physical start of the palloc'd digit buffer for the
 * NumericVar.  digits points at the first digit in actual use (the one
 * with the specified weight).  We normally leave an unused digit or two
 * (preset to zeroes) between buf and digits, so that there is room to store
 * a carry out of the top digit without reallocating space.  We just need to
 * decrement digits (and increment weight) to make room for the carry digit.
 * (There is no such extra space in a numeric value stored in the database,
 * only in a NumericVar in memory.)
 *
 * If buf is NULL then the digit buffer isn't actually palloc'd and should
 * not be freed --- see the constants below for an example.
 *
 * dscale, or display scale, is the nominal precision expressed as number
 * of digits after the decimal point (it must always be >= 0 at present).
 * dscale may be more than the number of physically stored fractional digits,
 * implying that we have suppressed storage of significant trailing zeroes.
 * It should never be less than the number of stored digits, since that would
 * imply hiding digits that are present.  NOTE that dscale is always expressed
 * in *decimal* digits, and so it may correspond to a fractional number of
 * base-NBASE digits --- divide by DEC_DIGITS to convert to NBASE digits.
 *
 * rscale, or result scale, is the target precision for a computation.
 * Like dscale it is expressed as number of *decimal* digits after the decimal
 * point, and is always >= 0 at present.
 * Note that rscale is not stored in variables --- it's figured on-the-fly
 * from the dscales of the inputs.
 *
 * While we consistently use "weight" to refer to the base-NBASE weight of
 * a numeric value, it is convenient in some scale-related calculations to
 * make use of the base-10 weight (ie, the approximate log10 of the value).
 * To avoid confusion, such a decimal-units weight is called a "dweight".
 *
 * NB: All the variable-level functions are written in a style that makes it
 * possible to give one and the same variable as argument and destination.
 * This is feasible because the digit buffer is separate from the variable.
 * ----------
 */
typedef struct NumericVar {
    int ndigits;          /* # of digits in digits[] - can be 0! */
    int weight;           /* weight of first digit */
    int sign;             /* NUMERIC_POS, _NEG, _NAN, _PINF, or _NINF */
    int dscale;           /* display scale */
    NumericDigit *buf;    /* start of palloc'd space for digits[] */
    NumericDigit *digits; /* base-NBASE digits */
} NumericVar;

/* ----------
 * Data for generate_series
 * ----------
 */
typedef struct {
    NumericVar current;
    NumericVar stop;
    NumericVar step;
} generate_series_numeric_fctx;

/*
 * We define our own macros for packing and unpacking abbreviated-key
 * representations for numeric values in order to avoid depending on
 * USE_FLOAT8_BYVAL.  The type of abbreviation we use is based only on
 * the size of a datum, not the argument-passing convention for float8.
 *
 * The range of abbreviations for finite values is from +PG_INT64/32_MAX
 * to -PG_INT64/32_MAX.  NaN has the abbreviation PG_INT64/32_MIN, and we
 * define the sort ordering to make that work out properly (see further
 * comments below).  PINF and NINF share the abbreviations of the largest
 * and smallest finite abbreviation classes.
 */
#define NUMERIC_ABBREV_BITS (SIZEOF_DATUM * BITS_PER_BYTE)
#if SIZEOF_DATUM == 8
#define NumericAbbrevGetDatum(X) ((Datum)(X))
#define DatumGetNumericAbbrev(X) ((int64)(X))
#define NUMERIC_ABBREV_NAN NumericAbbrevGetDatum(PG_INT64_MIN)
#define NUMERIC_ABBREV_PINF NumericAbbrevGetDatum(-PG_INT64_MAX)
#define NUMERIC_ABBREV_NINF NumericAbbrevGetDatum(PG_INT64_MAX)
#else
#define NumericAbbrevGetDatum(X) ((Datum)(X))
#define DatumGetNumericAbbrev(X) ((int32)(X))
#define NUMERIC_ABBREV_NAN NumericAbbrevGetDatum(PG_INT32_MIN)
#define NUMERIC_ABBREV_PINF NumericAbbrevGetDatum(-PG_INT32_MAX)
#define NUMERIC_ABBREV_NINF NumericAbbrevGetDatum(PG_INT32_MAX)
#endif

/* ----------
 * Some preinitialized constants
 * ----------
 */
static const NumericDigit const_zero_data[1] = {0};
static const NumericVar const_zero = {0, 0,    NUMERIC_POS,
                                      0, NULL, (NumericDigit *)const_zero_data};

static const NumericDigit const_one_data[1] = {1};
static const NumericVar const_one = {1, 0,    NUMERIC_POS,
                                     0, NULL, (NumericDigit *)const_one_data};

static const NumericVar const_minus_one = {
    1, 0, NUMERIC_NEG, 0, NULL, (NumericDigit *)const_one_data};

static const NumericDigit const_two_data[1] = {2};
static const NumericVar const_two = {1, 0,    NUMERIC_POS,
                                     0, NULL, (NumericDigit *)const_two_data};

#if DEC_DIGITS == 4
static const NumericDigit const_zero_point_nine_data[1] = {9000};
#elif DEC_DIGITS == 2
static const NumericDigit const_zero_point_nine_data[1] = {90};
#elif DEC_DIGITS == 1
static const NumericDigit const_zero_point_nine_data[1] = {9};
#endif
static const NumericVar const_zero_point_nine = {
    1, -1, NUMERIC_POS, 1, NULL, (NumericDigit *)const_zero_point_nine_data};

#if DEC_DIGITS == 4
static const NumericDigit const_one_point_one_data[2] = {1, 1000};
#elif DEC_DIGITS == 2
static const NumericDigit const_one_point_one_data[2] = {1, 10};
#elif DEC_DIGITS == 1
static const NumericDigit const_one_point_one_data[2] = {1, 1};
#endif
static const NumericVar const_one_point_one = {
    2, 0, NUMERIC_POS, 1, NULL, (NumericDigit *)const_one_point_one_data};

static const NumericVar const_nan = {0, 0, NUMERIC_NAN, 0, NULL, NULL};

static const NumericVar const_pinf = {0, 0, NUMERIC_PINF, 0, NULL, NULL};

static const NumericVar const_ninf = {0, 0, NUMERIC_NINF, 0, NULL, NULL};

#if DEC_DIGITS == 4
static const int round_powers[4] = {0, 1000, 100, 10};
#endif

#define digitbuf_alloc(ndigits) \
    ((NumericDigit *)palloc((ndigits) * sizeof(NumericDigit)))
#define digitbuf_free(buf)             \
    do {                               \
        if ((buf) != NULL) pfree(buf); \
    } while (0)

#define init_var(v) memset(v, 0, sizeof(NumericVar))

#define NUMERIC_DIGITS(num)                                      \
    (NUMERIC_HEADER_IS_SHORT(num) ? (num)->choice.n_short.n_data \
                                  : (num)->choice.n_long.n_data)
#define NUMERIC_NDIGITS(num) \
    ((VARSIZE(num) - NUMERIC_HEADER_SIZE(num)) / sizeof(NumericDigit))
#define NUMERIC_CAN_BE_SHORT(scale, weight)  \
    ((scale) <= NUMERIC_SHORT_DSCALE_MAX &&  \
     (weight) <= NUMERIC_SHORT_WEIGHT_MAX && \
     (weight) >= NUMERIC_SHORT_WEIGHT_MIN)

static Numeric make_result_opt_error(const NumericVar *var, bool *have_error);

static void alloc_var(NumericVar *var, int ndigits) {
    digitbuf_free(var->buf);
    var->buf = digitbuf_alloc(ndigits + 1);
    var->buf[0] = 0; /* spare digit for rounding */
    var->digits = var->buf + 1;
    var->ndigits = ndigits;
}

static void int64_to_numericvar(int64 val, NumericVar *var) {
    uint64 uval, newuval;
    NumericDigit *ptr;
    int ndigits;

    /* int64 can require at most 19 decimal digits; add one for safety */
    alloc_var(var, 20 / DEC_DIGITS);
    if (val < 0) {
        var->sign = NUMERIC_NEG;
        uval = -val;
    } else {
        var->sign = NUMERIC_POS;
        uval = val;
    }
    var->dscale = 0;
    if (val == 0) {
        var->ndigits = 0;
        var->weight = 0;
        return;
    }
    ptr = var->digits + var->ndigits;
    ndigits = 0;
    do {
        ptr--;
        ndigits++;
        newuval = uval / NBASE;
        *ptr = uval - newuval * NBASE;
        uval = newuval;
    } while (uval);
    var->digits = ptr;
    var->ndigits = ndigits;
    var->weight = ndigits - 1;
}

void dump_numeric(const char *str, Numeric num) {
    NumericDigit *digits = NUMERIC_DIGITS(num);
    int ndigits;
    int i;

    ndigits = NUMERIC_NDIGITS(num);

    printf("%s: NUMERIC w=%d d=%d ", str, NUMERIC_WEIGHT(num),
           NUMERIC_DSCALE(num));
    switch (NUMERIC_SIGN(num)) {
        case NUMERIC_POS:
            printf("POS");
            break;
        case NUMERIC_NEG:
            printf("NEG");
            break;
        case NUMERIC_NAN:
            printf("NaN");
            break;
        case NUMERIC_PINF:
            printf("Infinity");
            break;
        case NUMERIC_NINF:
            printf("-Infinity");
            break;
        default:
            printf("SIGN=0x%x", NUMERIC_SIGN(num));
            break;
    }

    for (i = 0; i < ndigits; i++) printf(" %0*d", DEC_DIGITS, digits[i]);
    printf("\n");
}

static Numeric make_result(const NumericVar *var) {
    return make_result_opt_error(var, NULL);
}

static Numeric make_result_opt_error(const NumericVar *var, bool *have_error) {
    Numeric result;
    NumericDigit *digits = var->digits;
    int weight = var->weight;
    int sign = var->sign;
    int n;
    Size len;

    if (have_error) *have_error = false;

    if ((sign & NUMERIC_SIGN_MASK) == NUMERIC_SPECIAL) {
        /*
         * Verify valid special value.  This could be just an Assert, perhaps,
         * but it seems worthwhile to expend a few cycles to ensure that we
         * never write any nonzero reserved bits to disk.
         */
        if (!(sign == NUMERIC_NAN || sign == NUMERIC_PINF ||
              sign == NUMERIC_NINF))
            printf("invalid numeric sign value 0x%x\n", sign);

        result = (Numeric)palloc(NUMERIC_HDRSZ_SHORT);

        SET_VARSIZE(result, NUMERIC_HDRSZ_SHORT);
        result->choice.n_header = sign;
        /* the header word is all we need */

        dump_numeric("make_result()", result);
        return result;
    }

    n = var->ndigits;

    /* truncate leading zeroes */
    while (n > 0 && *digits == 0) {
        digits++;
        weight--;
        n--;
    }
    /* truncate trailing zeroes */
    while (n > 0 && digits[n - 1] == 0) n--;

    /* If zero result, force to weight=0 and positive sign */
    if (n == 0) {
        weight = 0;
        sign = NUMERIC_POS;
    }

    /* Build the result */
    if (NUMERIC_CAN_BE_SHORT(var->dscale, weight)) {
        len = NUMERIC_HDRSZ_SHORT + n * sizeof(NumericDigit);
        result = (Numeric)palloc(len);
        SET_VARSIZE(result, len);
        result->choice.n_short.n_header =
            (sign == NUMERIC_NEG ? (NUMERIC_SHORT | NUMERIC_SHORT_SIGN_MASK)
                                 : NUMERIC_SHORT) |
            (var->dscale << NUMERIC_SHORT_DSCALE_SHIFT) |
            (weight < 0 ? NUMERIC_SHORT_WEIGHT_SIGN_MASK : 0) |
            (weight & NUMERIC_SHORT_WEIGHT_MASK);
    } else {
        len = NUMERIC_HDRSZ + n * sizeof(NumericDigit);
        result = (Numeric)palloc(len);
        SET_VARSIZE(result, len);
        result->choice.n_long.n_sign_dscale =
            sign | (var->dscale & NUMERIC_DSCALE_MASK);
        result->choice.n_long.n_weight = weight;
    }

    Assert(NUMERIC_NDIGITS(result) == n);
    if (n > 0) memcpy(NUMERIC_DIGITS(result), digits, n * sizeof(NumericDigit));

    /* Check for overflow of int16 fields */
    if (NUMERIC_WEIGHT(result) != weight ||
        NUMERIC_DSCALE(result) != var->dscale) {
        if (have_error) {
            *have_error = true;
            return NULL;
        } else {
            printf("value overflows numeric format\n");
        }
    }

    return result;
}

#define init_var(v) memset(v, 0, sizeof(NumericVar))

static void free_var(NumericVar *var) {
    digitbuf_free(var->buf);
    var->buf = NULL;
    var->digits = NULL;
    var->sign = NUMERIC_NAN;
}

Numeric int64_to_numeric(int64 val) {
    Numeric res;
    NumericVar result;

    init_var(&result);

    int64_to_numericvar(val, &result);

    res = make_result(&result);

    free_var(&result);

    return res;
}

static void strip_var(NumericVar *var) {
    NumericDigit *digits = var->digits;
    int ndigits = var->ndigits;

    /* Strip leading zeroes */
    while (ndigits > 0 && *digits == 0) {
        digits++;
        var->weight--;
        ndigits--;
    }

    /* Strip trailing zeroes */
    while (ndigits > 0 && digits[ndigits - 1] == 0) ndigits--;

    /* If it's zero, normalize the sign and weight */
    if (ndigits == 0) {
        var->sign = NUMERIC_POS;
        var->weight = 0;
    }

    var->digits = digits;
    var->ndigits = ndigits;
}

static void round_var(NumericVar *var, int rscale) {
    NumericDigit *digits = var->digits;
    int di;
    int ndigits;
    int carry;

    var->dscale = rscale;

    /* decimal digits wanted */
    di = (var->weight + 1) * DEC_DIGITS + rscale;

    /*
     * If di = 0, the value loses all digits, but could round up to 1 if its
     * first extra digit is >= 5.  If di < 0 the result must be 0.
     */
    if (di < 0) {
        var->ndigits = 0;
        var->weight = 0;
        var->sign = NUMERIC_POS;
    } else {
        /* NBASE digits wanted */
        ndigits = (di + DEC_DIGITS - 1) / DEC_DIGITS;

        /* 0, or number of decimal digits to keep in last NBASE digit */
        di %= DEC_DIGITS;

        if (ndigits < var->ndigits || (ndigits == var->ndigits && di > 0)) {
            var->ndigits = ndigits;

#if DEC_DIGITS == 1
            /* di must be zero */
            carry = (digits[ndigits] >= HALF_NBASE) ? 1 : 0;
#else
            if (di == 0)
                carry = (digits[ndigits] >= HALF_NBASE) ? 1 : 0;
            else {
                /* Must round within last NBASE digit */
                int extra, pow10;

#if DEC_DIGITS == 4
                pow10 = round_powers[di];
#elif DEC_DIGITS == 2
                pow10 = 10;
#else
#error unsupported NBASE
#endif
                extra = digits[--ndigits] % pow10;
                digits[ndigits] -= extra;
                carry = 0;
                if (extra >= pow10 / 2) {
                    pow10 += digits[ndigits];
                    if (pow10 >= NBASE) {
                        pow10 -= NBASE;
                        carry = 1;
                    }
                    digits[ndigits] = pow10;
                }
            }
#endif

            /* Propagate carry if needed */
            while (carry) {
                carry += digits[--ndigits];
                if (carry >= NBASE) {
                    digits[ndigits] = carry - NBASE;
                    carry = 1;
                } else {
                    digits[ndigits] = carry;
                    carry = 0;
                }
            }

            if (ndigits < 0) {
                Assert(ndigits == -1); /* better not have added > 1 digit */
                Assert(var->digits > var->buf);
                var->digits--;
                var->ndigits++;
                var->weight++;
            }
        }
    }
}

static void trunc_var(NumericVar *var, int rscale) {
    int di;
    int ndigits;

    var->dscale = rscale;

    /* decimal digits wanted */
    di = (var->weight + 1) * DEC_DIGITS + rscale;

    /*
     * If di <= 0, the value loses all digits.
     */
    if (di <= 0) {
        var->ndigits = 0;
        var->weight = 0;
        var->sign = NUMERIC_POS;
    } else {
        /* NBASE digits wanted */
        ndigits = (di + DEC_DIGITS - 1) / DEC_DIGITS;

        if (ndigits <= var->ndigits) {
            var->ndigits = ndigits;

#if DEC_DIGITS == 1
            /* no within-digit stuff to worry about */
#else
            /* 0, or number of decimal digits to keep in last NBASE digit */
            di %= DEC_DIGITS;

            if (di > 0) {
                /* Must truncate within last NBASE digit */
                NumericDigit *digits = var->digits;
                int extra, pow10;

#if DEC_DIGITS == 4
                pow10 = round_powers[di];
#elif DEC_DIGITS == 2
                pow10 = 10;
#else
#error unsupported NBASE
#endif
                extra = digits[--ndigits] % pow10;
                digits[ndigits] -= extra;
            }
#endif
        }
    }
}

static int numeric_sign_internal(Numeric num) {
    if (NUMERIC_IS_SPECIAL(num)) {
        Assert(!NUMERIC_IS_NAN(num));
        /* Must be Inf or -Inf */
        if (NUMERIC_IS_PINF(num))
            return 1;
        else
            return -1;
    }

    /*
     * The packed format is known to be totally zero digit trimmed always. So
     * once we've eliminated specials, we can identify a zero by the fact that
     * there are no digits at all.
     */
    else if (NUMERIC_NDIGITS(num) == 0)
        return 0;
    else if (NUMERIC_SIGN(num) == NUMERIC_NEG)
        return -1;
    else
        return 1;
}

static void init_var_from_num(Numeric num, NumericVar *dest) {
    dest->ndigits = NUMERIC_NDIGITS(num);
    dest->weight = NUMERIC_WEIGHT(num);
    dest->sign = NUMERIC_SIGN(num);
    dest->dscale = NUMERIC_DSCALE(num);
    dest->digits = NUMERIC_DIGITS(num);
    dest->buf = NULL; /* digits array is not palloc'd */
}

static void add_abs(const NumericVar *var1, const NumericVar *var2,
                    NumericVar *result) {
    NumericDigit *res_buf;
    NumericDigit *res_digits;
    int res_ndigits;
    int res_weight;
    int res_rscale, rscale1, rscale2;
    int res_dscale;
    int i, i1, i2;
    int carry = 0;

    /* copy these values into local vars for speed in inner loop */
    int var1ndigits = var1->ndigits;
    int var2ndigits = var2->ndigits;
    NumericDigit *var1digits = var1->digits;
    NumericDigit *var2digits = var2->digits;

    res_weight = Max(var1->weight, var2->weight) + 1;

    res_dscale = Max(var1->dscale, var2->dscale);

    /* Note: here we are figuring rscale in base-NBASE digits */
    rscale1 = var1->ndigits - var1->weight - 1;
    rscale2 = var2->ndigits - var2->weight - 1;
    res_rscale = Max(rscale1, rscale2);

    res_ndigits = res_rscale + res_weight + 1;
    if (res_ndigits <= 0) res_ndigits = 1;

    res_buf = digitbuf_alloc(res_ndigits + 1);
    res_buf[0] = 0; /* spare digit for later rounding */
    res_digits = res_buf + 1;

    i1 = res_rscale + var1->weight + 1;
    i2 = res_rscale + var2->weight + 1;
    for (i = res_ndigits - 1; i >= 0; i--) {
        i1--;
        i2--;
        if (i1 >= 0 && i1 < var1ndigits) carry += var1digits[i1];
        if (i2 >= 0 && i2 < var2ndigits) carry += var2digits[i2];

        if (carry >= NBASE) {
            res_digits[i] = carry - NBASE;
            carry = 1;
        } else {
            res_digits[i] = carry;
            carry = 0;
        }
    }

    Assert(carry == 0); /* else we failed to allow for carry out */

    digitbuf_free(result->buf);
    result->ndigits = res_ndigits;
    result->buf = res_buf;
    result->digits = res_digits;
    result->weight = res_weight;
    result->dscale = res_dscale;

    /* Remove leading/trailing zeroes */
    strip_var(result);
}

static int cmp_abs_common(const NumericDigit *var1digits, int var1ndigits,
                          int var1weight, const NumericDigit *var2digits,
                          int var2ndigits, int var2weight) {
    int i1 = 0;
    int i2 = 0;

    /* Check any digits before the first common digit */

    while (var1weight > var2weight && i1 < var1ndigits) {
        if (var1digits[i1++] != 0) return 1;
        var1weight--;
    }
    while (var2weight > var1weight && i2 < var2ndigits) {
        if (var2digits[i2++] != 0) return -1;
        var2weight--;
    }

    /* At this point, either w1 == w2 or we've run out of digits */

    if (var1weight == var2weight) {
        while (i1 < var1ndigits && i2 < var2ndigits) {
            int stat = var1digits[i1++] - var2digits[i2++];

            if (stat) {
                if (stat > 0) return 1;
                return -1;
            }
        }
    }

    /*
     * At this point, we've run out of digits on one side or the other; so any
     * remaining nonzero digits imply that side is larger
     */
    while (i1 < var1ndigits) {
        if (var1digits[i1++] != 0) return 1;
    }
    while (i2 < var2ndigits) {
        if (var2digits[i2++] != 0) return -1;
    }

    return 0;
}

static int cmp_abs(const NumericVar *var1, const NumericVar *var2) {
    return cmp_abs_common(var1->digits, var1->ndigits, var1->weight,
                          var2->digits, var2->ndigits, var2->weight);
}

static void zero_var(NumericVar *var) {
    digitbuf_free(var->buf);
    var->buf = NULL;
    var->digits = NULL;
    var->ndigits = 0;
    var->weight = 0;         /* by convention; doesn't really matter */
    var->sign = NUMERIC_POS; /* anything but NAN... */
}

static void sub_abs(const NumericVar *var1, const NumericVar *var2,
                    NumericVar *result) {
    NumericDigit *res_buf;
    NumericDigit *res_digits;
    int res_ndigits;
    int res_weight;
    int res_rscale, rscale1, rscale2;
    int res_dscale;
    int i, i1, i2;
    int borrow = 0;

    /* copy these values into local vars for speed in inner loop */
    int var1ndigits = var1->ndigits;
    int var2ndigits = var2->ndigits;
    NumericDigit *var1digits = var1->digits;
    NumericDigit *var2digits = var2->digits;

    res_weight = var1->weight;

    res_dscale = Max(var1->dscale, var2->dscale);

    /* Note: here we are figuring rscale in base-NBASE digits */
    rscale1 = var1->ndigits - var1->weight - 1;
    rscale2 = var2->ndigits - var2->weight - 1;
    res_rscale = Max(rscale1, rscale2);

    res_ndigits = res_rscale + res_weight + 1;
    if (res_ndigits <= 0) res_ndigits = 1;

    res_buf = digitbuf_alloc(res_ndigits + 1);
    res_buf[0] = 0; /* spare digit for later rounding */
    res_digits = res_buf + 1;

    i1 = res_rscale + var1->weight + 1;
    i2 = res_rscale + var2->weight + 1;
    for (i = res_ndigits - 1; i >= 0; i--) {
        i1--;
        i2--;
        if (i1 >= 0 && i1 < var1ndigits) borrow += var1digits[i1];
        if (i2 >= 0 && i2 < var2ndigits) borrow -= var2digits[i2];

        if (borrow < 0) {
            res_digits[i] = borrow + NBASE;
            borrow = -1;
        } else {
            res_digits[i] = borrow;
            borrow = 0;
        }
    }

    Assert(borrow == 0); /* else caller gave us var1 < var2 */

    digitbuf_free(result->buf);
    result->ndigits = res_ndigits;
    result->buf = res_buf;
    result->digits = res_digits;
    result->weight = res_weight;
    result->dscale = res_dscale;

    /* Remove leading/trailing zeroes */
    strip_var(result);
}

static void add_var(const NumericVar *var1, const NumericVar *var2,
                    NumericVar *result) {
    /*
     * Decide on the signs of the two variables what to do
     */
    if (var1->sign == NUMERIC_POS) {
        if (var2->sign == NUMERIC_POS) {
            /*
             * Both are positive result = +(ABS(var1) + ABS(var2))
             */
            add_abs(var1, var2, result);
            result->sign = NUMERIC_POS;
        } else {
            /*
             * var1 is positive, var2 is negative Must compare absolute values
             */
            switch (cmp_abs(var1, var2)) {
                case 0:
                    /* ----------
                     * ABS(var1) == ABS(var2)
                     * result = ZERO
                     * ----------
                     */
                    zero_var(result);
                    result->dscale = Max(var1->dscale, var2->dscale);
                    break;

                case 1:
                    /* ----------
                     * ABS(var1) > ABS(var2)
                     * result = +(ABS(var1) - ABS(var2))
                     * ----------
                     */
                    sub_abs(var1, var2, result);
                    result->sign = NUMERIC_POS;
                    break;

                case -1:
                    /* ----------
                     * ABS(var1) < ABS(var2)
                     * result = -(ABS(var2) - ABS(var1))
                     * ----------
                     */
                    sub_abs(var2, var1, result);
                    result->sign = NUMERIC_NEG;
                    break;
            }
        }
    } else {
        if (var2->sign == NUMERIC_POS) {
            /* ----------
             * var1 is negative, var2 is positive
             * Must compare absolute values
             * ----------
             */
            switch (cmp_abs(var1, var2)) {
                case 0:
                    /* ----------
                     * ABS(var1) == ABS(var2)
                     * result = ZERO
                     * ----------
                     */
                    zero_var(result);
                    result->dscale = Max(var1->dscale, var2->dscale);
                    break;

                case 1:
                    /* ----------
                     * ABS(var1) > ABS(var2)
                     * result = -(ABS(var1) - ABS(var2))
                     * ----------
                     */
                    sub_abs(var1, var2, result);
                    result->sign = NUMERIC_NEG;
                    break;

                case -1:
                    /* ----------
                     * ABS(var1) < ABS(var2)
                     * result = +(ABS(var2) - ABS(var1))
                     * ----------
                     */
                    sub_abs(var2, var1, result);
                    result->sign = NUMERIC_POS;
                    break;
            }
        } else {
            /* ----------
             * Both are negative
             * result = -(ABS(var1) + ABS(var2))
             * ----------
             */
            add_abs(var1, var2, result);
            result->sign = NUMERIC_NEG;
        }
    }
}

/*
 * numeric_add_opt_error() -
 *
 *	Internal version of numeric_add().  If "*have_error" flag is provided,
 *	on error it's set to true, NULL returned.  This is helpful when caller
 *	need to handle errors by itself.
 */
Numeric numeric_add_opt_error(Numeric num1, Numeric num2, bool *have_error) {
    NumericVar arg1;
    NumericVar arg2;
    NumericVar result;
    Numeric res;

    /*
     * Handle NaN and infinities
     */
    if (NUMERIC_IS_SPECIAL(num1) || NUMERIC_IS_SPECIAL(num2)) {
        if (NUMERIC_IS_NAN(num1) || NUMERIC_IS_NAN(num2))
            return make_result(&const_nan);
        if (NUMERIC_IS_PINF(num1)) {
            if (NUMERIC_IS_NINF(num2))
                return make_result(&const_nan); /* Inf + -Inf */
            else
                return make_result(&const_pinf);
        }
        if (NUMERIC_IS_NINF(num1)) {
            if (NUMERIC_IS_PINF(num2))
                return make_result(&const_nan); /* -Inf + Inf */
            else
                return make_result(&const_ninf);
        }
        /* by here, num1 must be finite, so num2 is not */
        if (NUMERIC_IS_PINF(num2)) return make_result(&const_pinf);
        Assert(NUMERIC_IS_NINF(num2));
        return make_result(&const_ninf);
    }

    /*
     * Unpack the values, let add_var() compute the result and return it.
     */
    init_var_from_num(num1, &arg1);
    init_var_from_num(num2, &arg2);

    init_var(&result);
    add_var(&arg1, &arg2, &result);

    res = make_result_opt_error(&result, have_error);

    free_var(&result);

    return res;
}

/*
 * sub_var() -
 *
 *	Full version of sub functionality on variable level (handling signs).
 *	result might point to one of the operands too without danger.
 */
static void sub_var(const NumericVar *var1, const NumericVar *var2,
                    NumericVar *result) {
    /*
     * Decide on the signs of the two variables what to do
     */
    if (var1->sign == NUMERIC_POS) {
        if (var2->sign == NUMERIC_NEG) {
            /* ----------
             * var1 is positive, var2 is negative
             * result = +(ABS(var1) + ABS(var2))
             * ----------
             */
            add_abs(var1, var2, result);
            result->sign = NUMERIC_POS;
        } else {
            /* ----------
             * Both are positive
             * Must compare absolute values
             * ----------
             */
            switch (cmp_abs(var1, var2)) {
                case 0:
                    /* ----------
                     * ABS(var1) == ABS(var2)
                     * result = ZERO
                     * ----------
                     */
                    zero_var(result);
                    result->dscale = Max(var1->dscale, var2->dscale);
                    break;

                case 1:
                    /* ----------
                     * ABS(var1) > ABS(var2)
                     * result = +(ABS(var1) - ABS(var2))
                     * ----------
                     */
                    sub_abs(var1, var2, result);
                    result->sign = NUMERIC_POS;
                    break;

                case -1:
                    /* ----------
                     * ABS(var1) < ABS(var2)
                     * result = -(ABS(var2) - ABS(var1))
                     * ----------
                     */
                    sub_abs(var2, var1, result);
                    result->sign = NUMERIC_NEG;
                    break;
            }
        }
    } else {
        if (var2->sign == NUMERIC_NEG) {
            /* ----------
             * Both are negative
             * Must compare absolute values
             * ----------
             */
            switch (cmp_abs(var1, var2)) {
                case 0:
                    /* ----------
                     * ABS(var1) == ABS(var2)
                     * result = ZERO
                     * ----------
                     */
                    zero_var(result);
                    result->dscale = Max(var1->dscale, var2->dscale);
                    break;

                case 1:
                    /* ----------
                     * ABS(var1) > ABS(var2)
                     * result = -(ABS(var1) - ABS(var2))
                     * ----------
                     */
                    sub_abs(var1, var2, result);
                    result->sign = NUMERIC_NEG;
                    break;

                case -1:
                    /* ----------
                     * ABS(var1) < ABS(var2)
                     * result = +(ABS(var2) - ABS(var1))
                     * ----------
                     */
                    sub_abs(var2, var1, result);
                    result->sign = NUMERIC_POS;
                    break;
            }
        } else {
            /* ----------
             * var1 is negative, var2 is positive
             * result = -(ABS(var1) + ABS(var2))
             * ----------
             */
            add_abs(var1, var2, result);
            result->sign = NUMERIC_NEG;
        }
    }
}

/*
 * numeric_sub_opt_error() -
 *
 *	Internal version of numeric_sub().  If "*have_error" flag is provided,
 *	on error it's set to true, NULL returned.  This is helpful when caller
 *	need to handle errors by itself.
 */
Numeric numeric_sub_opt_error(Numeric num1, Numeric num2, bool *have_error) {
    NumericVar arg1;
    NumericVar arg2;
    NumericVar result;
    Numeric res;

    /*
     * Handle NaN and infinities
     */
    if (NUMERIC_IS_SPECIAL(num1) || NUMERIC_IS_SPECIAL(num2)) {
        if (NUMERIC_IS_NAN(num1) || NUMERIC_IS_NAN(num2))
            return make_result(&const_nan);
        if (NUMERIC_IS_PINF(num1)) {
            if (NUMERIC_IS_PINF(num2))
                return make_result(&const_nan); /* Inf - Inf */
            else
                return make_result(&const_pinf);
        }
        if (NUMERIC_IS_NINF(num1)) {
            if (NUMERIC_IS_NINF(num2))
                return make_result(&const_nan); /* -Inf - -Inf */
            else
                return make_result(&const_ninf);
        }
        /* by here, num1 must be finite, so num2 is not */
        if (NUMERIC_IS_PINF(num2)) return make_result(&const_ninf);
        Assert(NUMERIC_IS_NINF(num2));
        return make_result(&const_pinf);
    }

    /*
     * Unpack the values, let sub_var() compute the result and return it.
     */
    init_var_from_num(num1, &arg1);
    init_var_from_num(num2, &arg2);

    init_var(&result);
    sub_var(&arg1, &arg2, &result);

    res = make_result_opt_error(&result, have_error);

    free_var(&result);

    return res;
}

static void mul_var(const NumericVar *var1, const NumericVar *var2,
                    NumericVar *result, int rscale) {
    int res_ndigits;
    int res_sign;
    int res_weight;
    int maxdigits;
    int *dig;
    int carry;
    int maxdig;
    int newdig;
    int var1ndigits;
    int var2ndigits;
    NumericDigit *var1digits;
    NumericDigit *var2digits;
    NumericDigit *res_digits;
    int i, i1, i2;

    /*
     * Arrange for var1 to be the shorter of the two numbers.  This improves
     * performance because the inner multiplication loop is much simpler than
     * the outer loop, so it's better to have a smaller number of iterations
     * of the outer loop.  This also reduces the number of times that the
     * accumulator array needs to be normalized.
     */
    if (var1->ndigits > var2->ndigits) {
        const NumericVar *tmp = var1;

        var1 = var2;
        var2 = tmp;
    }

    /* copy these values into local vars for speed in inner loop */
    var1ndigits = var1->ndigits;
    var2ndigits = var2->ndigits;
    var1digits = var1->digits;
    var2digits = var2->digits;

    if (var1ndigits == 0 || var2ndigits == 0) {
        /* one or both inputs is zero; so is result */
        zero_var(result);
        result->dscale = rscale;
        return;
    }

    /* Determine result sign and (maximum possible) weight */
    if (var1->sign == var2->sign)
        res_sign = NUMERIC_POS;
    else
        res_sign = NUMERIC_NEG;
    res_weight = var1->weight + var2->weight + 2;

    /*
     * Determine the number of result digits to compute.  If the exact result
     * would have more than rscale fractional digits, truncate the computation
     * with MUL_GUARD_DIGITS guard digits, i.e., ignore input digits that
     * would only contribute to the right of that.  (This will give the exact
     * rounded-to-rscale answer unless carries out of the ignored positions
     * would have propagated through more than MUL_GUARD_DIGITS digits.)
     *
     * Note: an exact computation could not produce more than var1ndigits +
     * var2ndigits digits, but we allocate one extra output digit in case
     * rscale-driven rounding produces a carry out of the highest exact digit.
     */
    res_ndigits = var1ndigits + var2ndigits + 1;
    maxdigits = res_weight + 1 + (rscale + DEC_DIGITS - 1) / DEC_DIGITS +
                MUL_GUARD_DIGITS;
    res_ndigits = Min(res_ndigits, maxdigits);

    if (res_ndigits < 3) {
        /* All input digits will be ignored; so result is zero */
        zero_var(result);
        result->dscale = rscale;
        return;
    }

    /*
     * We do the arithmetic in an array "dig[]" of signed int's.  Since
     * INT_MAX is noticeably larger than NBASE*NBASE, this gives us headroom
     * to avoid normalizing carries immediately.
     *
     * maxdig tracks the maximum possible value of any dig[] entry; when this
     * threatens to exceed INT_MAX, we take the time to propagate carries.
     * Furthermore, we need to ensure that overflow doesn't occur during the
     * carry propagation passes either.  The carry values could be as much as
     * INT_MAX/NBASE, so really we must normalize when digits threaten to
     * exceed INT_MAX - INT_MAX/NBASE.
     *
     * To avoid overflow in maxdig itself, it actually represents the max
     * possible value divided by NBASE-1, ie, at the top of the loop it is
     * known that no dig[] entry exceeds maxdig * (NBASE-1).
     */
    dig = (int *)calloc(res_ndigits, sizeof(int));
    maxdig = 0;

    /*
     * The least significant digits of var1 should be ignored if they don't
     * contribute directly to the first res_ndigits digits of the result that
     * we are computing.
     *
     * Digit i1 of var1 and digit i2 of var2 are multiplied and added to digit
     * i1+i2+2 of the accumulator array, so we need only consider digits of
     * var1 for which i1 <= res_ndigits - 3.
     */
    for (i1 = Min(var1ndigits - 1, res_ndigits - 3); i1 >= 0; i1--) {
        NumericDigit var1digit = var1digits[i1];

        if (var1digit == 0) continue;

        /* Time to normalize? */
        maxdig += var1digit;
        if (maxdig > (INT_MAX - INT_MAX / NBASE) / (NBASE - 1)) {
            /* Yes, do it */
            carry = 0;
            for (i = res_ndigits - 1; i >= 0; i--) {
                newdig = dig[i] + carry;
                if (newdig >= NBASE) {
                    carry = newdig / NBASE;
                    newdig -= carry * NBASE;
                } else
                    carry = 0;
                dig[i] = newdig;
            }
            Assert(carry == 0);
            /* Reset maxdig to indicate new worst-case */
            maxdig = 1 + var1digit;
        }

        /*
         * Add the appropriate multiple of var2 into the accumulator.
         *
         * As above, digits of var2 can be ignored if they don't contribute,
         * so we only include digits for which i1+i2+2 < res_ndigits.
         *
         * This inner loop is the performance bottleneck for multiplication,
         * so we want to keep it simple enough so that it can be
         * auto-vectorized.  Accordingly, process the digits left-to-right
         * even though schoolbook multiplication would suggest right-to-left.
         * Since we aren't propagating carries in this loop, the order does
         * not matter.
         */
        {
            int i2limit = Min(var2ndigits, res_ndigits - i1 - 2);
            int *dig_i1_2 = &dig[i1 + 2];

            for (i2 = 0; i2 < i2limit; i2++)
                dig_i1_2[i2] += var1digit * var2digits[i2];
        }
    }

    /*
     * Now we do a final carry propagation pass to normalize the result, which
     * we combine with storing the result digits into the output. Note that
     * this is still done at full precision w/guard digits.
     */
    alloc_var(result, res_ndigits);
    res_digits = result->digits;
    carry = 0;
    for (i = res_ndigits - 1; i >= 0; i--) {
        newdig = dig[i] + carry;
        if (newdig >= NBASE) {
            carry = newdig / NBASE;
            newdig -= carry * NBASE;
        } else
            carry = 0;
        res_digits[i] = newdig;
    }
    Assert(carry == 0);

    pfree(dig);

    /*
     * Finally, round the result to the requested precision.
     */
    result->weight = res_weight;
    result->sign = res_sign;

    /* Round to target rscale (and set result->dscale) */
    round_var(result, rscale);

    /* Strip leading and trailing zeroes */
    strip_var(result);
}

/*
 * numeric_mul_opt_error() -
 *
 *	Internal version of numeric_mul().  If "*have_error" flag is provided,
 *	on error it's set to true, NULL returned.  This is helpful when caller
 *	need to handle errors by itself.
 */
Numeric numeric_mul_opt_error(Numeric num1, Numeric num2, bool *have_error) {
    NumericVar arg1;
    NumericVar arg2;
    NumericVar result;
    Numeric res;

    /*
     * Handle NaN and infinities
     */
    if (NUMERIC_IS_SPECIAL(num1) || NUMERIC_IS_SPECIAL(num2)) {
        if (NUMERIC_IS_NAN(num1) || NUMERIC_IS_NAN(num2))
            return make_result(&const_nan);
        if (NUMERIC_IS_PINF(num1)) {
            switch (numeric_sign_internal(num2)) {
                case 0:
                    return make_result(&const_nan); /* Inf * 0 */
                case 1:
                    return make_result(&const_pinf);
                case -1:
                    return make_result(&const_ninf);
            }
            Assert(false);
        }
        if (NUMERIC_IS_NINF(num1)) {
            switch (numeric_sign_internal(num2)) {
                case 0:
                    return make_result(&const_nan); /* -Inf * 0 */
                case 1:
                    return make_result(&const_ninf);
                case -1:
                    return make_result(&const_pinf);
            }
            Assert(false);
        }
        /* by here, num1 must be finite, so num2 is not */
        if (NUMERIC_IS_PINF(num2)) {
            switch (numeric_sign_internal(num1)) {
                case 0:
                    return make_result(&const_nan); /* 0 * Inf */
                case 1:
                    return make_result(&const_pinf);
                case -1:
                    return make_result(&const_ninf);
            }
            Assert(false);
        }
        Assert(NUMERIC_IS_NINF(num2));
        switch (numeric_sign_internal(num1)) {
            case 0:
                return make_result(&const_nan); /* 0 * -Inf */
            case 1:
                return make_result(&const_ninf);
            case -1:
                return make_result(&const_pinf);
        }
        Assert(false);
    }

    /*
     * Unpack the values, let mul_var() compute the result and return it.
     * Unlike add_var() and sub_var(), mul_var() will round its result. In
     the
     * case of numeric_mul(), which is invoked for the * operator on
     numerics,
     * we request exact representation for the product (rscale = sum(dscale
     of
     * arg1, dscale of arg2)).  If the exact result has more digits after the
     * decimal point than can be stored in a numeric, we round it.  Rounding
     * after computing the exact result ensures that the final result is
     * correctly rounded (rounding in mul_var() using a truncated product
     * would not guarantee this).
     */
    init_var_from_num(num1, &arg1);
    init_var_from_num(num2, &arg2);

    init_var(&result);
    mul_var(&arg1, &arg2, &result, arg1.dscale + arg2.dscale);

    if (result.dscale > NUMERIC_DSCALE_MAX)
        round_var(&result, NUMERIC_DSCALE_MAX);

    res = make_result_opt_error(&result, have_error);

    free_var(&result);

    return res;
}

static int select_div_scale(const NumericVar *var1, const NumericVar *var2) {
    int weight1, weight2, qweight, i;
    NumericDigit firstdigit1, firstdigit2;
    int rscale;

    /*
     * The result scale of a division isn't specified in any SQL standard. For
     * PostgreSQL we select a result scale that will give at least
     * NUMERIC_MIN_SIG_DIGITS significant digits, so that numeric gives a
     * result no less accurate than float8; but use a scale not less than
     * either input's display scale.
     */

    /* Get the actual (normalized) weight and first digit of each input */

    weight1 = 0; /* values to use if var1 is zero */
    firstdigit1 = 0;
    for (i = 0; i < var1->ndigits; i++) {
        firstdigit1 = var1->digits[i];
        if (firstdigit1 != 0) {
            weight1 = var1->weight - i;
            break;
        }
    }

    weight2 = 0; /* values to use if var2 is zero */
    firstdigit2 = 0;
    for (i = 0; i < var2->ndigits; i++) {
        firstdigit2 = var2->digits[i];
        if (firstdigit2 != 0) {
            weight2 = var2->weight - i;
            break;
        }
    }

    /*
     * Estimate weight of quotient.  If the two first digits are equal, we
     * can't be sure, but assume that var1 is less than var2.
     */
    qweight = weight1 - weight2;
    if (firstdigit1 <= firstdigit2) qweight--;

    /* Select result scale */
    rscale = NUMERIC_MIN_SIG_DIGITS - qweight * DEC_DIGITS;
    rscale = Max(rscale, var1->dscale);
    rscale = Max(rscale, var2->dscale);
    rscale = Max(rscale, NUMERIC_MIN_DISPLAY_SCALE);
    rscale = Min(rscale, NUMERIC_MAX_DISPLAY_SCALE);

    return rscale;
}

static void div_var_int(const NumericVar *var, int ival, int ival_weight,
                        NumericVar *result, int rscale, bool round) {
    NumericDigit *var_digits = var->digits;
    int var_ndigits = var->ndigits;
    int res_sign;
    int res_weight;
    int res_ndigits;
    NumericDigit *res_buf;
    NumericDigit *res_digits;
    uint32 divisor;
    int i;

    /* Guard against division by zero */
    if (ival == 0) {
        printf("division by zero\n");
        // ereport(ERROR, errcode(ERRCODE_DIVISION_BY_ZERO),
        //         errmsg("division by zero"));
    }

    /* Result zero check */
    if (var_ndigits == 0) {
        zero_var(result);
        result->dscale = rscale;
        return;
    }

    /*
     * Determine the result sign, weight and number of digits to calculate.
     * The weight figured here is correct if the emitted quotient has no
     * leading zero digits; otherwise strip_var() will fix things up.
     */
    if (var->sign == NUMERIC_POS)
        res_sign = ival > 0 ? NUMERIC_POS : NUMERIC_NEG;
    else
        res_sign = ival > 0 ? NUMERIC_NEG : NUMERIC_POS;
    res_weight = var->weight - ival_weight;
    /* The number of accurate result digits we need to produce: */
    res_ndigits = res_weight + 1 + (rscale + DEC_DIGITS - 1) / DEC_DIGITS;
    /* ... but always at least 1 */
    res_ndigits = Max(res_ndigits, 1);
    /* If rounding needed, figure one more digit to ensure correct result */
    if (round) res_ndigits++;

    res_buf = digitbuf_alloc(res_ndigits + 1);
    res_buf[0] = 0; /* spare digit for later rounding */
    res_digits = res_buf + 1;

    /*
     * Now compute the quotient digits.  This is the short division algorithm
     * described in Knuth volume 2, section 4.3.1 exercise 16, except that we
     * allow the divisor to exceed the internal base.
     *
     * In this algorithm, the carry from one digit to the next is at most
     * divisor - 1.  Therefore, while processing the next digit, carry may
     * become as large as divisor * NBASE - 1, and so it requires a 64-bit
     * integer if this exceeds UINT_MAX.
     */
    divisor = abs(ival);

    if (divisor <= UINT_MAX / NBASE) {
        /* carry cannot overflow 32 bits */
        uint32 carry = 0;

        for (i = 0; i < res_ndigits; i++) {
            carry = carry * NBASE + (i < var_ndigits ? var_digits[i] : 0);
            res_digits[i] = (NumericDigit)(carry / divisor);
            carry = carry % divisor;
        }
    } else {
        /* carry may exceed 32 bits */
        uint64 carry = 0;

        for (i = 0; i < res_ndigits; i++) {
            carry = carry * NBASE + (i < var_ndigits ? var_digits[i] : 0);
            res_digits[i] = (NumericDigit)(carry / divisor);
            carry = carry % divisor;
        }
    }

    /* Store the quotient in result */
    digitbuf_free(result->buf);
    result->ndigits = res_ndigits;
    result->buf = res_buf;
    result->digits = res_digits;
    result->weight = res_weight;
    result->sign = res_sign;

    /* Round or truncate to target rscale (and set result->dscale) */
    if (round)
        round_var(result, rscale);
    else
        trunc_var(result, rscale);

    /* Strip leading/trailing zeroes */
    strip_var(result);
}

static void div_var(const NumericVar *var1, const NumericVar *var2,
                    NumericVar *result, int rscale, bool round) {
    int div_ndigits;
    int res_ndigits;
    int res_sign;
    int res_weight;
    int carry;
    int borrow;
    int divisor1;
    int divisor2;
    NumericDigit *dividend;
    NumericDigit *divisor;
    NumericDigit *res_digits;
    int i;
    int j;

    /* copy these values into local vars for speed in inner loop */
    int var1ndigits = var1->ndigits;
    int var2ndigits = var2->ndigits;

    /*
     * First of all division by zero check; we must not be handed an
     * unnormalized divisor.
     */
    if (var2ndigits == 0 || var2->digits[0] == 0) {
        printf("division by zero\n");
        // ereport(ERROR, (errcode(ERRCODE_DIVISION_BY_ZERO),
        //                 errmsg("division by zero")));
    }
    /*
     * If the divisor has just one or two digits, delegate to div_var_int(),
     * which uses fast short division.
     *
     * Similarly, on platforms with 128-bit integer support, delegate to
     * div_var_int64() for divisors with three or four digits.
     */
    if (var2ndigits <= 2) {
        int idivisor;
        int idivisor_weight;

        idivisor = var2->digits[0];
        idivisor_weight = var2->weight;
        if (var2ndigits == 2) {
            idivisor = idivisor * NBASE + var2->digits[1];
            idivisor_weight--;
        }
        if (var2->sign == NUMERIC_NEG) idivisor = -idivisor;

        div_var_int(var1, idivisor, idivisor_weight, result, rscale, round);
        return;
    }
#ifdef HAVE_INT128
    if (var2ndigits <= 4) {
        int64 idivisor;
        int idivisor_weight;

        idivisor = var2->digits[0];
        idivisor_weight = var2->weight;
        for (i = 1; i < var2ndigits; i++) {
            idivisor = idivisor * NBASE + var2->digits[i];
            idivisor_weight--;
        }
        if (var2->sign == NUMERIC_NEG) idivisor = -idivisor;

        div_var_int64(var1, idivisor, idivisor_weight, result, rscale, round);
        return;
    }
#endif

    /*
     * Otherwise, perform full long division.
     */

    /* Result zero check */
    if (var1ndigits == 0) {
        zero_var(result);
        result->dscale = rscale;
        return;
    }

    /*
     * Determine the result sign, weight and number of digits to calculate.
     * The weight figured here is correct if the emitted quotient has no
     * leading zero digits; otherwise strip_var() will fix things up.
     */
    if (var1->sign == var2->sign)
        res_sign = NUMERIC_POS;
    else
        res_sign = NUMERIC_NEG;
    res_weight = var1->weight - var2->weight;
    /* The number of accurate result digits we need to produce: */
    res_ndigits = res_weight + 1 + (rscale + DEC_DIGITS - 1) / DEC_DIGITS;
    /* ... but always at least 1 */
    res_ndigits = Max(res_ndigits, 1);
    /* If rounding needed, figure one more digit to ensure correct result */
    if (round) res_ndigits++;

    /*
     * The working dividend normally requires res_ndigits + var2ndigits
     * digits, but make it at least var1ndigits so we can load all of var1
     * into it.  (There will be an additional digit dividend[0] in the
     * dividend space, but for consistency with Knuth's notation we don't
     * count that in div_ndigits.)
     */
    div_ndigits = res_ndigits + var2ndigits;
    div_ndigits = Max(div_ndigits, var1ndigits);

    /*
     * We need a workspace with room for the working dividend (div_ndigits+1
     * digits) plus room for the possibly-normalized divisor (var2ndigits
     * digits).  It is convenient also to have a zero at divisor[0] with the
     * actual divisor data in divisor[1 .. var2ndigits].  Transferring the
     * digits into the workspace also allows us to realloc the result (which
     * might be the same as either input var) before we begin the main loop.
     * Note that we use palloc0 to ensure that divisor[0], dividend[0], and
     * any additional dividend positions beyond var1ndigits, start out 0.
     */
    dividend = (NumericDigit *)calloc((div_ndigits + var2ndigits + 2),
                                      sizeof(NumericDigit));
    divisor = dividend + (div_ndigits + 1);
    memcpy(dividend + 1, var1->digits, var1ndigits * sizeof(NumericDigit));
    memcpy(divisor + 1, var2->digits, var2ndigits * sizeof(NumericDigit));

    /*
     * Now we can realloc the result to hold the generated quotient digits.
     */
    alloc_var(result, res_ndigits);
    res_digits = result->digits;

    /*
     * The full multiple-place algorithm is taken from Knuth volume 2,
     * Algorithm 4.3.1D.
     *
     * We need the first divisor digit to be >= NBASE/2.  If it isn't, make it
     * so by scaling up both the divisor and dividend by the factor "d".  (The
     * reason for allocating dividend[0] above is to leave room for possible
     * carry here.)
     */
    if (divisor[1] < HALF_NBASE) {
        int d = NBASE / (divisor[1] + 1);

        carry = 0;
        for (i = var2ndigits; i > 0; i--) {
            carry += divisor[i] * d;
            divisor[i] = carry % NBASE;
            carry = carry / NBASE;
        }
        Assert(carry == 0);
        carry = 0;
        /* at this point only var1ndigits of dividend can be nonzero */
        for (i = var1ndigits; i >= 0; i--) {
            carry += dividend[i] * d;
            dividend[i] = carry % NBASE;
            carry = carry / NBASE;
        }
        Assert(carry == 0);
        Assert(divisor[1] >= HALF_NBASE);
    }
    /* First 2 divisor digits are used repeatedly in main loop */
    divisor1 = divisor[1];
    divisor2 = divisor[2];

    /*
     * Begin the main loop.  Each iteration of this loop produces the j'th
     * quotient digit by dividing dividend[j .. j + var2ndigits] by the
     * divisor; this is essentially the same as the common manual procedure
     * for long division.
     */
    for (j = 0; j < res_ndigits; j++) {
        /* Estimate quotient digit from the first two dividend digits */
        int next2digits = dividend[j] * NBASE + dividend[j + 1];
        int qhat;

        /*
         * If next2digits are 0, then quotient digit must be 0 and there's no
         * need to adjust the working dividend.  It's worth testing here to
         * fall out ASAP when processing trailing zeroes in a dividend.
         */
        if (next2digits == 0) {
            res_digits[j] = 0;
            continue;
        }

        if (dividend[j] == divisor1)
            qhat = NBASE - 1;
        else
            qhat = next2digits / divisor1;

        /*
         * Adjust quotient digit if it's too large.  Knuth proves that after
         * this step, the quotient digit will be either correct or just one
         * too large.  (Note: it's OK to use dividend[j+2] here because we
         * know the divisor length is at least 2.)
         */
        while (divisor2 * qhat >
               (next2digits - qhat * divisor1) * NBASE + dividend[j + 2])
            qhat--;

        /* As above, need do nothing more when quotient digit is 0 */
        if (qhat > 0) {
            NumericDigit *dividend_j = &dividend[j];

            /*
             * Multiply the divisor by qhat, and subtract that from the
             * working dividend.  The multiplication and subtraction are
             * folded together here, noting that qhat <= NBASE (since it might
             * be one too large), and so the intermediate result "tmp_result"
             * is in the range [-NBASE^2, NBASE - 1], and "borrow" is in the
             * range [0, NBASE].
             */
            borrow = 0;
            for (i = var2ndigits; i >= 0; i--) {
                int tmp_result;

                tmp_result = dividend_j[i] - borrow - divisor[i] * qhat;
                borrow = (NBASE - 1 - tmp_result) / NBASE;
                dividend_j[i] = tmp_result + borrow * NBASE;
            }

            /*
             * If we got a borrow out of the top dividend digit, then indeed
             * qhat was one too large.  Fix it, and add back the divisor to
             * correct the working dividend.  (Knuth proves that this will
             * occur only about 3/NBASE of the time; hence, it's a good idea
             * to test this code with small NBASE to be sure this section gets
             * exercised.)
             */
            if (borrow) {
                qhat--;
                carry = 0;
                for (i = var2ndigits; i >= 0; i--) {
                    carry += dividend_j[i] + divisor[i];
                    if (carry >= NBASE) {
                        dividend_j[i] = carry - NBASE;
                        carry = 1;
                    } else {
                        dividend_j[i] = carry;
                        carry = 0;
                    }
                }
                /* A carry should occur here to cancel the borrow above */
                Assert(carry == 1);
            }
        }

        /* And we're done with this quotient digit */
        res_digits[j] = qhat;
    }

    pfree(dividend);

    /*
     * Finally, round or truncate the result to the requested precision.
     */
    result->weight = res_weight;
    result->sign = res_sign;

    /* Round or truncate to target rscale (and set result->dscale) */
    if (round)
        round_var(result, rscale);
    else
        trunc_var(result, rscale);

    /* Strip leading and trailing zeroes */
    strip_var(result);
}

/*
 * numeric_div_opt_error() -
 *
 *	Internal version of numeric_div().  If "*have_error" flag is provided,
 *	on error it's set to true, NULL returned.  This is helpful when caller
 *	need to handle errors by itself.
 */
Numeric numeric_div_opt_error(Numeric num1, Numeric num2, bool *have_error) {
    NumericVar arg1;
    NumericVar arg2;
    NumericVar result;
    Numeric res;
    int rscale;

    if (have_error) *have_error = false;

    /*
     * Handle NaN and infinities
     */
    if (NUMERIC_IS_SPECIAL(num1) || NUMERIC_IS_SPECIAL(num2)) {
        if (NUMERIC_IS_NAN(num1) || NUMERIC_IS_NAN(num2))
            return make_result(&const_nan);
        if (NUMERIC_IS_PINF(num1)) {
            if (NUMERIC_IS_SPECIAL(num2))
                return make_result(&const_nan); /* Inf / [-]Inf */
            switch (numeric_sign_internal(num2)) {
                case 0:
                    if (have_error) {
                        *have_error = true;
                        return NULL;
                    }
                    printf("division by zero\n");
                    // ereport(ERROR,
                    // 		(errcode(ERRCODE_DIVISION_BY_ZERO),
                    // 		 errmsg("division by zero")));
                    break;
                case 1:
                    return make_result(&const_pinf);
                case -1:
                    return make_result(&const_ninf);
            }
            Assert(false);
        }
        if (NUMERIC_IS_NINF(num1)) {
            if (NUMERIC_IS_SPECIAL(num2))
                return make_result(&const_nan); /* -Inf / [-]Inf */
            switch (numeric_sign_internal(num2)) {
                case 0:
                    if (have_error) {
                        *have_error = true;
                        return NULL;
                    }
                    printf("division by zero\n");
                    // ereport(ERROR,
                    // 		(errcode(ERRCODE_DIVISION_BY_ZERO),
                    // 		 errmsg("division by zero")));
                    break;
                case 1:
                    return make_result(&const_ninf);
                case -1:
                    return make_result(&const_pinf);
            }
            Assert(false);
        }
        /* by here, num1 must be finite, so num2 is not */

        /*
         * POSIX would have us return zero or minus zero if num1 is zero, and
         * otherwise throw an underflow error.  But the numeric type doesn't
         * really do underflow, so let's just return zero.
         */
        return make_result(&const_zero);
    }

    /*
     * Unpack the arguments
     */
    init_var_from_num(num1, &arg1);
    init_var_from_num(num2, &arg2);

    init_var(&result);

    /*
     * Select scale for division result
     */
    rscale = select_div_scale(&arg1, &arg2);

    /*
     * If "have_error" is provided, check for division by zero here
     */
    if (have_error && (arg2.ndigits == 0 || arg2.digits[0] == 0)) {
        *have_error = true;
        return NULL;
    }

    /*
     * Do the divide and return the result
     */
    div_var(&arg1, &arg2, &result, rscale, true);

    res = make_result_opt_error(&result, have_error);

    free_var(&result);

    return res;
}

static char *get_str_from_var(const NumericVar *var) {
    int dscale;
    char *str;
    char *cp;
    char *endcp;
    int i;
    int d;
    NumericDigit dig;

#if DEC_DIGITS > 1
    NumericDigit d1;
#endif

    dscale = var->dscale;

    /*
     * Allocate space for the result.
     *
     * i is set to the # of decimal digits before decimal point. dscale is the
     * # of decimal digits we will print after decimal point. We may generate
     * as many as DEC_DIGITS-1 excess digits at the end, and in addition we
     * need room for sign, decimal point, null terminator.
     */
    i = (var->weight + 1) * DEC_DIGITS;
    if (i <= 0) i = 1;

    str = palloc(i + dscale + DEC_DIGITS + 2);
    cp = str;

    /*
     * Output a dash for negative values
     */
    if (var->sign == NUMERIC_NEG) *cp++ = '-';

    /*
     * Output all digits before the decimal point
     */
    if (var->weight < 0) {
        d = var->weight + 1;
        *cp++ = '0';
    } else {
        for (d = 0; d <= var->weight; d++) {
            dig = (d < var->ndigits) ? var->digits[d] : 0;
            /* In the first digit, suppress extra leading decimal zeroes */
#if DEC_DIGITS == 4
            {
                bool putit = (d > 0);

                d1 = dig / 1000;
                dig -= d1 * 1000;
                putit |= (d1 > 0);
                if (putit) *cp++ = d1 + '0';
                d1 = dig / 100;
                dig -= d1 * 100;
                putit |= (d1 > 0);
                if (putit) *cp++ = d1 + '0';
                d1 = dig / 10;
                dig -= d1 * 10;
                putit |= (d1 > 0);
                if (putit) *cp++ = d1 + '0';
                *cp++ = dig + '0';
            }
#elif DEC_DIGITS == 2
            d1 = dig / 10;
            dig -= d1 * 10;
            if (d1 > 0 || d > 0) *cp++ = d1 + '0';
            *cp++ = dig + '0';
#elif DEC_DIGITS == 1
            *cp++ = dig + '0';
#else
#error unsupported NBASE
#endif
        }
    }

    /*
     * If requested, output a decimal point and all the digits that follow it.
     * We initially put out a multiple of DEC_DIGITS digits, then truncate if
     * needed.
     */
    if (dscale > 0) {
        *cp++ = '.';
        endcp = cp + dscale;
        for (i = 0; i < dscale; d++, i += DEC_DIGITS) {
            dig = (d >= 0 && d < var->ndigits) ? var->digits[d] : 0;
#if DEC_DIGITS == 4
            d1 = dig / 1000;
            dig -= d1 * 1000;
            *cp++ = d1 + '0';
            d1 = dig / 100;
            dig -= d1 * 100;
            *cp++ = d1 + '0';
            d1 = dig / 10;
            dig -= d1 * 10;
            *cp++ = d1 + '0';
            *cp++ = dig + '0';
#elif DEC_DIGITS == 2
            d1 = dig / 10;
            dig -= d1 * 10;
            *cp++ = d1 + '0';
            *cp++ = dig + '0';
#elif DEC_DIGITS == 1
            *cp++ = dig + '0';
#else
#error unsupported NBASE
#endif
        }
        cp = endcp;
    }

    /*
     * terminate the string and return it
     */
    *cp = '\0';
    return str;
}

void print_numeric(Numeric x) {
    NumericVar xv;
    char *str;
    init_var_from_num(x, &xv);
    str = get_str_from_var(&xv);
    printf("a + b = %s\n", str);
}

void Numeric_ToString(Numeric x) {
    NumericVar xv;
    char *str;
    init_var_from_num(x, &xv);
    str = get_str_from_var(&xv);
    // memcpy(out, str, strlen(str));
}

static bool set_var_from_str(const char *str, const char *cp, NumericVar *dest,
                             const char **endptr) {
    bool have_dp = false;
    int i;
    unsigned char *decdigits;
    int sign = NUMERIC_POS;
    int dweight = -1;
    int ddigits;
    int dscale = 0;
    int weight;
    int ndigits;
    int offset;
    NumericDigit *digits;

    /*
     * We first parse the string to extract decimal digits and determine the
     * correct decimal weight.  Then convert to NBASE representation.
     */
    switch (*cp) {
        case '+':
            sign = NUMERIC_POS;
            cp++;
            break;

        case '-':
            sign = NUMERIC_NEG;
            cp++;
            break;
    }

    if (*cp == '.') {
        have_dp = true;
        cp++;
    }

    if (!isdigit((unsigned char)*cp)) goto invalid_syntax;

    decdigits = (unsigned char *)palloc(strlen(cp) + DEC_DIGITS * 2);

    /* leading padding for digit alignment later */
    memset(decdigits, 0, DEC_DIGITS);
    i = DEC_DIGITS;

    while (*cp) {
        if (isdigit((unsigned char)*cp)) {
            decdigits[i++] = *cp++ - '0';
            if (!have_dp)
                dweight++;
            else
                dscale++;
        } else if (*cp == '.') {
            if (have_dp) goto invalid_syntax;
            have_dp = true;
            cp++;
            /* decimal point must not be followed by underscore */
            if (*cp == '_') goto invalid_syntax;
        } else if (*cp == '_') {
            /* underscore must be followed by more digits */
            cp++;
            if (!isdigit((unsigned char)*cp)) goto invalid_syntax;
        } else
            break;
    }

    ddigits = i - DEC_DIGITS;
    /* trailing padding for digit alignment later */
    memset(decdigits + i, 0, DEC_DIGITS - 1);

    /* Handle exponent, if any */
    if (*cp == 'e' || *cp == 'E') {
        int64 exponent = 0;
        bool neg = false;

        /*
         * At this point, dweight and dscale can't be more than about
         * INT_MAX/2 due to the MaxAllocSize limit on string length, so
         * constraining the exponent similarly should be enough to prevent
         * integer overflow in this function.  If the value is too large to
         * fit in storage format, make_result() will complain about it later;
         * for consistency use the same ereport errcode/text as make_result().
         */

        /* exponent sign */
        cp++;
        if (*cp == '+')
            cp++;
        else if (*cp == '-') {
            neg = true;
            cp++;
        }

        /* exponent digits */
        if (!isdigit((unsigned char)*cp)) goto invalid_syntax;

        while (*cp) {
            if (isdigit((unsigned char)*cp)) {
                exponent = exponent * 10 + (*cp++ - '0');
                if (exponent > PG_INT32_MAX / 2) goto out_of_range;
            } else if (*cp == '_') {
                /* underscore must be followed by more digits */
                cp++;
                if (!isdigit((unsigned char)*cp)) goto invalid_syntax;
            } else
                break;
        }

        if (neg) exponent = -exponent;

        dweight += (int)exponent;
        dscale -= (int)exponent;
        if (dscale < 0) dscale = 0;
    }

    /*
     * Okay, convert pure-decimal representation to base NBASE.  First we need
     * to determine the converted weight and ndigits.  offset is the number of
     * decimal zeroes to insert before the first given digit to have a
     * correctly aligned first NBASE digit.
     */
    if (dweight >= 0)
        weight = (dweight + 1 + DEC_DIGITS - 1) / DEC_DIGITS - 1;
    else
        weight = -((-dweight - 1) / DEC_DIGITS + 1);
    offset = (weight + 1) * DEC_DIGITS - (dweight + 1);
    ndigits = (ddigits + offset + DEC_DIGITS - 1) / DEC_DIGITS;

    alloc_var(dest, ndigits);
    dest->sign = sign;
    dest->weight = weight;
    dest->dscale = dscale;

    i = DEC_DIGITS - offset;
    digits = dest->digits;

    while (ndigits-- > 0) {
#if DEC_DIGITS == 4
        *digits++ =
            ((decdigits[i] * 10 + decdigits[i + 1]) * 10 + decdigits[i + 2]) *
                10 +
            decdigits[i + 3];
#elif DEC_DIGITS == 2
        *digits++ = decdigits[i] * 10 + decdigits[i + 1];
#elif DEC_DIGITS == 1
        *digits++ = decdigits[i];
#else
#error unsupported NBASE
#endif
        i += DEC_DIGITS;
    }

    pfree(decdigits);

    /* Strip any leading/trailing zeroes, and normalize weight if zero */
    strip_var(dest);

    /* Return end+1 position for caller */
    *endptr = cp;

    return true;

out_of_range:
    printf("value overflows numeric format");

invalid_syntax:
    printf("invalid input syntax for type %s: \"%s\"", "numeric\n", str);
}

static inline int xdigit_value(char dig) {
    return dig >= '0' && dig <= '9'   ? dig - '0'
           : dig >= 'a' && dig <= 'f' ? dig - 'a' + 10
           : dig >= 'A' && dig <= 'F' ? dig - 'A' + 10
                                      : -1;
}

static bool set_var_from_non_decimal_integer_str(const char *str,
                                                 const char *cp, int sign,
                                                 int base, NumericVar *dest,
                                                 const char **endptr) {
    const char *firstdigit = cp;
    int64 tmp;
    int64 mul;
    NumericVar tmp_var;

    init_var(&tmp_var);

    zero_var(dest);

    /*
     * Process input digits in groups that fit in int64.  Here "tmp" is the
     * value of the digits in the group, and "mul" is base^n, where n is the
     * number of digits in the group.  Thus tmp < mul, and we must start a new
     * group when mul * base threatens to overflow PG_INT64_MAX.
     */
    tmp = 0;
    mul = 1;

    if (base == 16) {
        while (*cp) {
            if (isxdigit((unsigned char)*cp)) {
                if (mul > PG_INT64_MAX / 16) {
                    /* Add the contribution from this group of digits */
                    int64_to_numericvar(mul, &tmp_var);
                    mul_var(dest, &tmp_var, dest, 0);
                    int64_to_numericvar(tmp, &tmp_var);
                    add_var(dest, &tmp_var, dest);

                    /* Result will overflow if weight overflows int16 */
                    if (dest->weight > SHRT_MAX) goto out_of_range;

                    /* Begin a new group */
                    tmp = 0;
                    mul = 1;
                }

                tmp = tmp * 16 + xdigit_value(*cp++);
                mul = mul * 16;
            } else if (*cp == '_') {
                /* Underscore must be followed by more digits */
                cp++;
                if (!isxdigit((unsigned char)*cp)) goto invalid_syntax;
            } else
                break;
        }
    } else if (base == 8) {
        while (*cp) {
            if (*cp >= '0' && *cp <= '7') {
                if (mul > PG_INT64_MAX / 8) {
                    /* Add the contribution from this group of digits */
                    int64_to_numericvar(mul, &tmp_var);
                    mul_var(dest, &tmp_var, dest, 0);
                    int64_to_numericvar(tmp, &tmp_var);
                    add_var(dest, &tmp_var, dest);

                    /* Result will overflow if weight overflows int16 */
                    if (dest->weight > SHRT_MAX) goto out_of_range;

                    /* Begin a new group */
                    tmp = 0;
                    mul = 1;
                }

                tmp = tmp * 8 + (*cp++ - '0');
                mul = mul * 8;
            } else if (*cp == '_') {
                /* Underscore must be followed by more digits */
                cp++;
                if (*cp < '0' || *cp > '7') goto invalid_syntax;
            } else
                break;
        }
    } else if (base == 2) {
        while (*cp) {
            if (*cp >= '0' && *cp <= '1') {
                if (mul > PG_INT64_MAX / 2) {
                    /* Add the contribution from this group of digits */
                    int64_to_numericvar(mul, &tmp_var);
                    mul_var(dest, &tmp_var, dest, 0);
                    int64_to_numericvar(tmp, &tmp_var);
                    add_var(dest, &tmp_var, dest);

                    /* Result will overflow if weight overflows int16 */
                    if (dest->weight > SHRT_MAX) goto out_of_range;

                    /* Begin a new group */
                    tmp = 0;
                    mul = 1;
                }

                tmp = tmp * 2 + (*cp++ - '0');
                mul = mul * 2;
            } else if (*cp == '_') {
                /* Underscore must be followed by more digits */
                cp++;
                if (*cp < '0' || *cp > '1') goto invalid_syntax;
            } else
                break;
        }
    } else
        /* Should never happen; treat as invalid input */
        goto invalid_syntax;

    /* Check that we got at least one digit */
    if (unlikely(cp == firstdigit)) goto invalid_syntax;

    /* Add the contribution from the final group of digits */
    int64_to_numericvar(mul, &tmp_var);
    mul_var(dest, &tmp_var, dest, 0);
    int64_to_numericvar(tmp, &tmp_var);
    add_var(dest, &tmp_var, dest);

    if (dest->weight > SHRT_MAX) goto out_of_range;

    dest->sign = sign;

    free_var(&tmp_var);

    /* Return end+1 position for caller */
    *endptr = cp;

    return true;

out_of_range:
    printf("value overflows numeric format\n");

invalid_syntax:
    printf("invalid input syntax for type %s: \"%s\"", "numeric\n", str);
}

int pg_strncasecmp(const char *s1, const char *s2, size_t n) {
    while (n-- > 0) {
        unsigned char ch1 = (unsigned char)*s1++;
        unsigned char ch2 = (unsigned char)*s2++;

        if (ch1 != ch2) {
            if (ch1 >= 'A' && ch1 <= 'Z')
                ch1 += 'a' - 'A';
            else if (IS_HIGHBIT_SET(ch1) && isupper(ch1))
                ch1 = tolower(ch1);

            if (ch2 >= 'A' && ch2 <= 'Z')
                ch2 += 'a' - 'A';
            else if (IS_HIGHBIT_SET(ch2) && isupper(ch2))
                ch2 = tolower(ch2);

            if (ch1 != ch2) return (int)ch1 - (int)ch2;
        }
        if (ch1 == 0) break;
    }
    return 0;
}

Numeric numeric_in(const char *str) {
    Numeric res;
    const char *cp;
    const char *numstart;
    int sign;

    /* Skip leading spaces */
    cp = str;
    while (*cp) {
        if (!isspace((unsigned char)*cp)) break;
        cp++;
    }

    /*
     * Process the number's sign. This duplicates logic in set_var_from_str(),
     * but it's worth doing here, since it simplifies the handling of
     * infinities and non-decimal integers.
     */
    numstart = cp;
    sign = NUMERIC_POS;

    if (*cp == '+')
        cp++;
    else if (*cp == '-') {
        sign = NUMERIC_NEG;
        cp++;
    }

    /*
     * Check for NaN and infinities.  We recognize the same strings allowed by
     * float8in().
     *
     * Since all other legal inputs have a digit or a decimal point after the
     * sign, we need only check for NaN/infinity if that's not the case.
     */
    if (!isdigit((unsigned char)*cp) && *cp != '.') {
        /*
         * The number must be NaN or infinity; anything else can only be a
         * syntax error. Note that NaN mustn't have a sign.
         */
        if (pg_strncasecmp(numstart, "NaN", 3) == 0) {
            res = make_result(&const_nan);
            cp = numstart + 3;
        } else if (pg_strncasecmp(cp, "Infinity", 8) == 0) {
            res = make_result(sign == NUMERIC_POS ? &const_pinf : &const_ninf);
            cp += 8;
        } else if (pg_strncasecmp(cp, "inf", 3) == 0) {
            res = make_result(sign == NUMERIC_POS ? &const_pinf : &const_ninf);
            cp += 3;
        } else
            goto invalid_syntax;

        /*
         * Check for trailing junk; there should be nothing left but spaces.
         *
         * We intentionally do this check before applying the typmod because
         * we would like to throw any trailing-junk syntax error before any
         * semantic error resulting from apply_typmod_special().
         */
        while (*cp) {
            if (!isspace((unsigned char)*cp)) goto invalid_syntax;
            cp++;
        }
    } else {
        /*
         * We have a normal numeric value, which may be a non-decimal integer
         * or a regular decimal number.
         */
        NumericVar value;
        int base;
        bool have_error;

        init_var(&value);

        /*
         * Determine the number's base by looking for a non-decimal prefix
         * indicator ("0x", "0o", or "0b").
         */
        if (cp[0] == '0') {
            switch (cp[1]) {
                case 'x':
                case 'X':
                    base = 16;
                    break;
                case 'o':
                case 'O':
                    base = 8;
                    break;
                case 'b':
                case 'B':
                    base = 2;
                    break;
                default:
                    base = 10;
            }
        } else
            base = 10;

        /* Parse the rest of the number and apply the sign */
        if (base == 10) {
            if (!set_var_from_str(str, cp, &value, &cp)) return NULL;
            value.sign = sign;
        } else {
            if (!set_var_from_non_decimal_integer_str(str, cp + 2, sign, base,
                                                      &value, &cp))
                return NULL;
        }

        /*
         * Should be nothing left but spaces. As above, throw any typmod error
         * after finishing syntax check.
         */
        while (*cp) {
            if (!isspace((unsigned char)*cp)) goto invalid_syntax;
            cp++;
        }

        res = make_result_opt_error(&value, &have_error);

        if (have_error) printf("value overflows numeric format");

        free_var(&value);
    }

    return res;

invalid_syntax:
    printf("invalid input syntax for type %s: \"%s\"", "numeric\n", str);
}