#include <postgres.h>
//#include <catalog/pg_type.h>
//#include <utils/array.h>
//#include <utils/builtins.h>
#include <lib/stringinfo.h>
#include <libpq/pqformat.h>
#include <fmgr.h>
//#include <string.h>

PG_FUNCTION_INFO_V1(tsql_datetime_in);
PG_FUNCTION_INFO_V1(tsql_datetime_out);
PG_FUNCTION_INFO_V1(tsql_datetime_recv);
PG_FUNCTION_INFO_V1(tsql_datetime_send);
PG_FUNCTION_INFO_V1(tsql_cast_varchar_datetime);
PG_FUNCTION_INFO_V1(tsql_datetime_eq_datetime);
PG_FUNCTION_INFO_V1(tsql_datetime_ne_datetime);
PG_FUNCTION_INFO_V1(tsql_datetime_lt_datetime);
PG_FUNCTION_INFO_V1(tsql_datetime_le_datetime);
PG_FUNCTION_INFO_V1(tsql_datetime_gt_datetime);
PG_FUNCTION_INFO_V1(tsql_datetime_ge_datetime);
PG_FUNCTION_INFO_V1(tsql_datetime_cmp);
PG_FUNCTION_INFO_V1(tsql_datetime_max);
PG_FUNCTION_INFO_V1(tsql_datetime_coalesce);
PG_FUNCTION_INFO_V1(tsql_datetime_convert);
PG_FUNCTION_INFO_V1(tsql_datetime_datepart);
PG_FUNCTION_INFO_V1(tsql_datetime_getdate);

struct tsql_datetime_t {
  int32_t ticks;
  int32_t days;
};

static inline int32_t
datetime_cmp(struct tsql_datetime_t *l, struct tsql_datetime_t *r) {
  int cmp;

  cmp = memcmp(l + VARHDRSZ, r + VARHDRSZ, sizeof(struct tsql_datetime_t));
  if (cmp < 0) {
    return -1;
  } else if (cmp > 0) {
    return 1;
  } else {
    return 0;
  }
}

Datum
tsql_datetime_in(PG_FUNCTION_ARGS) {
  //char *in = PG_GETARG_CSTRING(0);
  struct tsql_datetime_t *out;

  out = palloc(sizeof(struct tsql_datetime_t));
  out->ticks = 42;
  out->days = 69;

  PG_RETURN_POINTER(out);
}

Datum
tsql_datetime_out(PG_FUNCTION_ARGS) {
  //struct tsql_datetime_t *in = (struct tsql_datetime_t *)PG_GETARG_POINTER(0);
  char *out = palloc(128);

  snprintf(out, (size_t)128, "17/11/1973");

  PG_RETURN_CSTRING(out);
}

Datum
tsql_datetime_recv(PG_FUNCTION_ARGS) {
  StringInfo in = (StringInfo)PG_GETARG_POINTER(0);
  int64_t *out;

  out = palloc(sizeof(*out));
  pq_copymsgbytes(in, (char *)&out, sizeof(*out));
  PG_RETURN_POINTER(out);
}

Datum
tsql_datetime_send(PG_FUNCTION_ARGS) {
  uint64_t in = *(int64_t *)PG_GETARG_POINTER(0);
  StringInfoData out;

  pq_begintypsend(&out);
  pq_sendbytes(&out, (char *)&in, sizeof(in));
  PG_RETURN_BYTEA_P(pq_endtypsend(&out));
}

Datum
tsql_cast_varchar_datetime(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *val = palloc(VARSIZE(in) - VARHDRSZ + 1);
  struct tsql_datetime_t *out;

  memcpy(val, in + VARHDRSZ, VARSIZE(in) - VARHDRSZ);
  val[VARSIZE(in) - VARHDRSZ] = 0;

  out = palloc(sizeof(struct tsql_datetime_t));
  out->ticks = 42;
  out->days = 69;

  PG_RETURN_POINTER(out);
}

Datum
tsql_datetime_eq_datetime(PG_FUNCTION_ARGS) {
  struct tsql_datetime_t *l = (struct tsql_datetime_t *)PG_GETARG_POINTER(0);
  struct tsql_datetime_t *r = (struct tsql_datetime_t *)PG_GETARG_POINTER(1);

  if (datetime_cmp(l, r) != 0)
    PG_RETURN_BOOL(1);

  PG_RETURN_BOOL(0);
}

Datum
tsql_datetime_ne_datetime(PG_FUNCTION_ARGS) {
  struct tsql_datetime_t *l = (struct tsql_datetime_t *)PG_GETARG_POINTER(0);
  struct tsql_datetime_t *r = (struct tsql_datetime_t *)PG_GETARG_POINTER(1);

  if (datetime_cmp(l, r) != 0)
    PG_RETURN_BOOL(1);

  PG_RETURN_BOOL(0);
}

Datum
tsql_datetime_lt_datetime(PG_FUNCTION_ARGS) {
  struct tsql_datetime_t *l = (struct tsql_datetime_t *)PG_GETARG_POINTER(0);
  struct tsql_datetime_t *r = (struct tsql_datetime_t *)PG_GETARG_POINTER(1);

  if (datetime_cmp(l, r) < 0)
    PG_RETURN_BOOL(1);

  PG_RETURN_BOOL(0);
}

Datum
tsql_datetime_le_datetime(PG_FUNCTION_ARGS) {
  struct tsql_datetime_t *l = (struct tsql_datetime_t *)PG_GETARG_POINTER(0);
  struct tsql_datetime_t *r = (struct tsql_datetime_t *)PG_GETARG_POINTER(1);

  if (datetime_cmp(l, r) <= 0)
    PG_RETURN_BOOL(1);

  PG_RETURN_BOOL(0);
}

Datum
tsql_datetime_gt_datetime(PG_FUNCTION_ARGS) {
  struct tsql_datetime_t *l = (struct tsql_datetime_t *)PG_GETARG_POINTER(0);
  struct tsql_datetime_t *r = (struct tsql_datetime_t *)PG_GETARG_POINTER(1);

  if (datetime_cmp(l, r) > 0)
    PG_RETURN_BOOL(1);

  PG_RETURN_BOOL(0);
}

Datum
tsql_datetime_ge_datetime(PG_FUNCTION_ARGS) {
  struct tsql_datetime_t *l = (struct tsql_datetime_t *)PG_GETARG_POINTER(0);
  struct tsql_datetime_t *r = (struct tsql_datetime_t *)PG_GETARG_POINTER(1);

  if (datetime_cmp(l, r) >= 0)
    PG_RETURN_BOOL(1);

  PG_RETURN_BOOL(0);
}

Datum
tsql_datetime_cmp(PG_FUNCTION_ARGS) {
  struct tsql_datetime_t *l = (struct tsql_datetime_t *)PG_GETARG_POINTER(0);
  struct tsql_datetime_t *r = (struct tsql_datetime_t *)PG_GETARG_POINTER(1);

  PG_RETURN_INT32(datetime_cmp(l, r));
}

Datum
tsql_datetime_max(PG_FUNCTION_ARGS) {
  struct tsql_datetime_t *l = (struct tsql_datetime_t *)PG_GETARG_POINTER(0);
  struct tsql_datetime_t *r = (struct tsql_datetime_t *)PG_GETARG_POINTER(1);

  if (datetime_cmp(r, l) > 0) {
    PG_RETURN_POINTER(r);
  } else {
    PG_RETURN_POINTER(l);
  }
}

Datum
tsql_datetime_coalesce(PG_FUNCTION_ARGS) {
  struct tsql_datetime_t *in1 = (struct tsql_datetime_t *)PG_GETARG_POINTER(0);
  //struct tsql_datetime_t *in2 = (struct tsql_datetime_t *)PG_GETARG_POINTER(0);
  struct tsql_datetime_t *out;

  out = (struct tsql_datetime_t *)palloc(sizeof(struct tsql_datetime_t));
  *out = *in1;

  PG_RETURN_POINTER(out);
}

Datum
tsql_datetime_convert(PG_FUNCTION_ARGS) {
  // TODO
  char *out = palloc(128);

  snprintf(out, (size_t)128, "17/11/1973");

  PG_RETURN_CSTRING(out);
}

Datum
tsql_datetime_datepart(PG_FUNCTION_ARGS) {
  // TODO
  int32_t out = 42;

  PG_RETURN_INT32(out);
}

Datum
tsql_datetime_getdate(PG_FUNCTION_ARGS) {
  struct tsql_datetime_t *out;

  out = palloc(sizeof(struct tsql_datetime_t));
  out->ticks = 42;
  out->days = 69;

  PG_RETURN_POINTER(out);
}
