#define _GNU_SOURCE
#include <stdio.h>
#include <postgres.h>
#include <lib/stringinfo.h>
#include <libpq/pqformat.h>
#include <fmgr.h>
#include <unicode/unum.h>

#define TYPE_bit uint8_t
#define CAST_bit(x) (uint8_t)(x)
#define GETARG_bit(x) PG_GETARG_CHAR(x)
#define RETURN_bit(x) PG_RETURN_CHAR(x)
#define PARSE_bit(fmt, text, len, pos, status) unum_parse(fmt, text, len, pos, status)

#define TYPE_tinyint uint8_t
#define CAST_tinyint(x) (uint8_t)(x)
#define GETARG_tinyint(x) PG_GETARG_CHAR(x)
#define RETURN_tinyint(x) PG_RETURN_CHAR(x)
#define PARSE_tinyint(fmt, text, len, pos, status) unum_parse(fmt, text, len, pos, status)

#define TYPE_smallint int16_t
#define CAST_smallint(x) (int16_t)(x)
#define GETARG_smallint(x) PG_GETARG_INT16(x)
#define RETURN_smallint(x) PG_RETURN_INT16(x)
#define PARSE_smallint(fmt, text, len, pos, status) unum_parse(fmt, text, len, pos, status)

#define TYPE_int int32_t
#define CAST_int(x) (int32_t)(x)
#define GETARG_int(x) PG_GETARG_INT32(x)
#define RETURN_int(x) PG_RETURN_INT32(x)
#define PARSE_int(fmt, text, len, pos, status) unum_parse(fmt, text, len, pos, status)

#define TYPE_bigint int64_t
#define CAST_bigint(x) (int64_t)(x)
#define GETARG_bigint(x) *(int64_t *)PG_GETARG_POINTER(x)
#define RETURN_bigint(x) do { int64_t *ret = palloc(sizeof(int64_t)); *ret = (x); PG_RETURN_POINTER(ret); } while(0)
#define PARSE_bigint(fmt, text, len, pos, status) unum_parseInt64(fmt, text, len, pos, status)

#define TYPE_real float
#define CAST_real(x) (float)(x)
#define GETARG_real(x) PG_GETARG_FLOAT4(x)
#define RETURN_real(x) PG_RETURN_FLOAT4(x)
#define PARSE_real(fmt, text, len, pos, status) unum_parseDouble(fmt, text, len, pos, status)

#define TYPE_float double
#define CAST_float(x) (double)(x)
#define GETARG_float(x) *(double *)PG_GETARG_POINTER(x)
#define RETURN_float(x) do { double *ret = palloc(sizeof(double)); *ret = (x); PG_RETURN_POINTER(ret); } while(0)
#define PARSE_float(fmt, text, len, pos, status) unum_parseDouble(fmt, text, len, pos, status)

