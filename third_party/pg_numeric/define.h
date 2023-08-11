#define uint8 unsigned short
#define uint32 unsigned int
#define FLEXIBLE_ARRAY_MEMBER 64
#define false 0
#define true 1
#define bool int
#define uint16 unsigned short
#define int16 short
#define int32 int
#define uint64 unsigned long
#define Size unsigned long
#define int64 long
#define INT64CONST(x) (x##L)
#define PG_INT64_MAX INT64CONST(0x7FFFFFFFFFFFFFFF)
#define PG_INT32_MAX (0x7FFFFFFF)
#define unlikely(x) __builtin_expect((x) != 0, 0)
#define HIGHBIT (0x80)
#define IS_HIGHBIT_SET(ch)((unsigned char)(ch)&HIGHBIT)

#define Max(x, y) ((x) > (y) ? (x) : (y))
#define Min(x, y) ((x) < (y) ? (x) : (y))