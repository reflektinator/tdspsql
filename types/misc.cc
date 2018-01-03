extern "C" {
#include <postgres.h>
#include <catalog/pg_type.h>
#include <utils/array.h>
#include <utils/builtins.h>
#include <fmgr.h>
#include <string.h>
#include <math.h>

PG_FUNCTION_INFO_V1(tsql_float_str);
PG_FUNCTION_INFO_V1(tsql_float_round);
PG_FUNCTION_INFO_V1(tsql_object_to_pg_object);

Datum
tsql_float_str(PG_FUNCTION_ARGS) {
  double val = PG_GETARG_FLOAT8(0);
  int32_t len = PG_GETARG_INT32(1);
  int32_t places = PG_GETARG_INT32(2);
  char *out;
  int buflen;

  buflen = snprintf(NULL, 0, "%*.*f", len, places, val);
  out = (char *)palloc(buflen + VARHDRSZ);
  SET_VARSIZE(out, buflen + VARHDRSZ);
  buflen = snprintf(out + VARHDRSZ, buflen, "%*.*f", len, places, val);
  PG_RETURN_POINTER(out);
}

Datum
tsql_float_round(PG_FUNCTION_ARGS) {
  double val = PG_GETARG_FLOAT8(0);
  double digits = PG_GETARG_INT32(0);
  double out;

  out = round(val * pow(digits, 10)) / pow(digits, 10);
  PG_RETURN_FLOAT8(out);
}

#define append(dst, src, len) do { memcpy(*(dst), (src), (len)); *(dst) += (len); } while (0)
#define nappend(dst, src, len) do { memcpy(*(dst), (src), (len)*2); *(dst) += (len); } while (0)

Datum
tsql_object_to_pg_object(PG_FUNCTION_ARGS) {
  char *in_datum = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *db_datum = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(1));
  char *schema_datum = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(2));
  char16_t *in = (char16_t *)(in_datum + VARHDRSZ);
  char16_t *db = (char16_t *)(db_datum + VARHDRSZ);
  char16_t *schema = (char16_t *)(schema_datum + VARHDRSZ);
  char *out_datum;
  char16_t *out;
  char16_t *outptr;
  int state = 0;
  int start[3];
  int end[3];
  int offset_count = 0;
  int i;

  elog(WARNING, "VARSIZE(in_datum) = %d", VARSIZE(in_datum));
  //elog(WARNING, "db = %s", db + VARHDRSZ);
  //elog(WARNING, "schema = %s", schema + VARHDRSZ);
  for (i = 0; i < (int)(VARSIZE(in_datum) - VARHDRSZ) / 2; i++) {
    switch(state) {
    case 0:
      switch (in[i]) {
      case u'.':
        if (offset_count == 3) {
          elog(ERROR, "invalid object format");
          PG_RETURN_NULL();
        }
        start[offset_count] = i;
        end[offset_count] = i;
        offset_count++;
        break;
      case u'"':
        start[offset_count] = i + 1;
        state = 2;
        break;
      default:
        start[offset_count] = i;
        state = 1;
        break;
      }
      break;
    case 1:
      if (in[i] == u'.') {
        end[offset_count] = i;
        offset_count++;
        state = 0;
      }
      break;
    case 2:
      if (in[i] == u'"') {
        end[offset_count] = i;
        offset_count++;
        state = 3;
      }
      break;
    case 3:
      if (in[i] != u'.') {
        elog(ERROR, "invalid object format");
      }
      state = 0;
      break;
    }
  }
  if (state == 1) {
    end[offset_count] = i;
    offset_count++;
  }
  switch (offset_count) {
  case 0:
    elog(ERROR, "invalid object format");
    return 0;
  case 1:
    out_datum = (char *)palloc(VARHDRSZ + (1 + end[0] - start[0] + 1) * sizeof(char16_t));
    SET_VARSIZE(out_datum, VARHDRSZ + (1 + end[0] - start[0] + 1) * sizeof(char16_t));
    out = (char16_t *)(out_datum + VARHDRSZ);
    outptr = out;
    nappend(&outptr, u"\"", 1);
    nappend(&outptr, in + start[0], end[0] - start[0]);
    nappend(&outptr, u"\"", 1);
    break;
  case 2:
    out_datum = (char *)palloc(VARHDRSZ + (1 + (VARSIZE(db) - VARHDRSZ) / 2 + 1 + end[0] - start[0] + 3 + end[1] - start[1] + 1) * 2);
    SET_VARSIZE(out_datum, VARHDRSZ + (1 + (VARSIZE(db) - VARHDRSZ) / 2 + 1 + end[0] - start[0] + 3 + end[1] - start[1] + 1) * 2);
    out = (char16_t *)(out_datum + VARHDRSZ);
    outptr = out;
    nappend(&outptr, u"\"", 1);
    nappend(&outptr, db, (VARSIZE(db) - VARHDRSZ) / 2);
    nappend(&outptr, u".", 1);
    nappend(&outptr, in + start[0], end[0] - start[0]);
    nappend(&outptr, u"\".\"", 3);
    nappend(&outptr, in + start[1], end[1] - start[1]);
//elog(WARNING, "2: out = %s", out + VARHDRSZ);
    nappend(&outptr, "\"", 1);
    break;
  case 3:
    if (end[1] == start[1]) {
      out_datum = (char *)palloc(VARHDRSZ + (1 + end[0] - start[0] + 1 + (VARSIZE(schema) - VARHDRSZ) / 2 + 3 + end[2] - start[2] + 1) * 2);
      SET_VARSIZE(out_datum, VARHDRSZ + (1 + end[0] - start[0] + 1 + (VARSIZE(schema) - VARHDRSZ) / 2 + 3 + end[2] - start[2] + 1) * 2);
    } else {
      out_datum = (char *)palloc(VARHDRSZ + (1 + end[0] - start[0] + 1 + end[1] - start[1] + 3 + end[2] - start[2] + 1) * 2);
      SET_VARSIZE(out_datum, VARHDRSZ + (4 + end[0] - start[0] + 1 + end[1] - start[1] + 3 + end[2] - start[2] + 1) * 2);
    }
    out = (char16_t *)(out_datum + VARHDRSZ);
    outptr = out;
    nappend(&outptr, u"\"", 1);
    nappend(&outptr, in + start[0], end[0] - start[0]);
    nappend(&outptr, u".", 1);
    if (end[1] == start[1]) {
      nappend(&outptr, schema + VARHDRSZ, (VARSIZE(schema) - VARHDRSZ) / 2);
    } else {
      nappend(&outptr, in + start[1], end[1] - start[1]);
    }
    nappend(&outptr, u"\".\"", 3);
    nappend(&outptr, in + start[2], end[2] - start[2]);
    nappend(&outptr, u"\"", 1);
    break;
  }
  // TODO: this is rubbish
  for (i = VARHDRSZ; i < (int)VARSIZE(out_datum); i += 2) {
    out_datum[i] = tolower(out_datum[i]);
  }
  PG_RETURN_POINTER(out_datum);
}

}

