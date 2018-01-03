#include <postgres.h>
#include <catalog/pg_type.h>
#include <utils/array.h>
#include <utils/builtins.h>
#include <fmgr.h>
#include <string.h>

PG_FUNCTION_INFO_V1(tsql_varbinary_in);
PG_FUNCTION_INFO_V1(tsql_varbinary_out);
PG_FUNCTION_INFO_V1(tsql_varbinary_typmod_in);
PG_FUNCTION_INFO_V1(tsql_varbinary_typmod_out);
PG_FUNCTION_INFO_V1(tsql_varbinary_recv);
PG_FUNCTION_INFO_V1(tsql_varbinary_send);
PG_FUNCTION_INFO_V1(tsql_cast_varbinary_varbinary);
PG_FUNCTION_INFO_V1(tsql_varbinary_eq_varbinary);
PG_FUNCTION_INFO_V1(tsql_varbinary_ne_varbinary);
PG_FUNCTION_INFO_V1(tsql_varbinary_lt_varbinary);
PG_FUNCTION_INFO_V1(tsql_varbinary_le_varbinary);
PG_FUNCTION_INFO_V1(tsql_varbinary_gt_varbinary);
PG_FUNCTION_INFO_V1(tsql_varbinary_ge_varbinary);
PG_FUNCTION_INFO_V1(tsql_varbinary_cmp);
PG_FUNCTION_INFO_V1(tsql_varbinary_pl_varbinary);
PG_FUNCTION_INFO_V1(tsql_varbinary_coalesce);

static int32_t
varbinary_cmp(char *l, char *r) {
  int cmp;
  int len;

  if (VARSIZE(l) < VARSIZE(r))
    len = VARSIZE(l) - VARHDRSZ;
  else
    len = VARSIZE(r) - VARHDRSZ;

  cmp = memcmp(l + VARHDRSZ, r + VARHDRSZ, len);
  if (cmp < 0) {
    return -1;
  } else if (cmp > 0) {
    return 1;
  } else if (VARSIZE(l) < VARSIZE(r)) {
    return -1;
  } else if (VARSIZE(l) > VARSIZE(r)) {
    return 1;
  } else {
    return 0;
  }
}

Datum
tsql_varbinary_in(PG_FUNCTION_ARGS) {
  char *in = PG_GETARG_CSTRING(0);
  int32_t len = PG_GETARG_INT32(2);
  char *out;

  // TODO translate 0xXXXX string into bytes
  if (len == -1 || len > strlen(in)) {
    len = strlen(in);
  } else {
    // TODO issue a warning or an error?
  }

  out = palloc(VARHDRSZ + len);
  memcpy(out + VARHDRSZ, in, len);
  SET_VARSIZE(out, VARHDRSZ + len);
  PG_RETURN_POINTER(out);
}

Datum
tsql_varbinary_out(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));

  char *out = palloc(VARSIZE(in) - VARHDRSZ + 1);
  memcpy(out, in + VARHDRSZ, VARSIZE(in) - VARHDRSZ);
  out[VARSIZE(in) - VARHDRSZ] = 0;

  PG_RETURN_CSTRING(out);
}

Datum
tsql_varbinary_typmod_in(PG_FUNCTION_ARGS) {
  ArrayType *in = PG_GETARG_ARRAYTYPE_P(0);
  Datum *elems;
  int elem_count;
  int32_t out;

  deconstruct_array(in, CSTRINGOID, -2, false, 'c', &elems, NULL, &elem_count);
  if ((strcasecmp(DatumGetCString(elems[0]), "max")) == 0) {
    out = 0x7FFFFFFF;
  } else {
    out = pg_atoi(DatumGetCString(elems[0]), sizeof(int32), '\0');
    /* check that <= 8000 */
  }
  PG_RETURN_INT32(out);
}

Datum
tsql_varbinary_typmod_out(PG_FUNCTION_ARGS) {
  int32_t len = PG_GETARG_INT32(0);
  char *out = palloc(7);

  if (len != 0x7FFFFFFF) {
    snprintf(out, 6, "(%d)", len);
  } else {
    strcpy(out, "(max)");
  }
  PG_RETURN_CSTRING(out);
}

Datum
tsql_varbinary_recv(PG_FUNCTION_ARGS) {
  char *in = PG_GETARG_POINTER(0);
  char *out;
// TODO takes a typmod argument - use it

  out = palloc(VARSIZE(in));
  memcpy(out, in, VARSIZE(in));
  PG_RETURN_POINTER(out);
}

Datum
tsql_varbinary_send(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *out;

  out = palloc(VARSIZE(in));
  memcpy(out, in, VARSIZE(in));
  PG_RETURN_POINTER(out);
}

Datum
tsql_cast_varbinary_varbinary(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  int32_t len = PG_GETARG_INT32(1);
  char *out;

  if (len == -1 || len > VARSIZE(in) - VARHDRSZ) {
    len = VARSIZE(in) - VARHDRSZ;
  } else {
    // TODO issue a warning or an error?
  }
  out = palloc(VARHDRSZ + len);
  memcpy(out + VARHDRSZ, in + VARHDRSZ, len);
  SET_VARSIZE(out, VARHDRSZ + len);
  PG_RETURN_POINTER(out);
}

Datum
tsql_varbinary_eq_varbinary(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varbinary_cmp(l, r) == 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varbinary_ne_varbinary(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varbinary_cmp(l, r) != 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varbinary_lt_varbinary(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varbinary_cmp(l, r) < 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varbinary_le_varbinary(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varbinary_cmp(l, r) <= 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varbinary_gt_varbinary(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varbinary_cmp(l, r) > 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varbinary_ge_varbinary(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varbinary_cmp(l, r) >= 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varbinary_cmp(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  PG_RETURN_INT32(varbinary_cmp(l, r));
}

Datum
tsql_varbinary_pl_varbinary(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  char *out = palloc(VARHDRSZ + VARSIZE(l) - VARHDRSZ + VARSIZE(r) - VARHDRSZ);
  SET_VARSIZE(out, VARHDRSZ + VARSIZE(l) - VARHDRSZ + VARSIZE(r) - VARHDRSZ);
  memcpy(out + VARHDRSZ, l + VARHDRSZ, VARSIZE(l) - VARHDRSZ);
  memcpy(out + VARSIZE(l), r + VARHDRSZ, VARSIZE(r) - VARHDRSZ);
  PG_RETURN_POINTER(out);
}

Datum
tsql_varbinary_max(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varbinary_cmp(r, l) > 0) {
    PG_RETURN_POINTER(r);
  } else {
    PG_RETURN_POINTER(l);
  }
}

Datum
tsql_varbinary_coalesce(PG_FUNCTION_ARGS) {
  // TODO if arg0 is null then return arg1
  char *out;

  out = palloc(VARHDRSZ + 2);
  SET_VARSIZE(out, VARHDRSZ + 2);
  out[VARHDRSZ + 0] = '4';
  out[VARHDRSZ + 1] = '2';
  
  PG_RETURN_POINTER(out);
}

