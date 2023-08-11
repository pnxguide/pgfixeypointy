#define NUMERIC_MAX_PRECISION 1000

#define NUMERIC_MIN_SCALE (-1000)
#define NUMERIC_MAX_SCALE 1000

/*
 * Internal limits on the scales chosen for calculation results
 */
#define NUMERIC_MAX_DISPLAY_SCALE NUMERIC_MAX_PRECISION
#define NUMERIC_MIN_DISPLAY_SCALE 0

#define NUMERIC_MAX_RESULT_SCALE (NUMERIC_MAX_PRECISION * 2)

/*
 * For inherently inexact calculations such as division and square root,
 * we try to get at least this many significant digits; the idea is to
 * deliver a result no worse than float8 would.
 */
#define NUMERIC_MIN_SIG_DIGITS 16
#define FLEXIBLE_ARRAY_MEMBER 64

/* The actual contents of Numeric are private to numeric.c */
struct NumericData;
typedef struct NumericData *Numeric;

#include "define.h"

#define palloc malloc
#define pfree free

extern Numeric int64_to_numeric(int64 val);
void print_numeric(Numeric x);
Numeric numeric_in(const char *str);

void Numeric_ToString(Numeric x);

/*
 * Utility functions in numeric.c
 */
extern Numeric numeric_add_opt_error(Numeric num1, Numeric num2,
                                     bool *have_error);
extern Numeric numeric_sub_opt_error(Numeric num1, Numeric num2,
                                     bool *have_error);
extern Numeric numeric_mul_opt_error(Numeric num1, Numeric num2,
									 bool *have_error);
extern Numeric numeric_div_opt_error(Numeric num1, Numeric num2,
									 bool *have_error);