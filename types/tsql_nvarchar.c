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
#include <unicode/ucnv.h>
#include <unicode/ucol.h>

PG_FUNCTION_INFO_V1(tsql_nvarchar_in);
PG_FUNCTION_INFO_V1(tsql_nvarchar_out);
/* uses varchar typmod functions */
PG_FUNCTION_INFO_V1(tsql_nvarchar_recv);
PG_FUNCTION_INFO_V1(tsql_nvarchar_send);
PG_FUNCTION_INFO_V1(tsql_cast_nvarchar_nvarchar);
PG_FUNCTION_INFO_V1(tsql_cast_varchar_nvarchar);
PG_FUNCTION_INFO_V1(tsql_cast_uniqueidentifier_nvarchar);
//PG_FUNCTION_INFO_V1(tsql_cast_nvarchar_int);
//PG_FUNCTION_INFO_V1(tsql_cast_float_nvarchar);
PG_FUNCTION_INFO_V1(tsql_nvarchar_eq_nvarchar);
PG_FUNCTION_INFO_V1(tsql_nvarchar_ne_nvarchar);
PG_FUNCTION_INFO_V1(tsql_nvarchar_lt_nvarchar);
PG_FUNCTION_INFO_V1(tsql_nvarchar_le_nvarchar);
PG_FUNCTION_INFO_V1(tsql_nvarchar_gt_nvarchar);
PG_FUNCTION_INFO_V1(tsql_nvarchar_ge_nvarchar);
PG_FUNCTION_INFO_V1(tsql_nvarchar_cmp);
PG_FUNCTION_INFO_V1(tsql_nvarchar_pl_nvarchar);
PG_FUNCTION_INFO_V1(tsql_nvarchar_max);
PG_FUNCTION_INFO_V1(tsql_nvarchar_upper);
PG_FUNCTION_INFO_V1(tsql_nvarchar_lower);
PG_FUNCTION_INFO_V1(tsql_nvarchar_ltrim);
PG_FUNCTION_INFO_V1(tsql_nvarchar_rtrim);
PG_FUNCTION_INFO_V1(tsql_nvarchar_left);
PG_FUNCTION_INFO_V1(tsql_nvarchar_len);
PG_FUNCTION_INFO_V1(tsql_nvarchar_replace);
PG_FUNCTION_INFO_V1(tsql_nvarchar_coalesce);

static int32_t
nvarchar_cmp(char *l, char *r) {
  int cmp;
  UCollator *ucol;
  UErrorCode ustatus;

  ustatus = U_ZERO_ERROR;
  ucol = ucol_open("en_US", &ustatus);
  if(U_FAILURE(ustatus)) {
    char *msg;
    asprintf(&msg, "%s", u_errorName(ustatus));
    elog(ERROR, msg);
  }
  /* if CI */
  ucol_setStrength(ucol, UCOL_SECONDARY); /* ignore case differences */

  cmp = ucol_strcoll(ucol, (UChar *)(l + VARHDRSZ), (VARSIZE(l) - VARHDRSZ) / 2, (UChar *)(r + VARHDRSZ), (VARSIZE(r) - VARHDRSZ) / 2);

  if (cmp == UCOL_LESS) {
    return -1;
  } else if (cmp == UCOL_GREATER) {
    return 1;
  } else {
    return 0;
  }
}

Datum
tsql_nvarchar_in(PG_FUNCTION_ARGS) {
  char *in = PG_GETARG_CSTRING(0);
  int32_t len = PG_GETARG_INT32(2);
  //collation = PG_GET_COLLATION();
  char *out;
  UConverter *ucnv = NULL;
  UErrorCode ustatus = U_ZERO_ERROR;
  int outlen;

  if (len == -1 || len >= strlen(in)) {
    len = strlen(in);
  } else {
    elog(WARNING, "String truncation in assign");
  }
  ucnv = ucnv_open("ISO-8859-1", &ustatus);
  if(U_FAILURE(ustatus)) {
    char *msg;
    asprintf(&msg, "%s", u_errorName(ustatus));
    elog(ERROR, msg);
  }
  outlen = ucnv_toUChars(ucnv, NULL, 0, in, len, &ustatus);
  if (U_FAILURE(ustatus) && ustatus != U_BUFFER_OVERFLOW_ERROR) {
    char *msg;
    asprintf(&msg, "%s", u_errorName(ustatus));
    elog(ERROR, msg);
  }
  out = palloc(VARHDRSZ + outlen * sizeof(UChar));
  ustatus = U_ZERO_ERROR;
  ucnv_toUChars(ucnv, (UChar *)(out + VARHDRSZ), outlen, in, len, &ustatus);
  SET_VARSIZE(out, VARHDRSZ + outlen * sizeof(UChar));
  ucnv_close(ucnv);
  PG_RETURN_POINTER(out);
}

Datum
tsql_nvarchar_out(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *out;
  UConverter *ucnv = NULL;
  UErrorCode ustatus = U_ZERO_ERROR;
  int outlen;

  ucnv = ucnv_open("ISO-8859-1", &ustatus);
  if(U_FAILURE(ustatus)) {
    char *msg;
    asprintf(&msg, "%s", u_errorName(ustatus));
    elog(ERROR, msg);
  }
  outlen = ucnv_fromUChars(ucnv, NULL, 0, (UChar *)(in + VARHDRSZ), (VARSIZE(in) - VARHDRSZ) / 2, &ustatus);
  if (U_FAILURE(ustatus) && ustatus != U_BUFFER_OVERFLOW_ERROR) {
    char *msg;
    asprintf(&msg, "%s", u_errorName(ustatus));
    elog(ERROR, msg);
  }
  out = palloc(outlen + 1);
  ustatus = U_ZERO_ERROR;
  ucnv_fromUChars(ucnv, out, outlen + 1, (UChar *)(in + VARHDRSZ), (VARSIZE(in) - VARHDRSZ) / 2, &ustatus);
  ucnv_close(ucnv);
  PG_RETURN_CSTRING(out);
}

Datum
tsql_nvarchar_recv(PG_FUNCTION_ARGS) {
  StringInfo in = (StringInfo)PG_GETARG_POINTER(0);
  char *out;
// TODO takes a typmod argument - use it

  out = palloc(VARHDRSZ + in->len);
  pq_copymsgbytes(in, out + VARHDRSZ, in->len);
  SET_VARSIZE(out, VARHDRSZ + in->len);
  PG_RETURN_POINTER(out);
}

Datum
tsql_nvarchar_send(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  StringInfoData out;

  pq_begintypsend(&out);
  pq_sendbytes(&out, in + VARHDRSZ, VARSIZE(in) - VARHDRSZ);
  PG_RETURN_BYTEA_P(pq_endtypsend(&out));
}

Datum
tsql_cast_nvarchar_nvarchar(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  int32_t len = PG_GETARG_INT32(1);
  char *out;

  if (len == -1 || len >= (VARSIZE(in) - VARHDRSZ) / 2) {
    len = VARSIZE(in) - VARHDRSZ;
  } else {
    elog(WARNING, "String truncation in cast");
    len = len * 2;
  }
  out = palloc(VARHDRSZ + len);
  memcpy(out + VARHDRSZ, in + VARHDRSZ, len);
  SET_VARSIZE(out, VARHDRSZ + len);
  PG_RETURN_POINTER(out);
}

Datum
tsql_cast_varchar_nvarchar(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  int32_t len = PG_GETARG_INT32(1);
  //collation = PG_GET_COLLATION();
  char *out;
  UConverter *ucnv = NULL;
  UErrorCode ustatus = U_ZERO_ERROR;
  int outlen;

  if (len == -1 || len >= VARSIZE(in) - VARHDRSZ) {
    len = VARSIZE(in) - VARHDRSZ;
  } else {
    elog(WARNING, "String truncation in assign");
  }

  ucnv = ucnv_open("ISO-8859-1", &ustatus);
  if(U_FAILURE(ustatus)) {
    char *msg;
    asprintf(&msg, "%s", u_errorName(ustatus));
    elog(ERROR, msg);
  }
  outlen = ucnv_toUChars(ucnv, NULL, 0, in + VARHDRSZ, len, &ustatus);
  if (U_FAILURE(ustatus) && ustatus != U_BUFFER_OVERFLOW_ERROR) {
    char *msg;
    asprintf(&msg, "%s", u_errorName(ustatus));
    elog(ERROR, msg);
  }
  out = palloc(VARHDRSZ + outlen * sizeof(UChar));
  ustatus = U_ZERO_ERROR;
  ucnv_toUChars(ucnv, (UChar *)(out + VARHDRSZ), outlen, in + VARHDRSZ, len, &ustatus);
  SET_VARSIZE(out, VARHDRSZ + outlen * sizeof(UChar));
  ucnv_close(ucnv);
  PG_RETURN_POINTER(out);
}

Datum
tsql_cast_uniqueidentifier_nvarchar(PG_FUNCTION_ARGS) {
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
tsql_cast_int_nvarchar(PG_FUNCTION_ARGS) {
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
tsql_cast_nvarchar_int(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *in2 = palloc(VARSIZE(in) - VARHDRSZ + 1);
  int32_t out;

  memcpy(in2, in + VARHDRSZ, VARSIZE(in) - VARHDRSZ);
  in2[VARSIZE(in) - VARHDRSZ] = 0;
  out = atoi(in2);
  PG_RETURN_INT32(out);
}

Datum
tsql_cast_float_nvarchar(PG_FUNCTION_ARGS) {
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
tsql_nvarchar_eq_nvarchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (nvarchar_cmp(l, r) == 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_nvarchar_ne_nvarchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (nvarchar_cmp(l, r) != 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_nvarchar_lt_nvarchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (nvarchar_cmp(l, r) < 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_nvarchar_le_nvarchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (nvarchar_cmp(l, r) <= 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_nvarchar_gt_nvarchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (nvarchar_cmp(l, r) > 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_nvarchar_ge_nvarchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (nvarchar_cmp(l, r) >= 0)
    PG_RETURN_BOOL(1);
  else
    PG_RETURN_BOOL(0);
}

Datum
tsql_nvarchar_cmp(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  PG_RETURN_INT32(nvarchar_cmp(l, r));
}

Datum
tsql_nvarchar_pl_nvarchar(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  char *out = palloc(VARHDRSZ + VARSIZE(l) - VARHDRSZ + VARSIZE(r) - VARHDRSZ);
  SET_VARSIZE(out, VARHDRSZ + VARSIZE(l) - VARHDRSZ + VARSIZE(r) - VARHDRSZ);
  memcpy(out + VARHDRSZ, l + VARHDRSZ, VARSIZE(l) - VARHDRSZ);
  memcpy(out + VARSIZE(l), r + VARHDRSZ, VARSIZE(r) - VARHDRSZ);
  PG_RETURN_POINTER(out);
}

Datum
tsql_nvarchar_max(PG_FUNCTION_ARGS) {
  char *l = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));

  if (nvarchar_cmp(r, l) > 0) {
    PG_RETURN_POINTER(r);
  } else {
    PG_RETURN_POINTER(l);
  }
}

Datum
tsql_nvarchar_upper(PG_FUNCTION_ARGS) {
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
tsql_nvarchar_lower(PG_FUNCTION_ARGS) {
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
tsql_nvarchar_ltrim(PG_FUNCTION_ARGS) {
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
tsql_nvarchar_rtrim(PG_FUNCTION_ARGS) {
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
tsql_nvarchar_left(PG_FUNCTION_ARGS) {
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
tsql_nvarchar_len(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  int32_t out;

  out = VARSIZE(in) - VARHDRSZ;
  PG_RETURN_INT32(out);
}

Datum
tsql_nvarchar_replace(PG_FUNCTION_ARGS) {
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
tsql_nvarchar_coalesce(PG_FUNCTION_ARGS) {
  // TODO if arg0 is null then return arg1
  char *out;

  out = palloc(VARHDRSZ + 2);
  SET_VARSIZE(out, VARHDRSZ + 2);
  out[VARHDRSZ + 0] = '4';
  out[VARHDRSZ + 1] = '2';
  
  PG_RETURN_POINTER(out);
}

