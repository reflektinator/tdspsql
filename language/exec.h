#ifndef _EXEC_H
#define _EXEC_H

#include "../tdspsql.h"

#define TSQL_XTYPE_CHAR      175
#define TSQL_XTYPE_VARCHAR   167
#define TSQL_XTYPE_NCHAR     239
#define TSQL_XTYPE_NVARCHAR  231
#define TSQL_XTYPE_VARBINARY 165
#define TSQL_XTYPE_BIT       104
#define TSQL_XTYPE_TINYINT    48
#define TSQL_XTYPE_SMALLINT   52
#define TSQL_XTYPE_INT        56
#define TSQL_XTYPE_BIGINT    127
#define TSQL_XTYPE_REAL       59
#define TSQL_XTYPE_FLOAT      62
#define TSQL_XTYPE_DECIMAL   106
#define TSQL_XTYPE_DATETIME  61

class tsql_type {
private:
public:
  int oid;
  int xtype;
  string name;
  uint16_t size;
  int byval;
  char storage;
  tsql_type() {
    oid = -1;
    xtype = -1;
    name = "INVALID";
  }
  tsql_type(int oid, int xtype, string name, uint16_t size, int byval, char storage) {
    this->oid = oid;
    this->xtype = xtype;
    this->name = name;
    this->size = size;
    this->byval = byval;
    this->storage = storage;
  }
};

class tsql_types {
public:
  bool initialised;
  map<Oid, tsql_type> by_oid;
  map<string, tsql_type> by_name;

  tsql_types() { initialised = 0; }
  void add(Oid oid, int xtype, string name, uint16_t size, int byval, char storage) {
    by_oid[oid] = tsql_type(oid, xtype, name, size, byval, storage);
    by_name[name] = tsql_type(oid, xtype, name, size, byval, storage);
  }
};

class tsql_results {
  SPITupleTable *tuptable;
  int rows;

public:
  tsql_results(SPITupleTable *tuptable, uint32_t processed) { 
    this->tuptable = tuptable;
    rows = processed;
  }
  ~tsql_results() {
    /* nothing to do here. SPI_finish releases the tuptable */
  }
  int is_error() { return 0; }
  char *error_message() { return strdup(""); }
  int col_count() { return tuptable->tupdesc->natts; }
  int row_count() { return rows; }
  int col_type(int col) {
    return SPI_gettypeid(tuptable->tupdesc, col + 1);
  }
  int32_t col_typmod(int col) {
    return tuptable->tupdesc->attrs[col]->atttypmod;
  }
  int32_t col_size(int col) {
    return tuptable->tupdesc->attrs[col]->attlen;
  }
  char *col_name(int col) {
    return tuptable->tupdesc->attrs[col]->attname.data;
  }
  Datum field_datum(int row, int col, bool *isnull) {
    return SPI_getbinval(tuptable->vals[row], tuptable->tupdesc, col + 1, isnull);
  }
  char *field_value(int row, int col) {
    bool isnull;
    int offset = 0;
    Datum result = SPI_getbinval(tuptable->vals[row], tuptable->tupdesc, col + 1, &isnull);
    if (isnull) {
pprintf(WARNING, "is null");
      return NULL;
    }
    if (tuptable->tupdesc->attrs[col]->attbyval) {
pprintf(WARNING, "byval");
      //return (char *)(uintptr_t)((*(uint32_t *)result) & ((1 << (tuptable->tupdesc->attrs[col]->attlen << 3)) - 1));
      char *result2 = (char *)palloc(sizeof(int32_t));
      *(int32_t *)result2 = (int32_t)(intptr_t)result;
      return result2;
    }
    if (tuptable->tupdesc->attrs[col]->attlen == (int16_t)-1) {
      offset = VARHDRSZ;
    }
    if (tuptable->tupdesc->attrs[col]->attstorage == 'p') {
pprintf(WARNING, "plain");
      return ((char *)result) + offset;
    } else {
pprintf(WARNING, "non-plain");
pprintf(WARNING, "___%s___\n", ((char *)PG_DETOAST_DATUM_COPY(result)) + offset);
      return ((char *)PG_DETOAST_DATUM_COPY(result)) + offset;
    }
  }
  int field_length(int row, int col) {
    if (tuptable->tupdesc->attrs[col]->attlen != (int16_t)-1) {
      return tuptable->tupdesc->attrs[col]->attlen;
    } else {
      bool isnull;
      Datum result = SPI_getbinval(tuptable->vals[row], tuptable->tupdesc, col + 1, &isnull);
      if (isnull) {
        return 0;
      } else {
        return VARSIZE(PG_DETOAST_DATUM(result)) - VARHDRSZ;
      }
    }
  }
};


class tsql_var {
protected:
  static Datum copy_datum(const Datum value, const tsql_type &type, const bool isnull) {
    if (isnull || type.byval) {
      return value;
    } else if (type.size != -1) {
      Datum ret = (Datum)malloc(type.size);
      memcpy((void *)ret, (void *)value, type.size);
      return ret;
    } else {
      Datum ret = (Datum)malloc(VARSIZE(value));
      memcpy((void *)ret, (void *)value, VARSIZE(value));
      return ret;
    }
  }
public:
  tsql_type type;
  Datum value;
  uint32_t typmod;
  bool isnull;

  tsql_var() {
    type = tsql_type();
    isnull = 1;
    
  }
  tsql_var(const tsql_var &r) {
    type = r.type;
    typmod = r.typmod;
    isnull = r.isnull;
    value = copy_datum(r.value, r.type, r.isnull);
  }
  tsql_var(Oid type) {
    this->type = types.by_oid[type];
    typmod = -1;
    value = (Datum)NULL;
    isnull = 1;
  }
  tsql_var(tsql_results &results, int row, int col) {
    type = types.by_oid[results.col_type(col)];
    typmod = results.col_typmod(col);
    value = results.field_datum(row, col, &isnull);
    value = copy_datum(value, type, isnull);
  }
#if 0
  tsql_var(int oid, const Datum value) {
    this->type = types.by_oid[oid];
    this->value = copy_datum(value, this->type, this->isnull);
  }
#endif
  ~tsql_var() {
    if (value && !type.byval && !isnull) {
      free((void *)value);
    }
  }
  tsql_var &operator = (tsql_var r) {
    type = r.type;
    isnull = r.isnull;
    value = copy_datum(r.value, r.type, r.isnull);
    typmod = r.typmod;
    return *this;
  }
};

class tsql_var_int : public tsql_var {
public:
  tsql_var_int(int value) {
    this->type = types.by_name["int"];
    this->typmod = -1;
    this->value = (Datum)value;
    this->isnull = 0;
  }
};

class tsql_var_varchar : public tsql_var {
public:
  tsql_var_varchar(string value) {
    this->type = types.by_name["varchar"];
pprintf(WARNING, "varchar type = %d\n", this->type.oid);
    this->typmod = value.size();
    this->value = (Datum)malloc(value.size() + VARHDRSZ);
    this->isnull = 0;
    memcpy((void *)(this->value + VARHDRSZ), value.data(), value.size());
    SET_VARSIZE(this->value, value.size() + VARHDRSZ);
  }
};

class tsql_var_nvarchar : public tsql_var {
public:
  tsql_var_nvarchar(uint32_t length, char *value) {
    this->type = types.by_name["nvarchar"];
    this->typmod = length / 2;
    this->value = (Datum)malloc(length + VARHDRSZ);
    this->isnull = 0;
    memcpy((void *)(this->value + VARHDRSZ), value, length);
    SET_VARSIZE(this->value, length + VARHDRSZ);
  }
};

class tsql_executor {
protected:
public:
  //tsql_types types;
  int nocount;
  int fmtonly;
  map<string, tsql_var> sess_vars;
  string username;
  string database;
  tdspacket_tokenised *stpt;
  tsql_executor() {
    nocount = 0;
    fmtonly = 0;
  }
  int init();
  /* return results to caller (should only be small number of results) */
  tsql_results *exec_int(string sql, vector<string>vars, map<string, tsql_var>prog_vars);
  /* send results to client */
  int exec_ext(string sql, vector<string>vars, map<string, tsql_var>prog_vars);
  /* return boolean */
  int exec_bool(string expr, vector<string>vars, map<string, tsql_var>prog_vars, int *result);
  /* command - expect no results */
  int exec_cmd(string sql, vector<string>vars, map<string, tsql_var>prog_vars);
  /* empty query - might still have to report something to the client though(?) */
  int exec_nul();
  /* send error to client error */
  void error(uint32_t error_number, uint8_t error_state, uint8_t error_severity, const char *message);
  /* send info to client info */
  void info(uint32_t info_number, uint8_t info_state, uint8_t info_severity, const char *message);
  /* send set database info to client */
  void set_database(string database);
};

#endif
