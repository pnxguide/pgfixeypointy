// clang-format off
#include "../../src/include/postgres.h"
#include "../../src/include/fmgr.h"
// clang-format on

extern void *decimal_in_impl(char *input, uint8_t scale);
extern const char *decimal_out_impl(void *in);

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
