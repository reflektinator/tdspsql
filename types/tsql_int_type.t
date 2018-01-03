PG_FUNCTION_INFO_V1(tsql_@TYPE_NAME@_in);
PG_FUNCTION_INFO_V1(tsql_@TYPE_NAME@_out);
PG_FUNCTION_INFO_V1(tsql_@TYPE_NAME@_recv);
PG_FUNCTION_INFO_V1(tsql_@TYPE_NAME@_send);

PG_FUNCTION_INFO_V1(tsql_@TYPE_NAME@_um);

PG_FUNCTION_INFO_V1(tsql_@TYPE_NAME@_min);
PG_FUNCTION_INFO_V1(tsql_@TYPE_NAME@_max);

PG_FUNCTION_INFO_V1(tsql_@TYPE_NAME@_coalesce);

PG_FUNCTION_INFO_V1(tsql_cast_int4_@TYPE_NAME@);
PG_FUNCTION_INFO_V1(tsql_cast_int8_@TYPE_NAME@);
PG_FUNCTION_INFO_V1(tsql_cast_varchar_@TYPE_NAME@);
PG_FUNCTION_INFO_V1(tsql_cast_nvarchar_@TYPE_NAME@);
PG_FUNCTION_INFO_V1(tsql_cast_@TYPE_NAME@_varchar);
PG_FUNCTION_INFO_V1(tsql_cast_@TYPE_NAME@_float8);

static inline TYPE_@TYPE_NAME@
@TYPE_NAME@_cmp_@TYPE_NAME@(TYPE_@TYPE_NAME@ l, TYPE_@TYPE_NAME@ r) {
  if (l == r) {
    return 0;
  } else if (l < r) {
    return -1;
  } else {
    return 1;
  }
}

Datum
tsql_@TYPE_NAME@_in(PG_FUNCTION_ARGS) {
  TYPE_@TYPE_NAME@ out;

  char *in = PG_GETARG_CSTRING(0);
  if (sscanf(in, "@SCANF_FORMAT@", &out) != 1) {
    /* error */
  }

  if (out != 0 && out != 1) {
    /* error */
  }
  RETURN_@TYPE_NAME@(out);
}

Datum
tsql_@TYPE_NAME@_out(PG_FUNCTION_ARGS) {
  TYPE_@TYPE_NAME@ in = GETARG_@TYPE_NAME@(0);
  size_t len;
  char *out;

  len = snprintf(NULL, 0, "@SCANF_FORMAT@", in);
  out = palloc(len);
  sprintf(out, "@SCANF_FORMAT@", in);

  PG_RETURN_CSTRING(out);
}

Datum
tsql_@TYPE_NAME@_recv(PG_FUNCTION_ARGS) {
  StringInfo in = (StringInfo)PG_GETARG_POINTER(0);
  TYPE_@TYPE_NAME@ out;

  pq_copymsgbytes(in, (char *)&out, sizeof(out));

  RETURN_@TYPE_NAME@(out);
}

Datum
tsql_@TYPE_NAME@_send(PG_FUNCTION_ARGS) {
  TYPE_@TYPE_NAME@ in = GETARG_@TYPE_NAME@(0);
  StringInfoData out;

  pq_begintypsend(&out);
  pq_sendbytes(&out, (char *)&in, sizeof(TYPE_@TYPE_NAME@));
  PG_RETURN_BYTEA_P(pq_endtypsend(&out));
}

Datum
tsql_@TYPE_NAME@_um(PG_FUNCTION_ARGS) {
  TYPE_@TYPE_NAME@ r = GETARG_@TYPE_NAME@(0);

  RETURN_@TYPE_NAME@(-r);
}

Datum
tsql_@TYPE_NAME@_min(PG_FUNCTION_ARGS) {
  TYPE_@TYPE_NAME@ l = GETARG_@TYPE_NAME@(0);
  TYPE_@TYPE_NAME@ r = GETARG_@TYPE_NAME@(1);

  if (r < l) {
    RETURN_@TYPE_NAME@(r);
  } else {
    RETURN_@TYPE_NAME@(l);
  }
}

Datum
tsql_@TYPE_NAME@_max(PG_FUNCTION_ARGS) {
  TYPE_@TYPE_NAME@ l = GETARG_@TYPE_NAME@(0);
  TYPE_@TYPE_NAME@ r = GETARG_@TYPE_NAME@(1);

  if (r > l) {
    RETURN_@TYPE_NAME@(r);
  } else {
    RETURN_@TYPE_NAME@(l);
  }
}

Datum
tsql_@TYPE_NAME@_coalesce(PG_FUNCTION_ARGS) {
  TYPE_@TYPE_NAME@ in1 = GETARG_@TYPE_NAME@(0);

  //TODO
  RETURN_@TYPE_NAME@(in1);
}

Datum
tsql_cast_int4_@TYPE_NAME@(PG_FUNCTION_ARGS) {
  int32_t in = PG_GETARG_INT32(0);
  TYPE_@TYPE_NAME@ out;

  out = CAST_@TYPE_NAME@(in);

  RETURN_@TYPE_NAME@(out);
}

Datum
tsql_cast_int8_@TYPE_NAME@(PG_FUNCTION_ARGS) {
  int64_t in = PG_GETARG_INT64(0);
  TYPE_@TYPE_NAME@ out;

  out = CAST_@TYPE_NAME@(in);

  RETURN_@TYPE_NAME@(out);
}

Datum
tsql_cast_varchar_@TYPE_NAME@(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  char *val;
  TYPE_@TYPE_NAME@ out;
  char dummy;

  val = palloc(VARSIZE(in) - VARHDRSZ + 1);
  memcpy(val, in + VARHDRSZ, VARSIZE(in) - VARHDRSZ);
  val[VARSIZE(in) - VARHDRSZ] = 0;

  if (sscanf(val, "@SCANF_FORMAT@%c", &out, &dummy) != 1 || out < 0 || out > 255) {
    /* this is an error */
  }
  RETURN_@TYPE_NAME@(out);
}

Datum
tsql_cast_nvarchar_@TYPE_NAME@(PG_FUNCTION_ARGS) {
  char *in = (char *)PG_DETOAST_DATUM(PG_GETARG_POINTER(0));
  TYPE_@TYPE_NAME@ out;
  UNumberFormat ufmt;
  UErrorCode ustatus;

  ustatus = U_ZERO_ERROR;;
  ufmt = unum_open(UNUM_DEFAULT, NULL, -1, NULL, NULL, &ustatus);
  if(U_FAILURE(ustatus)) {
    char *msg;
    asprintf(&msg, "%s", u_errorName(ustatus));
    elog(ERROR, msg);
  }
  ustatus = U_ZERO_ERROR;;
  out = (TYPE_@TYPE_NAME@)PARSE_@TYPE_NAME@(ufmt, (UChar *)(in + VARHDRSZ), VARSIZE(in) - VARHDRSZ, NULL, &ustatus);
  if(U_FAILURE(ustatus)) {
    char *msg;
    asprintf(&msg, "%s", u_errorName(ustatus));
    elog(ERROR, msg);
  }
  unum_close(ufmt);
  RETURN_@TYPE_NAME@(out);
}

Datum
tsql_cast_@TYPE_NAME@_varchar(PG_FUNCTION_ARGS) {
  TYPE_@TYPE_NAME@ in = GETARG_@TYPE_NAME@(0);
  char *out;
  size_t len;

  len = snprintf(NULL, 0, "@SCANF_FORMAT@", in);
  out = palloc(VARHDRSZ + len);
  sprintf(out + VARHDRSZ, "@SCANF_FORMAT@", in);
  SET_VARSIZE(out, VARHDRSZ + len);
  PG_RETURN_POINTER(out);
}

Datum
tsql_cast_@TYPE_NAME@_float8(PG_FUNCTION_ARGS) {
  TYPE_@TYPE_NAME@ in = GETARG_@TYPE_NAME@(0);
  double out;

  out = in;

  PG_RETURN_FLOAT8(out);
}

