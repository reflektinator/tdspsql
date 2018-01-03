#define _GNU_SOURCE
#include <postgres.h>
#include <catalog/pg_type.h>
#include <utils/array.h>
#include <utils/builtins.h>
#include <lib/stringinfo.h>
#include <libpq/pqformat.h>
#include <fmgr.h>
#include <string.h>
#include <stdio.h>

PG_FUNCTION_INFO_V1(tsql_varchar_in);
PG_FUNCTION_INFO_V1(tsql_varchar_out);
PG_FUNCTION_INFO_V1(tsql_varchar_typmod_in);
PG_FUNCTION_INFO_V1(tsql_varchar_typmod_out);
PG_FUNCTION_INFO_V1(tsql_varchar_recv);
PG_FUNCTION_INFO_V1(tsql_varchar_send);
PG_FUNCTION_INFO_V1(tsql_cast_uniqueidentifier_varchar);
PG_FUNCTION_INFO_V1(tsql_cast_varchar_varchar);
//PG_FUNCTION_INFO_V1(tsql_cast_varchar_int);
//PG_FUNCTION_INFO_V1(tsql_cast_float_varchar);
PG_FUNCTION_INFO_V1(tsql_varchar_eq_varchar);
PG_FUNCTION_INFO_V1(tsql_varchar_ne_varchar);
PG_FUNCTION_INFO_V1(tsql_varchar_lt_varchar);
PG_FUNCTION_INFO_V1(tsql_varchar_le_varchar);
PG_FUNCTION_INFO_V1(tsql_varchar_gt_varchar);
PG_FUNCTION_INFO_V1(tsql_varchar_ge_varchar);
PG_FUNCTION_INFO_V1(tsql_varchar_cmp);
PG_FUNCTION_INFO_V1(tsql_varchar_pl_varchar);
PG_FUNCTION_INFO_V1(tsql_varchar_max);
PG_FUNCTION_INFO_V1(tsql_varchar_upper);
PG_FUNCTION_INFO_V1(tsql_varchar_lower);
PG_FUNCTION_INFO_V1(tsql_varchar_ltrim);
PG_FUNCTION_INFO_V1(tsql_varchar_rtrim);
PG_FUNCTION_INFO_V1(tsql_varchar_left);
PG_FUNCTION_INFO_V1(tsql_varchar_len);
PG_FUNCTION_INFO_V1(tsql_varchar_replace);
PG_FUNCTION_INFO_V1(tsql_varchar_coalesce);

static int32_t
varchar_cmp(char *l, char *r) {
  int cmp;
  int len;

  if (VARSIZE(l) < VARSIZE(r))
    len = VARSIZE(l) - VARHDRSZ;
  else
    len = VARSIZE(r) - VARHDRSZ;

  /* collation?? */
  cmp = strncasecmp(l + VARHDRSZ, r + VARHDRSZ, len);
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
tsql_varchar_in(PG_FUNCTION_ARGS) {
  char *in = PG_GETARG_CSTRING(0);
  int32_t len = PG_GETARG_INT32(2);
  char *out;

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
tsql_varchar_out(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));

  char *out = palloc(VARSIZE(in) - VARHDRSZ + 1);
  memcpy(out, in + VARHDRSZ, VARSIZE(in) - VARHDRSZ);
  out[VARSIZE(in) - VARHDRSZ] = 0;

  PG_RETURN_CSTRING(out);
}

Datum
tsql_varchar_typmod_in(PG_FUNCTION_ARGS) {
  ArrayType *in = PG_GETARG_ARRAYTYPE_P(0);
  Datum *elems;
  int elem_count;
  int32_t out;

  // TODO - maybe char is varchar with a high bit set in typmod?
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
tsql_varchar_typmod_out(PG_FUNCTION_ARGS) {
  int32_t len = PG_GETARG_INT32(0);
  int ret;
  char *out = palloc(7);

  if (len != 0x7FFFFFFF) {
    ret = snprintf(out, 6, "(%d)", len);
    if (ret < 0 || ret > 6) {
      strcpy(out, "(xxx)");
    }
  } else {
    strcpy(out, "(max)");
  }
  PG_RETURN_CSTRING(out);
}

Datum
tsql_varchar_recv(PG_FUNCTION_ARGS) {
  StringInfo in = (StringInfo)PG_GETARG_POINTER(0);
  char *out;

  out = palloc(VARHDRSZ + in->len);
  pq_copymsgbytes(in, out + VARHDRSZ, in->len);
//asprintf(&msg, "len = %d, %02hhx %02hhx %02hhx %02hhx %02hhx\n", in->len, out[VARHDRSZ+0],  out[VARHDRSZ+1],  out[VARHDRSZ+2],  out[VARHDRSZ+3],  out[VARHDRSZ+4]);
//elog(WARNING, msg);
  SET_VARSIZE(out, VARHDRSZ + in->len);
  PG_RETURN_POINTER(out);
}

Datum
tsql_varchar_send(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  StringInfoData out;

  pq_begintypsend(&out);
  pq_sendbytes(&out, in + VARHDRSZ, VARSIZE(in) - VARHDRSZ);
  PG_RETURN_BYTEA_P(pq_endtypsend(&out));
}

Datum
tsql_cast_varchar_varchar(PG_FUNCTION_ARGS) {
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
tsql_cast_uniqueidentifier_varchar(PG_FUNCTION_ARGS) {
  char *in = PG_GETARG_POINTER(0);
  char *out = palloc(VARHDRSZ + 36);

  sprintf(out + VARHDRSZ, "%08X-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
    *(uint32_t *)(in + 0),
    *(uint16_t *)(in + 4), *(uint16_t *)(in + 6),
    *(uint8_t *)(in + 8), *(uint8_t *)(in + 9),
    *(uint8_t *)(in + 10), *(uint8_t *)(in + 11),
    *(uint8_t *)(in + 12), *(uint8_t *)(in + 13),
    *(uint8_t *)(in + 14), *(uint8_t *)(in + 15));
  SET_VARSIZE(out, VARHDRSZ + 36);
  PG_RETURN_POINTER(out);
}

#if 0
Datum
tsql_cast_int_varchar(PG_FUNCTION_ARGS) {
  int32_t in = PG_GETARG_INT32(0);
  char *out;
  size_t len;

  len = snprintf(NULL, 0, "%d", in);
  out = palloc(VARHDRSZ + len);
  sprintf(out + VARHDRSZ, "%d", in);
  SET_VARSIZE(out, VARHDRSZ + len);
  PG_RETURN_POINTER(out);
}

Datum
tsql_cast_varchar_int(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *in2 = palloc(VARSIZE(in) - VARHDRSZ + 1);
  int32_t out;

  memcpy(in2, in + VARHDRSZ, VARSIZE(in) - VARHDRSZ);
  in2[VARSIZE(in) - VARHDRSZ] = 0;
  out = atoi(in2);
  PG_RETURN_INT32(out);
}

Datum
tsql_cast_float_varchar(PG_FUNCTION_ARGS) {
  double in = PG_GETARG_FLOAT8(0);
  char *out;
  size_t len;

  len = snprintf(NULL, 0, "%f", in);
  out = palloc(VARHDRSZ + len);
  sprintf(out + VARHDRSZ, "%f", in);
  SET_VARSIZE(out, VARHDRSZ + len);
  PG_RETURN_POINTER(out);
}
#endif

Datum
tsql_varchar_eq_varchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varchar_cmp(l, r) == 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varchar_ne_varchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varchar_cmp(l, r) != 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varchar_lt_varchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varchar_cmp(l, r) < 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varchar_le_varchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varchar_cmp(l, r) <= 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varchar_gt_varchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varchar_cmp(l, r) > 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varchar_ge_varchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varchar_cmp(l, r) >= 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_varchar_cmp(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  PG_RETURN_INT32(varchar_cmp(l, r));
}

Datum
tsql_varchar_pl_varchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  char *out = palloc(VARHDRSZ + VARSIZE(l) - VARHDRSZ + VARSIZE(r) - VARHDRSZ);
  SET_VARSIZE(out, VARHDRSZ + VARSIZE(l) - VARHDRSZ + VARSIZE(r) - VARHDRSZ);
  memcpy(out + VARHDRSZ, l + VARHDRSZ, VARSIZE(l) - VARHDRSZ);
  memcpy(out + VARSIZE(l), r + VARHDRSZ, VARSIZE(r) - VARHDRSZ);
  PG_RETURN_POINTER(out);
}

Datum
tsql_varchar_max(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (varchar_cmp(r, l) > 0) {
    PG_RETURN_POINTER(r);
  } else {
    PG_RETURN_POINTER(l);
  }
}

Datum
tsql_varchar_upper(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *out;
  int i;

  out = palloc(VARSIZE(in));
  SET_VARSIZE(out, VARSIZE(in));
  for (i = VARHDRSZ; i < VARSIZE(in); i++) {
    out[i] = toupper(in[i]);
  }
  PG_RETURN_POINTER(out);
}

Datum
tsql_varchar_lower(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *out;
  int i;

  out = palloc(VARSIZE(in));
  SET_VARSIZE(out, VARSIZE(in));
  for (i = VARHDRSZ; i < VARSIZE(in); i++) {
    out[i] = tolower(in[i]);
  }
  PG_RETURN_POINTER(out);
}

Datum
tsql_varchar_ltrim(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *out;
  int i;

  for (i = VARHDRSZ; i < VARSIZE(in); i++) {
    if (in[i] != ' ')
      break;
  }
  out = palloc(VARSIZE(in) - i);
  SET_VARSIZE(out, VARSIZE(in) - i);
  memcpy(out + VARHDRSZ, in + i, VARSIZE(in) - i); 
  PG_RETURN_POINTER(out);
}

Datum
tsql_varchar_rtrim(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *out;
  int i;

  for (i = VARSIZE(in) - 1; i >= VARHDRSZ; i--) {
    if (in[i] != ' ')
      break;
  }
  out = palloc(i + 1);
  SET_VARSIZE(out, i + 1);
  memcpy(out + VARHDRSZ, in + VARHDRSZ, i - VARHDRSZ); 
  PG_RETURN_POINTER(out);
}

Datum
tsql_varchar_left(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  int32_t len = PG_GETARG_INT32(1);
  char *out;

  if (len > VARSIZE(in) - VARHDRSZ) {
    out = palloc(VARSIZE(in));
    memcpy(out, in, VARSIZE(in));
  } else {
    out = palloc(len - VARHDRSZ);
    memcpy(out + VARHDRSZ, in + VARHDRSZ, len);
    SET_VARSIZE(out, len + VARHDRSZ);
  }
  PG_RETURN_POINTER(out);
}

Datum
tsql_varchar_len(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  int32_t out;

  out = VARSIZE(in) - VARHDRSZ;
  PG_RETURN_INT32(out);
}

Datum
tsql_varchar_replace(PG_FUNCTION_ARGS) {
  char *expr = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *pattern = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *replace = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *out;
  int i;
  int matches;

  while (i < VARSIZE(expr) - VARHDRSZ - (VARSIZE(pattern) - VARHDRSZ)) {
    if (memcmp(expr + VARHDRSZ + i, pattern + VARHDRSZ, VARSIZE(pattern) - VARHDRSZ) == 0) {
      matches++;
      i += VARSIZE(pattern) - VARHDRSZ;
    } else {
      i++;
    }
  }
  out = palloc(VARSIZE(expr) + (VARSIZE(pattern) - VARHDRSZ) * matches - (VARSIZE(replace) - VARHDRSZ) * matches);
  i = 0;
  matches = 0;
  while (i < VARSIZE(expr) - VARHDRSZ - (VARSIZE(pattern) - VARHDRSZ)) {
    if (memcmp(expr + VARHDRSZ + i, pattern + VARHDRSZ, VARSIZE(pattern) - VARHDRSZ) == 0) {
      memcpy(out + VARHDRSZ + i + (VARSIZE(pattern) - VARHDRSZ) * matches - (VARSIZE(replace) - VARHDRSZ) * matches, replace + VARHDRSZ, VARSIZE(replace) - VARHDRSZ);
      matches++;
      i += VARSIZE(pattern) - VARHDRSZ;
    } else {
      out[VARHDRSZ + i + (VARSIZE(pattern) - VARHDRSZ) * matches - (VARSIZE(replace) - VARHDRSZ) * matches] = expr[VARHDRSZ + i];
      i++;
    }
  }
  SET_VARSIZE(out, VARSIZE(expr) + (VARSIZE(pattern) - VARHDRSZ) * matches - (VARSIZE(replace) - VARHDRSZ) * matches);
    
  PG_RETURN_POINTER(out);
}

Datum
tsql_varchar_coalesce(PG_FUNCTION_ARGS) {
  // TODO if arg0 is null then return arg1
  char *out;

  out = palloc(VARHDRSZ + 2);
  SET_VARSIZE(out, VARHDRSZ + 2);
  out[VARHDRSZ + 0] = '4';
  out[VARHDRSZ + 1] = '2';
  
  PG_RETURN_POINTER(out);
}
