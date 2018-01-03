#include "../tdspsql.h"

tdstoken_results::tdstoken_results(tsql_results *results, tsql_types *types) : tdstoken(0xFF) {
  this->results = results;
  this->types = types;
};

size_t
tdstoken_results::writetoken(dynbuffer *buffer, uint32_t tdsversion) {
  /* write our own token byte */
  buffer->add_u8(0x81);
  buffer->add_hu16((uint16_t)results->col_count());
  for (int i = 0; i < results->col_count(); i++) {
printf("column %d:\n", i);
printf("  type = %d\n", results->col_type(i));
printf("  mod = %d\n", results->col_typmod(i));
printf("  size = %d\n", results->col_size(i));
    if (tdsversion >= 0x72000000) {
      buffer->add_hu32(0); /* user type */
    } else {
      buffer->add_hu16(0); /* user type */
    }
    tsql_type type = types->by_oid[(int)results->col_type(i)];

    switch(type.xtype) {
    case TSQL_XTYPE_CHAR:
      buffer->add_hu16(0x0021); /* flags */
      buffer->add_u8(TSQL_XTYPE_CHAR);
      if (tdsversion < 0x72000000 && results->col_typmod(i) == -1) {
        buffer->add_hu16(8000);
      } else {
        buffer->add_hu16(results->col_typmod(i));
      }
      buffer->add_hu16(1033); /* codepage */
      buffer->add_hu16(0xd0); /* flags - don't know what this is */
      buffer->add_u8(0x00); /* charset id */
      break;
    case TSQL_XTYPE_VARCHAR:
      buffer->add_hu16(0x0021); /* flags */
      buffer->add_u8(TSQL_XTYPE_VARCHAR);
      if (tdsversion < 0x72000000 && results->col_typmod(i) == -1) {
        buffer->add_hu16(8000);
      } else {
        buffer->add_hu16(results->col_typmod(i));
      }
      buffer->add_hu16(1033); /* codepage */
      buffer->add_hu16(0xd0); /* flags - don't know what this is */
      buffer->add_u8(0x00); /* charset id */
      break;
    case TSQL_XTYPE_NVARCHAR:
      buffer->add_hu16(0x0000); /* flags */
      buffer->add_u8(TSQL_XTYPE_NVARCHAR);
      if (tdsversion < 0x72000000 && results->col_typmod(i) == -1) {
        buffer->add_hu16(8000);
      } else {
        buffer->add_hu16(results->col_typmod(i));
      }
      buffer->add_hu16(1033); /* codepage */
      buffer->add_hu16(0xd0); /* flags - don't know what this is */
      buffer->add_u8(0x00); /* charset id */
      break;
    case TSQL_XTYPE_VARBINARY:
      buffer->add_hu16(0x0021); /* flags */
      buffer->add_u8(TSQL_XTYPE_VARBINARY);
      if (tdsversion < 0x72000000 && results->col_typmod(i) == -1) {
        buffer->add_hu16(8000);
      } else {
        buffer->add_hu16(results->col_typmod(i));
      }
      break;
    case TSQL_XTYPE_BIT:
      buffer->add_hu16(0x0001); /* flags */
      buffer->add_u8(TSQL_XTYPE_BIT);
      buffer->add_u8(0x01);
      break;
    case TSQL_XTYPE_TINYINT:
      buffer->add_hu16(0x0001); /* flags */
      buffer->add_u8(0x26); /* nullable int type? */
      buffer->add_u8(0x01);
      break;
    case TSQL_XTYPE_SMALLINT:
      buffer->add_hu16(1); /* flags */
      buffer->add_u8(0x26); /* nullable int type? */
      buffer->add_u8(0x02);
      break;
    case TSQL_XTYPE_INT:
      buffer->add_hu16(1); /* flags */
      buffer->add_u8(0x26); /* nullable int type? */
      buffer->add_u8(0x04);
      break;
    case TSQL_XTYPE_BIGINT:
      buffer->add_hu16(1); /* flags */
      buffer->add_u8(0x26); /* nullable int type? */
      buffer->add_u8(0x08);
      break;
    case TSQL_XTYPE_REAL:
      buffer->add_hu16(1); /* flags */
      buffer->add_u8(0x6d); /* nullable int type? */
      buffer->add_u8(0x04);
      break;
    case TSQL_XTYPE_FLOAT:
      buffer->add_hu16(1); /* flags */
      buffer->add_u8(0x6d); /* nullable int type? */
      buffer->add_u8(0x08);
      break;
    case TSQL_XTYPE_DECIMAL:
      printf("***DECIMAL***\n");
      buffer->add_hu16(0); /* flags */
      buffer->add_u8(TSQL_XTYPE_DECIMAL);
      buffer->add_u8(0x11); /* ? */
printf("decimal (%d, %d)\n", results->col_typmod(i) >> 16, ((results->col_typmod(i) & 0xFFFF) - 4));
      buffer->add_u8(results->col_typmod(i) >> 16); /* size */
      buffer->add_u8((results->col_typmod(i) & 0xFFFF) - 4); /* precision */
      break;
    case TSQL_XTYPE_DATETIME:
      // NOT NULL type is 61, NULLABLE type is 111
      buffer->add_hu16(0x0021); /* flags */
      buffer->add_u8(111); /* NULLable datetime */
      buffer->add_u8(8); /* length */
      break;
      
      break;
    default:
      printf("Unknown type %d\n", results->col_type(i));
      break;
    }
    buffer->add_cc1_ucs2(results->col_name(i));
  }
  for (int i = 0; i < results->row_count(); i++) {
    buffer->add_u8(0xd1);
pprintf(WARNING, "row = %d", i);
    for (int j = 0; j < results->col_count(); j++) {
      tsql_type type = types->by_oid[(int)results->col_type(j)];
pprintf(WARNING, "col = %d, type = %hhd", j, type.xtype);
      switch(type.xtype) {
      case TSQL_XTYPE_CHAR: {
#if 0
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (isnull) {
          buffer->add_bytes(NULL, 0);
        } else {
          data = (Datum)PG_DETOAST_DATUM(data);
          buffer->add_bytes(((char *)data) + VARHDRSZ, VARSIZE(data) - VARHDRSZ);
        }
#endif
        break; }
      case TSQL_XTYPE_VARCHAR: {
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (tdsversion >= 0x72000000 && results->col_typmod(j) == -1) {
          if (isnull) {
pprintf(WARNING, "isnull");
            buffer->add_hu32(0xFFFFFFFF);
            buffer->add_hu32(0xFFFFFFFF);
          } else {
            data = (Datum)PG_DETOAST_DATUM(data);
            buffer->add_hu32(VARSIZE(data) - VARHDRSZ);
            buffer->add_hu32(0); // ?
            buffer->add_hu32(VARSIZE(data) - VARHDRSZ);
            buffer->add_bytes(((char *)data) + VARHDRSZ, VARSIZE(data) - VARHDRSZ);
            buffer->add_hu32(0); // ?
          }
        } else {
          if (isnull) {
            buffer->add_hu16(0xFFFF);
          } else {
            data = (Datum)PG_DETOAST_DATUM(data);
            buffer->add_hu16(VARSIZE(data) - VARHDRSZ);
            buffer->add_bytes(((char *)data) + VARHDRSZ, VARSIZE(data) - VARHDRSZ);
          }
        }
        break; }
      case TSQL_XTYPE_NVARCHAR: {
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (tdsversion >= 0x72000000 && results->col_typmod(j) == -1) {
          if (isnull) {
            buffer->add_hu32(0xFFFFFFFF);
            buffer->add_hu32(0xFFFFFFFF);
          } else {
            data = (Datum)PG_DETOAST_DATUM(data);
            buffer->add_hu32(VARSIZE(data) - VARHDRSZ);
            buffer->add_hu32(0); // ?
            buffer->add_hu32(VARSIZE(data) - VARHDRSZ);
            buffer->add_bytes(((char *)data) + VARHDRSZ, VARSIZE(data) - VARHDRSZ);
            buffer->add_hu32(0); // ?
          }
        } else {
          if (isnull) {
            buffer->add_hu16(0xFFFF);
          } else {
            data = (Datum)PG_DETOAST_DATUM(data);
            buffer->add_hu16(VARSIZE(data) - VARHDRSZ);
            buffer->add_bytes(((char *)data) + VARHDRSZ, VARSIZE(data) - VARHDRSZ);
          }
        }
        break; }
      case TSQL_XTYPE_VARBINARY: {
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (tdsversion >= 0x72000000 && results->col_typmod(j) == -1) {
          if (isnull) {
            buffer->add_hu32(0xFFFFFFFF);
            buffer->add_hu32(0xFFFFFFFF);
          } else {
            data = (Datum)PG_DETOAST_DATUM(data);
            buffer->add_hu32(VARSIZE(data) - VARHDRSZ);
            buffer->add_hu32(0); // ?
            buffer->add_hu32(VARSIZE(data) - VARHDRSZ);
            buffer->add_bytes(((char *)data) + VARHDRSZ, VARSIZE(data) - VARHDRSZ);
            buffer->add_hu32(0); // ?
          }
        } else {
          if (isnull) {
            buffer->add_hu16(0xFFFF);
          } else {
            data = (Datum)PG_DETOAST_DATUM(data);
            buffer->add_hu16(VARSIZE(data) - VARHDRSZ);
            buffer->add_bytes(((char *)data) + VARHDRSZ, VARSIZE(data) - VARHDRSZ);
          }
        }
        break; }
      case TSQL_XTYPE_BIT: {
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (isnull) {
          buffer->add_u8(0);
        } else {
          buffer->add_u8(1);
          buffer->add_u8((uint8_t)data);
        }
        break; }
      case TSQL_XTYPE_TINYINT: {
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (isnull) {
          buffer->add_u8(0);
        } else {
          buffer->add_u8(1);
          buffer->add_u8((uint8_t)data);
        }
        break; }
      case TSQL_XTYPE_SMALLINT: {
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (isnull) {
          buffer->add_u8(0);
        } else {
          buffer->add_u8(2);
          buffer->add_hu16((uint16_t)data);
        }
        break; }
      case TSQL_XTYPE_INT: {
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (isnull) {
          buffer->add_u8(0);
        } else {
          buffer->add_u8(4);
          buffer->add_hu32((uint32_t)data);
        }
        break; }
      case TSQL_XTYPE_BIGINT: {
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (isnull) {
          buffer->add_u8(0);
        } else {
          buffer->add_u8(8);
          buffer->add_bytes((char *)data, 8);
        }
        break; }
      case TSQL_XTYPE_REAL: {
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (isnull) {
          buffer->add_u8(0);
        } else {
          buffer->add_u8(4);
          buffer->add_hu32((uint8_t)data);
        }
        break; }
      case TSQL_XTYPE_FLOAT: {
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (isnull) {
          buffer->add_u8(0);
        } else {
          buffer->add_u8(8);
          buffer->add_bytes((char *)data, 8);
        }
        break; }
      case TSQL_XTYPE_DECIMAL:
        break;
      case TSQL_XTYPE_DATETIME: {
        bool isnull;
        Datum data = results->field_datum(i, j, &isnull);
        if (isnull) {
          buffer->add_u8(0);
        } else {
          buffer->add_u8(8);
          buffer->add_bytes((char *)data, 8);
        }
        break; }
      }
    }
  }
printf("finished adding results token\n");
  return 0;
}

