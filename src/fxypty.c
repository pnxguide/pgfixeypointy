// clang-format off
#include "postgres.h"
#include "fmgr.h"
#include "utils/array.h"
// clang-format on

void *_fxypty_in(char *input, uint64_t scale);
void _fxypty_out(char out[64], void *in);
void *_fxypty_add(void *a, void *b);
void *_fxypty_subtract(void *a, void *b);
void *_fxypty_multiply(void *a, void *b);
void *_fxypty_divide(void *a, void *b);
int _fxypty_compare(void *a, void *b);

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(fxypty_in);
PG_FUNCTION_INFO_V1(fxypty_out);
PG_FUNCTION_INFO_V1(fxypty_typmod_in);
PG_FUNCTION_INFO_V1(fxypty_typmod_out);

PG_FUNCTION_INFO_V1(fxypty);

PG_FUNCTION_INFO_V1(fxypty_add);
PG_FUNCTION_INFO_V1(fxypty_subtract);
PG_FUNCTION_INFO_V1(fxypty_multiply);
PG_FUNCTION_INFO_V1(fxypty_divide);
PG_FUNCTION_INFO_V1(fxypty_eq);
PG_FUNCTION_INFO_V1(fxypty_neq);
PG_FUNCTION_INFO_V1(fxypty_lt);
PG_FUNCTION_INFO_V1(fxypty_gt);
PG_FUNCTION_INFO_V1(fxypty_lte);
PG_FUNCTION_INFO_V1(fxypty_gte);
PG_FUNCTION_INFO_V1(fxypty_smaller);
PG_FUNCTION_INFO_V1(fxypty_larger);

#define MAXIMUM_DIGIT 38
#define DEFAULT_SCALE 2

/**
 * Casting functions
 */

/// @brief Cast from a fxypty object with its type modifier into another fxypty
/// object (Use to include the type modifier)
/// @param [fxypty, int32] refer to the pointer to the fxypty object and the
/// type modifier
/// @return The pointer to the new fxypty object
Datum fxypty(PG_FUNCTION_ARGS) {
    void *decimal = (void *)PG_GETARG_POINTER(0);
    int32 typmod = PG_GETARG_INT32(1);
    Datum result;

    if (typmod != -1) {
        result = DirectFunctionCall1(fxypty_out, (uint64)decimal);
        result = DirectFunctionCall3(fxypty_in, result, 0, typmod);
    }

    PG_RETURN_POINTER(decimal);
}

/**
 * Input/output functions
 */

/// @brief Parse an input string and generate a fxypty object based on the
/// string.
/// @param [string, Oid, int32] where the string is the string decimal and int32
/// is the type modifier.
/// @return The pointer to the fxypty object.
Datum fxypty_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);
    char input_buffer[64];
    void *result;

    // If the typmod check starts here
    int32 number_of_digits;
    int32 number_of_fractional_digits;
    int32 typmod;

    number_of_digits = MAXIMUM_DIGIT;
    number_of_fractional_digits = DEFAULT_SCALE;
    typmod = PG_GETARG_INT32(2);
    if (typmod != -1) {
        number_of_digits = (typmod >> 16) & 0xffff;
        number_of_fractional_digits = typmod & 0xffff;
    }

    // FIXME: Check whether the number_of_digits matches or not
    (void)number_of_digits;

    // Check input syntax
    if (sscanf(str, "%s", input_buffer) != 1) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                 errmsg("invalid syntax for type %s: \"%s\"", "fxypty", str)));
    }

    // Generate input
    result = _fxypty_in(input_buffer, number_of_fractional_digits);

    PG_RETURN_POINTER(result);
}

/// @brief Generate a string from the fxypty object.
/// @param [fxypty] A pointer to the fxypty object.
/// @return A string generated from the fxypty object.
Datum fxypty_out(PG_FUNCTION_ARGS) {
    void *decimal = (void *)PG_GETARG_POINTER(0);

    // Generate output
    char *result = (char *)palloc(sizeof(char) * 64);
    _fxypty_out(result, decimal);

    PG_RETURN_CSTRING(result);
}

/// @brief Parse and validate a type modifier and generate an int32 to represent
/// it.
/// @param [[int32, int32]] refer to the number of maximum digits and the number
/// of maximum fractional digits.
/// @return The int32 representation of the type modifier.
Datum fxypty_typmod_in(PG_FUNCTION_ARGS) {
    ArrayType *ta = PG_GETARG_ARRAYTYPE_P(0);
    int n;
    int32 *tl = ArrayGetIntegerTypmods(ta, &n);
    int32 typmod;
    int32 number_of_digits = tl[0];
    int32 number_of_fractional_digits = tl[1];

    if (n == 2) {
        if (number_of_digits > 38 || number_of_digits < 0 ||
            number_of_digits - number_of_fractional_digits < 0) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("invalid type modifier")));
        }
        typmod = (number_of_digits << 16) | number_of_fractional_digits;
    } else {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("invalid type modifier")));
        typmod = 0;
    }

    PG_RETURN_INT32(typmod);
}

/// @brief Generate a string representing a type modifier in its int32
/// representation.
/// @param [int32] The int32 representation of the type modifier.
/// @return The string representation of the type modifier.
Datum fxypty_typmod_out(PG_FUNCTION_ARGS) {
    int32 typmod = PG_GETARG_INT32(0);
    int32 number_of_digits = (typmod >> 16) & 0xffff;
    int32 number_of_fractional_digits = typmod & 0xffff;
    char *result = (char *)palloc(sizeof(char) * 64);

    if (typmod == -1) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                        errmsg("invalid typmod for type %s", "fxypty")));
    }

    if (typmod >= 0) {
        snprintf(result, 64, "(%d,%d)", number_of_digits,
                 number_of_fractional_digits);
    } else {
        *result = '\0';
    }

    PG_RETURN_CSTRING(result);
}

/**
 * Operator functions
 */

/// @brief Add two fxypty objects.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return The sum of both the fxypty objects.
Datum fxypty_add(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    result = _fxypty_add(a, b);

    PG_RETURN_POINTER(result);
}

/// @brief Subtract two fxypty objects.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return The difference of the first object to the second object.
Datum fxypty_subtract(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    result = _fxypty_subtract(a, b);

    PG_RETURN_POINTER(result);
}

/// @brief Multiply two fxypty objects.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return The product of both the fxypty objects.
Datum fxypty_multiply(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    result = _fxypty_multiply(a, b);

    PG_RETURN_POINTER(result);
}

/// @brief Divide two fxypty objects.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return The quotient from dividing the first object by the second object.
Datum fxypty_divide(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    result = _fxypty_divide(a, b);

    PG_RETURN_POINTER(result);
}

/// @brief Check whether values of two fxypty objects are equivalent.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return true when both values are equivalent. Otherwise, false.
Datum fxypty_eq(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    int result = _fxypty_compare(a, b);

    PG_RETURN_BOOL(result == 0);
}

/// @brief Check whether values of two fxypty objects are not equivalent.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return true when both values are not equivalent. Otherwise, false.
Datum fxypty_neq(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    int result = _fxypty_compare(a, b);

    PG_RETURN_BOOL(result != 0);
}

/// @brief Check whether values of the first fxypty object is less than the
/// second fxypty object.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return true when the value of the first object is less than the second
/// object. Otherwise, false.
Datum fxypty_lt(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    int result = _fxypty_compare(a, b);

    PG_RETURN_BOOL(result < 0);
}

/// @brief Check whether values of the first fxypty object is greater than the
/// second fxypty object.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return true when the value of the first object is greater than the second
/// object. Otherwise, false.
Datum fxypty_gt(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    int result = _fxypty_compare(a, b);

    PG_RETURN_BOOL(result > 0);
}

/// @brief Check whether values of the first fxypty object is less than or equal
/// to the second fxypty object.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return true when the value of the first object is less than or equal to the
/// second object. Otherwise, false.
Datum fxypty_lte(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    int result = _fxypty_compare(a, b);

    PG_RETURN_BOOL(result <= 0);
}

/// @brief Check whether values of the first fxypty object is greater than or
/// equal to the second fxypty object.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return true when the value of the first object is greater than or equal to
/// the second object. Otherwise, false.
Datum fxypty_gte(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    int result = _fxypty_compare(a, b);

    PG_RETURN_BOOL(result >= 0);
}

/// @brief Get the smaller value between two fxypty objects.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return The pointer to the smaller value.
Datum fxypty_smaller(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    void *smaller = _fxypty_compare(a, b) < 0 ? a : b;

    PG_RETURN_POINTER(smaller);
}

/// @brief Get the larger value between two fxypty objects.
/// @param [fxypty, fxypty] refer the fxypty objects.
/// @return The pointer to the larger value.
Datum fxypty_larger(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);
    void *larger = _fxypty_compare(a, b) >= 0 ? a : b;

    PG_RETURN_POINTER(larger);
}