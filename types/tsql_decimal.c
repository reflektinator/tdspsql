#include <postgres.h>
#include <catalog/pg_type.h>
#include <utils/array.h>
#include <utils/builtins.h>
#include <fmgr.h>
#include <string.h>
#include <gmp.h>

PG_FUNCTION_INFO_V1(tsql_decimal_in);
PG_FUNCTION_INFO_V1(tsql_decimal_out);
PG_FUNCTION_INFO_V1(tsql_decimal_typmod_in);
PG_FUNCTION_INFO_V1(tsql_decimal_typmod_out);
PG_FUNCTION_INFO_V1(tsql_decimal_recv);
PG_FUNCTION_INFO_V1(tsql_decimal_send);

PG_FUNCTION_INFO_V1(tsql_cast_decimal_tinyint);
PG_FUNCTION_INFO_V1(tsql_cast_decimal_smallint);
PG_FUNCTION_INFO_V1(tsql_cast_decimal_int);
PG_FUNCTION_INFO_V1(tsql_cast_tinyint_decimal);
PG_FUNCTION_INFO_V1(tsql_cast_smallint_decimal);
PG_FUNCTION_INFO_V1(tsql_cast_int_decimal);
PG_FUNCTION_INFO_V1(tsql_cast_decimal_float);
PG_FUNCTION_INFO_V1(tsql_cast_float_decimal);

PG_FUNCTION_INFO_V1(tsql_decimal_eq_decimal);
PG_FUNCTION_INFO_V1(tsql_decimal_ne_decimal);
PG_FUNCTION_INFO_V1(tsql_decimal_lt_decimal);
PG_FUNCTION_INFO_V1(tsql_decimal_le_decimal);
PG_FUNCTION_INFO_V1(tsql_decimal_gt_decimal);
PG_FUNCTION_INFO_V1(tsql_decimal_ge_decimal);
PG_FUNCTION_INFO_V1(tsql_decimal_cmp_decimal);
PG_FUNCTION_INFO_V1(tsql_decimal_eq_int);
PG_FUNCTION_INFO_V1(tsql_decimal_ne_int);
PG_FUNCTION_INFO_V1(tsql_decimal_pl_decimal);
PG_FUNCTION_INFO_V1(tsql_decimal_mi_decimal);
PG_FUNCTION_INFO_V1(tsql_decimal_mul_decimal);
PG_FUNCTION_INFO_V1(tsql_decimal_mul_int);

static int32_t
decimal_cmp_decimal(char *l_raw, char *r_raw) {
  mpz_t l;
  mpz_t r;
  int cmp;

  mpz_init(l);
  mpz_import(l, VARSIZE(l_raw) - VARHDRSZ - 2, -1, 1, -1, 0, l_raw + VARHDRSZ + 2);
  mpz_init(r);
  mpz_import(r, VARSIZE(r_raw) - VARHDRSZ - 2, -1, 1, -1, 0, r_raw + VARHDRSZ + 2);
  /* find difference in scale */
  /* multiply lower by difference */
  cmp = mpz_cmp(l, r);
  if (cmp < 0) {
    return -1;
  } else if (cmp > 0) {
    return 1;
  } else {
    return 0;
  }
}

static char *
cast_int_decimal(int32_t in) {
  char *out;
  mpz_t num;
  size_t mpz_len;
  char *result;
  int32_t precision;
  int32_t scale;

  mpz_init_set_si(num, in);
  result = (char *)mpz_export(NULL, &mpz_len, -1, 1, -1, 0, num);
  precision = mpz_sizeinbase(num, 10);
  scale = 0;

  out = palloc(VARHDRSZ + 2 + mpz_len);
  out[VARHDRSZ + 0] = precision;
  out[VARHDRSZ + 1] = scale;
  memcpy(out + VARHDRSZ + 2, result, mpz_len);
  SET_VARSIZE(out, VARHDRSZ + 2 + mpz_len);
  return out;
}

Datum
tsql_decimal_in(PG_FUNCTION_ARGS) {
  char *in = PG_GETARG_CSTRING(0);
  char *in2 = palloc(strlen(in) + 1);
  int32_t typmod = PG_GETARG_INT32(2);
  int32_t precision = typmod >> 8;
  int32_t scale = typmod & 255;
  int i, j, decimal;
  char *out;
  char *result;
  size_t mpz_len;
  mpz_t num;

  for (i = 0, j = 0, decimal = -1; i < strlen(in); i++) {
    if (j == 0 && in[i] == ' ') {
      continue;
    }
    if (j == 0 && in[i] == '-') {
      /* set a sign flag or something */
      continue;
    }
    if (in[i] == '.' && decimal == -1) {
      decimal = j + 1;
      continue;
    }
    if (!isdigit(in[i])) {
      /* this is an error */
    }
    in2[j] = in[i];
    j++;
  }
  in2[j] = 0;
  if (typmod == -1) {
    precision = j;
    if (decimal == -1) {
      scale = 0;
    } else {
      scale = j - decimal + 1;
    }
  }
  mpz_init_set_str(num, in2, 10);
  result = (char *)mpz_export(NULL, &mpz_len, -1, 1, -1, 0, num);
  out = palloc(VARHDRSZ + 2 + mpz_len);
  out[VARHDRSZ + 0] = precision;
  out[VARHDRSZ + 1] = scale;
  memcpy(out + VARHDRSZ + 2, result, mpz_len);
  SET_VARSIZE(out, VARHDRSZ + 2 + mpz_len);
  PG_RETURN_POINTER(out);
}

Datum
tsql_decimal_out(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  //int32_t precision = (uint32_t)(uint8_t)in[VARHDRSZ + 0];
  int32_t scale = (uint32_t)(uint8_t)in[VARHDRSZ + 1];
  char *out = palloc(69);
  char *raw = palloc(69);
  mpz_t num;

  /* it is valid to only pass one parameter in typmod... how to display? */

  mpz_init(num);
  mpz_import(num, VARSIZE(in) - VARHDRSZ - 2, -1, 1, -1, 0, in + VARHDRSZ + 2);
  mpz_get_str(raw, 10, num);
  //sprintf(out, "%d.%d: ", precision, scale);
  strncat(out, raw, strlen(raw) - scale);
  out[strlen(raw) - scale] = 0;
  if (scale > 0) {
    strcat(out, ".");
    strcat(out, raw + strlen(raw) - scale);
  }
  PG_RETURN_CSTRING(out);
}

Datum
tsql_decimal_typmod_in(PG_FUNCTION_ARGS) {
  ArrayType *in = PG_GETARG_ARRAYTYPE_P(0);
  Datum *elems;
  int elem_count;
  int32_t out;
  int32_t precision;
  int32_t scale;

  /* it is valid to only pass one parameter in typmod */
  deconstruct_array(in, CSTRINGOID, -2, false, 'c', &elems, NULL, &elem_count);
  switch(elem_count) {
  case 0:
    precision = 19; // TODO
    scale = 0; // TODO
    break;
  case 1:
    precision = pg_atoi(DatumGetCString(elems[0]), sizeof(int32), '\0');
    scale = 0;
    break;
  case 2:
    precision = pg_atoi(DatumGetCString(elems[0]), sizeof(int32), '\0');
    scale = pg_atoi(DatumGetCString(elems[1]), sizeof(int32), '\0');
    break;
  default:
    // error
    break;
  }
  /* precision should be >= 1 && <= 36 */
  /*  (but note that mpfr doesn't support precision < 2) */
  /* scale should be <= precision */
  out = (precision << 8) | (scale);
  PG_RETURN_INT32(out);
}

Datum
tsql_decimal_typmod_out(PG_FUNCTION_ARGS) {
  int32_t typmod = PG_GETARG_INT32(0);
  int32_t precision = typmod >> 8;
  int32_t scale = typmod & 255;
  char *out = palloc(8);

  snprintf(out, 8, "(%d,%d)", precision, scale);
  PG_RETURN_CSTRING(out);
}

Datum
tsql_cast_decimal_tinyint(PG_FUNCTION_ARGS) {
  char *in_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  mpz_t in;
  uint8_t out;
  mpz_init(in);
  mpz_import(in, VARSIZE(in_raw) - VARHDRSZ - 2, -1, 1, -1, 0, in_raw + VARHDRSZ + 2);
  out = (uint8_t)mpz_get_ui(in);
  PG_RETURN_CHAR(out);
}

Datum
tsql_cast_decimal_smallint(PG_FUNCTION_ARGS) {
  char *in_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  mpz_t in;
  int16_t out;
  mpz_init(in);
  mpz_import(in, VARSIZE(in_raw) - VARHDRSZ - 2, -1, 1, -1, 0, in_raw + VARHDRSZ + 2);
  out = (int8_t)mpz_get_ui(in);
  PG_RETURN_INT16(out);
}

Datum
tsql_cast_decimal_int(PG_FUNCTION_ARGS) {
  char *in_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  mpz_t in;
  int32_t out;
  mpz_init(in);
  mpz_import(in, VARSIZE(in_raw) - VARHDRSZ - 2, -1, 1, -1, 0, in_raw + VARHDRSZ + 2);
  out = (int32_t)mpz_get_ui(in);
  PG_RETURN_INT32(out);
}

Datum
tsql_cast_decimal_float(PG_FUNCTION_ARGS) {
  char *in_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  mpz_t in;
  double out;
  mpz_init(in);
  mpz_import(in, VARSIZE(in_raw) - VARHDRSZ - 2, -1, 1, -1, 0, in_raw + VARHDRSZ + 2);
  out = (int32_t)mpz_get_d(in);
  PG_RETURN_FLOAT8(out);
}

Datum
tsql_cast_smallint_decimal(PG_FUNCTION_ARGS) {
  int16_t in = PG_GETARG_INT16(0);
  //int32_t typmod = PG_GETARG_INT32(2);
  char *out;
  mpz_t num;
  size_t mpz_len;
  char *result;
  int32_t precision;
  int32_t scale;

  mpz_init_set_si(num, in);
  result = (char *)mpz_export(NULL, &mpz_len, -1, 1, -1, 0, num);
  precision = mpz_sizeinbase(num, 10);
  scale = 0;

  out = palloc(VARHDRSZ + 2 + mpz_len);
  out[VARHDRSZ + 0] = precision;
  out[VARHDRSZ + 1] = scale;
  memcpy(out + VARHDRSZ + 2, result, mpz_len);
  SET_VARSIZE(out, VARHDRSZ + 2 + mpz_len);
  PG_RETURN_POINTER(out);
}

Datum
tsql_cast_int_decimal(PG_FUNCTION_ARGS) {
  int32_t in = PG_GETARG_INT32(0);
  //int32_t typmod = PG_GETARG_INT32(2);
  char *out;
  mpz_t num;
  size_t mpz_len;
  char *result;
  int32_t precision;
  int32_t scale;

  mpz_init_set_si(num, in);
  result = (char *)mpz_export(NULL, &mpz_len, -1, 1, -1, 0, num);
  precision = mpz_sizeinbase(num, 10);
  scale = 0;

  out = palloc(VARHDRSZ + 2 + mpz_len);
  out[VARHDRSZ + 0] = precision;
  out[VARHDRSZ + 1] = scale;
  memcpy(out + VARHDRSZ + 2, result, mpz_len);
  SET_VARSIZE(out, VARHDRSZ + 2 + mpz_len);
  PG_RETURN_POINTER(out);
}

Datum
tsql_cast_float_decimal(PG_FUNCTION_ARGS) {
  double in = PG_GETARG_FLOAT8(0);
  //int32_t typmod = PG_GETARG_INT32(2);
  char *out;
  mpz_t num;
  size_t mpz_len;
  char *result;
  int32_t precision;
  int32_t scale;

  mpz_init_set_d(num, in);
  result = (char *)mpz_export(NULL, &mpz_len, -1, 1, -1, 0, num);
  precision = mpz_sizeinbase(num, 10);
  scale = 0;

  out = palloc(VARHDRSZ + 2 + mpz_len);
  out[VARHDRSZ + 0] = precision;
  out[VARHDRSZ + 1] = scale;
  memcpy(out + VARHDRSZ + 2, result, mpz_len);
  SET_VARSIZE(out, VARHDRSZ + 2 + mpz_len);
  PG_RETURN_POINTER(out);
}

Datum
tsql_decimal_eq_decimal(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));
  if (decimal_cmp_decimal(l_raw, r_raw) == 0) {
    PG_RETURN_BOOL(1);
  } else {
    PG_RETURN_BOOL(0);
  }
}

Datum
tsql_decimal_ne_decimal(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));
  if (decimal_cmp_decimal(l_raw, r_raw) != 0) {
    PG_RETURN_BOOL(1);
  } else {
    PG_RETURN_BOOL(0);
  }
}

Datum
tsql_decimal_lt_decimal(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));
  if (decimal_cmp_decimal(l_raw, r_raw) < 0) {
    PG_RETURN_BOOL(1);
  } else {
    PG_RETURN_BOOL(0);
  }
}

Datum
tsql_decimal_le_decimal(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));
  if (decimal_cmp_decimal(l_raw, r_raw) <= 0) {
    PG_RETURN_BOOL(1);
  } else {
    PG_RETURN_BOOL(0);
  }
}

Datum
tsql_decimal_gt_decimal(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));
  if (decimal_cmp_decimal(l_raw, r_raw) > 0) {
    PG_RETURN_BOOL(1);
  } else {
    PG_RETURN_BOOL(0);
  }
}

Datum
tsql_decimal_ge_decimal(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));
  if (decimal_cmp_decimal(l_raw, r_raw) >= 0) {
    PG_RETURN_BOOL(1);
  } else {
    PG_RETURN_BOOL(0);
  }
}

Datum
tsql_decimal_cmp_decimal(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));
  PG_RETURN_INT32(decimal_cmp_decimal(l_raw, r_raw));
}

Datum
tsql_decimal_eq_int(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r_raw = cast_int_decimal(PG_GETARG_INT32(1));

  if (decimal_cmp_decimal(l_raw, r_raw) == 0) {
    PG_RETURN_BOOL(0);
  } else {
    PG_RETURN_BOOL(1);
  }
}

Datum
tsql_decimal_ne_int(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *r_raw = cast_int_decimal(PG_GETARG_INT32(1));

  if (decimal_cmp_decimal(l_raw, r_raw) != 0) {
    PG_RETURN_BOOL(1);
  } else {
    PG_RETURN_BOOL(0);
  }
}

Datum
tsql_decimal_pl_decimal(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  int32_t l_p = (uint32_t)(uint8_t)l_raw[VARHDRSZ + 0];
  int32_t l_s = (uint32_t)(uint8_t)l_raw[VARHDRSZ + 1];
  char *r_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));
  mpz_t l;
  mpz_t r;
  mpz_t o;
  char *o_raw;
  char *tmp;
  size_t mpz_len;

// TODO: account for precision and scale
  mpz_init(l);
  mpz_import(l, VARSIZE(l_raw) - VARHDRSZ - 2, -1, 1, -1, 0, l_raw + VARHDRSZ + 2);
  mpz_init(r);
  mpz_import(r, VARSIZE(r_raw) - VARHDRSZ - 2, -1, 1, -1, 0, r_raw + VARHDRSZ + 2);

  mpz_init(o);
  mpz_add(o, l, r);

  tmp = (char *)mpz_export(NULL, &mpz_len, -1, 1, -1, 0, o);
  o_raw = palloc(VARHDRSZ + 2 + mpz_len);
  o_raw[VARHDRSZ + 0] = l_p;
  o_raw[VARHDRSZ + 1] = l_s;
  memcpy(o_raw + VARHDRSZ + 2, tmp, mpz_len);
  SET_VARSIZE(o_raw, VARHDRSZ + 2 + mpz_len);
  PG_RETURN_POINTER(o_raw);
}

Datum
tsql_decimal_mi_decimal(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  int32_t l_p = (uint32_t)(uint8_t)l_raw[VARHDRSZ + 0];
  int32_t l_s = (uint32_t)(uint8_t)l_raw[VARHDRSZ + 1];
  char *r_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));
  mpz_t l;
  mpz_t r;
  mpz_t o;
  char *o_raw;
  char *tmp;
  size_t mpz_len;

// TODO: account for precision and scale
  mpz_init(l);
  mpz_import(l, VARSIZE(l_raw) - VARHDRSZ - 2, -1, 1, -1, 0, l_raw + VARHDRSZ + 2);
  mpz_init(r);
  mpz_import(r, VARSIZE(r_raw) - VARHDRSZ - 2, -1, 1, -1, 0, r_raw + VARHDRSZ + 2);

  mpz_init(o);
  mpz_sub(o, l, r);

  tmp = (char *)mpz_export(NULL, &mpz_len, -1, 1, -1, 0, o);
  o_raw = palloc(VARHDRSZ + 2 + mpz_len);
  o_raw[VARHDRSZ + 0] = l_p;
  o_raw[VARHDRSZ + 1] = l_s;
  memcpy(o_raw + VARHDRSZ + 2, tmp, mpz_len);
  SET_VARSIZE(o_raw, VARHDRSZ + 2 + mpz_len);
  PG_RETURN_POINTER(o_raw);
}

Datum
tsql_decimal_mul_decimal(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  int32_t l_p = (uint32_t)(uint8_t)l_raw[VARHDRSZ + 0];
  int32_t l_s = (uint32_t)(uint8_t)l_raw[VARHDRSZ + 1];
  char *r_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));
  mpz_t l;
  mpz_t r;
  mpz_t o;
  char *o_raw;
  char *tmp;
  size_t mpz_len;

// TODO: account for precision and scale
  mpz_init(l);
  mpz_import(l, VARSIZE(l_raw) - VARHDRSZ - 2, -1, 1, -1, 0, l_raw + VARHDRSZ + 2);
  mpz_init(r);
  mpz_import(r, VARSIZE(r_raw) - VARHDRSZ - 2, -1, 1, -1, 0, r_raw + VARHDRSZ + 2);

  mpz_init(o);
  mpz_mul(o, l, r);

  tmp = (char *)mpz_export(NULL, &mpz_len, -1, 1, -1, 0, o);
  o_raw = palloc(VARHDRSZ + 2 + mpz_len);
  o_raw[VARHDRSZ + 0] = l_p;
  o_raw[VARHDRSZ + 1] = l_s;
  memcpy(o_raw + VARHDRSZ + 2, tmp, mpz_len);
  SET_VARSIZE(o_raw, VARHDRSZ + 2 + mpz_len);
  PG_RETURN_POINTER(o_raw);
}


Datum
tsql_decimal_mul_int(PG_FUNCTION_ARGS) {
  char *l_raw = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  int32_t l_p = (uint32_t)(uint8_t)l_raw[VARHDRSZ + 0];
  int32_t l_s = (uint32_t)(uint8_t)l_raw[VARHDRSZ + 1];
  mpz_t l;
  int32_t r = PG_GETARG_INT32(1);
  mpz_t o;
  char *o_raw;
  char *tmp;
  size_t mpz_len;

// TODO: account for precision and scale
  mpz_init(l);
  mpz_import(l, VARSIZE(l_raw) - VARHDRSZ - 2, -1, 1, -1, 0, l_raw + VARHDRSZ + 2);

  mpz_init(o);
  mpz_mul_si(o, l, r);

  tmp = (char *)mpz_export(NULL, &mpz_len, -1, 1, -1, 0, o);
  o_raw = palloc(VARHDRSZ + 2 + mpz_len);
  o_raw[VARHDRSZ + 0] = l_p;
  o_raw[VARHDRSZ + 1] = l_s;
  memcpy(o_raw + VARHDRSZ + 2, tmp, mpz_len);
  SET_VARSIZE(o_raw, VARHDRSZ + 2 + mpz_len);
  PG_RETURN_POINTER(o_raw);
}
