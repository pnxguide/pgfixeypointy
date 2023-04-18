// clang-format off
#include "../../src/include/postgres.h"
#include "../../src/include/fmgr.h"
// clang-format on

extern void *decimal_in_impl(char *input, uint8_t scale);
extern const char *decimal_out_impl(void *in);

extern void *decimal_add_impl(void *a, void *b);
extern void *decimal_sub_impl(void *a, void *b);
extern void *decimal_mul_impl(void *a, void *b);
extern void *decimal_mul_const_impl(void *a, int64_t b);
extern void *decimal_div_impl(void *a, void *b);
extern void *decimal_div_const_impl(void *a, int64_t b);
extern void *const_div_decimal_impl(int64_t a, void *b);

extern int decimal_cmp_impl(void *a, void *b);

PG_MODULE_MAGIC;

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(decimal_in);

Datum decimal_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);

    char *input;
    uint32_t scale;
    void *result;

    if (sscanf(str, " ( %s , %d )", &input, &scale) != 2)
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                        errmsg("invalid input syntax for type %s: \"%s\"",
                               "decimal", str)));

    // FIXME: Try-catch
    // if scale is not input can set default value
    // uint32_t scale = Decimal::DEFAULT_SCALE;

    result = decimal_in_impl(input, scale);
    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(decimal_out);

Datum decimal_out(PG_FUNCTION_ARGS) {
    void *decimal = (void *)PG_GETARG_POINTER(0);
    
    // FIXME: Try-catch
    char *result = decimal_out_impl(decimal);
    PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(decimal_add);

Datum decimal_add(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    result = decimal_add_impl(a, b);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(decimal_sub);

Datum decimal_sub(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    result = decimal_sub_impl(a, b);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(decimal_mul);

Datum decimal_mul(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    result = decimal_mul_impl(a, b);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(int4_decimal_mul);

Datum int4_decimal_mul(PG_FUNCTION_ARGS) {
    void *result;

    int32_t a = PG_GETARG_INT32(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    result = decimal_mul_const_impl(b, a);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(int8_decimal_mul);

Datum int8_decimal_mul(PG_FUNCTION_ARGS) {
    void *result;

    int64_t a = PG_GETARG_INT64(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    result = decimal_mul_const_impl(b, a);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(decimal_mul_int4);

Datum decimal_mul_int4(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    int32_t *b = PG_GETARG_INT32(1);

    result = decimal_mul_const_impl(a, b);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(decimal_mul_int8);

Datum decimal_mul_int8(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    int64_t *b = PG_GETARG_INT64(1);

    result = decimal_mul_const_impl(a, b);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(decimal_div);

Datum decimal_div(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    result = decimal_div_impl(a, b);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(int4_decimal_div);

Datum int4_decimal_div(PG_FUNCTION_ARGS) {
    void *result;

    int32_t a = PG_GETARG_INT32(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    result = const_div_decimal_impl(a, b);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(int8_decimal_div);

Datum int8_decimal_div(PG_FUNCTION_ARGS) {
    void *result;

    int64_t a = PG_GETARG_INT64(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    result = const_div_decimal_impl(a, b);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(decimal_div_int4);

Datum decimal_div_int4(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    int32_t *b = PG_GETARG_INT32(1);

    result = decimal_div_const_impl(a, b);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(decimal_div_int8);

Datum decimal_div_int8(PG_FUNCTION_ARGS) {
    void *result;

    void *a = (void *)PG_GETARG_POINTER(0);
    int64_t *b = PG_GETARG_INT64(1);

    result = decimal_div_const_impl(a, b);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(decimal_eq);

Datum decimal_eq(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    int diff = decimal_cmp_impl(a, b);

    PG_RETURN_BOOL(diff == 0);
}

PG_FUNCTION_INFO_V1(decimal_ne);

Datum decimal_ne(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    int diff = decimal_cmp_impl(a, b);

    PG_RETURN_BOOL(diff != 0);
}

PG_FUNCTION_INFO_V1(decimal_lt);

Datum decimal_lt(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    int diff = decimal_cmp_impl(a, b);

    PG_RETURN_BOOL(diff < 0);
}

PG_FUNCTION_INFO_V1(decimal_gt);

Datum decimal_gt(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    int diff = decimal_cmp_impl(a, b);

    PG_RETURN_BOOL(diff > 0);
}

PG_FUNCTION_INFO_V1(decimal_le);

Datum decimal_le(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    int diff = decimal_cmp_impl(a, b);

    PG_RETURN_BOOL(diff <= 0);
}

PG_FUNCTION_INFO_V1(decimal_ge);

Datum decimal_ge(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    int diff = decimal_cmp_impl(a, b);

    PG_RETURN_BOOL(diff >= 0);
}

PG_FUNCTION_INFO_V1(decimal_cmp);

Datum decimal_cmp(PG_FUNCTION_ARGS) {
    void *a = (void *)PG_GETARG_POINTER(0);
    void *b = (void *)PG_GETARG_POINTER(1);

    int diff = decimal_cmp_impl(a, b);

    PG_RETURN_INT32(diff);
}