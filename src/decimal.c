// clang-format off
#include "../../src/include/postgres.h"
#include "../../src/include/fmgr.h"
// clang-format on

extern void *_decimal_in(char *input, uint32_t scale);
extern const char *_decimal_out(void *in);

// extern void *decimal_add_impl(void *a, void *b);
// extern void *decimal_sub_impl(void *a, void *b);
// extern void *decimal_mul_impl(void *a, void *b);
// extern void *decimal_div_impl(void *a, void *b);

// extern int decimal_cmp_impl(void *a, void *b);

PG_MODULE_MAGIC;

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(decimal_in);

Datum decimal_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);

    char input_buffer[64];
    uint32_t scale;
    void *result;

    if (sscanf(str, "( %s , %d )", input_buffer, &scale) != 2)
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                        errmsg("invalid input syntax for type %s: \"%s\"",
                               "decimal", str)));

    // FIXME: Try-catch
    // if scale is not input can set default value
    // uint32_t scale = Decimal::DEFAULT_SCALE;

    result = _decimal_in(input_buffer, scale);
    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(decimal_out);

Datum decimal_out(PG_FUNCTION_ARGS) {
    void *decimal = (void *)PG_GETARG_POINTER(0);
    
    // FIXME: Try-catch
    const char *result = _decimal_out(decimal);
    PG_RETURN_CSTRING(result);
}

// PG_FUNCTION_INFO_V1(decimal_add);

// Datum decimal_add(PG_FUNCTION_ARGS) {
//     void *result;

//     void *a = (void *)PG_GETARG_POINTER(0);
//     void *b = (void *)PG_GETARG_POINTER(1);

//     result = decimal_add_impl(a, b);

//     PG_RETURN_POINTER(result);
// }

// PG_FUNCTION_INFO_V1(decimal_sub);

// Datum decimal_sub(PG_FUNCTION_ARGS) {
//     void *result;

//     void *a = (void *)PG_GETARG_POINTER(0);
//     void *b = (void *)PG_GETARG_POINTER(1);

//     result = decimal_sub_impl(a, b);

//     PG_RETURN_POINTER(result);
// }

// PG_FUNCTION_INFO_V1(decimal_mul);

// Datum decimal_mul(PG_FUNCTION_ARGS) {
//     void *result;

//     void *a = (void *)PG_GETARG_POINTER(0);
//     void *b = (void *)PG_GETARG_POINTER(1);

//     result = decimal_mul_impl(a, b);

//     PG_RETURN_POINTER(result);
// }

// PG_FUNCTION_INFO_V1(decimal_div);

// Datum decimal_div(PG_FUNCTION_ARGS) {
//     void *result;

//     void *a = (void *)PG_GETARG_POINTER(0);
//     void *b = (void *)PG_GETARG_POINTER(1);

//     result = decimal_div_impl(a, b);

//     PG_RETURN_POINTER(result);
// }

// PG_FUNCTION_INFO_V1(decimal_eq);

// Datum decimal_eq(PG_FUNCTION_ARGS) {
//     void *a = (void *)PG_GETARG_POINTER(0);
//     void *b = (void *)PG_GETARG_POINTER(1);

//     int diff = decimal_cmp_impl(a, b);

//     PG_RETURN_BOOL(diff == 0);
// }

// PG_FUNCTION_INFO_V1(decimal_ne);

// Datum decimal_ne(PG_FUNCTION_ARGS) {
//     void *a = (void *)PG_GETARG_POINTER(0);
//     void *b = (void *)PG_GETARG_POINTER(1);

//     int diff = decimal_cmp_impl(a, b);

//     PG_RETURN_BOOL(diff != 0);
// }

// PG_FUNCTION_INFO_V1(decimal_lt);

// Datum decimal_lt(PG_FUNCTION_ARGS) {
//     void *a = (void *)PG_GETARG_POINTER(0);
//     void *b = (void *)PG_GETARG_POINTER(1);

//     int diff = decimal_cmp_impl(a, b);

//     PG_RETURN_BOOL(diff < 0);
// }

// PG_FUNCTION_INFO_V1(decimal_gt);

// Datum decimal_gt(PG_FUNCTION_ARGS) {
//     void *a = (void *)PG_GETARG_POINTER(0);
//     void *b = (void *)PG_GETARG_POINTER(1);

//     int diff = decimal_cmp_impl(a, b);

//     PG_RETURN_BOOL(diff > 0);
// }

// PG_FUNCTION_INFO_V1(decimal_le);

// Datum decimal_le(PG_FUNCTION_ARGS) {
//     void *a = (void *)PG_GETARG_POINTER(0);
//     void *b = (void *)PG_GETARG_POINTER(1);

//     int diff = decimal_cmp_impl(a, b);

//     PG_RETURN_BOOL(diff <= 0);
// }

// PG_FUNCTION_INFO_V1(decimal_ge);

// Datum decimal_ge(PG_FUNCTION_ARGS) {
//     void *a = (void *)PG_GETARG_POINTER(0);
//     void *b = (void *)PG_GETARG_POINTER(1);

//     int diff = decimal_cmp_impl(a, b);

//     PG_RETURN_BOOL(diff >= 0);
// }

// PG_FUNCTION_INFO_V1(decimal_cmp);

// Datum decimal_cmp(PG_FUNCTION_ARGS) {
//     void *a = (void *)PG_GETARG_POINTER(0);
//     void *b = (void *)PG_GETARG_POINTER(1);

//     int diff = decimal_cmp_impl(a, b);

//     PG_RETURN_INT32(diff);
// }
