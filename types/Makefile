include ../Makefile.common

# disable builtin rules
.SUFFIXES:

HEADERS :=

OBJS := \
  tsql_varchar.o \
  tsql_nvarchar.o \
  tsql_varbinary.o \
  tsql_int_type.t.o \
  tsql_int_ops.t.o \
  tsql_int_casts.t.o \
  tsql_decimal.o \
  tsql_datetime.o \
  misc.o

SED := sed

#CFLAGS := -Wall -std=c++11 -g -I/usr/local/pgsql/include/server -fpic

all: types.o

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

tsql_int%.t.c: tsql_int_type.t tsql_int_ops.t tsql_int_casts.t
	cat tsql_int.t >tsql_int_type.t.c ; \
	cat tsql_int.t >tsql_int_ops.t.c ; \
	cat tsql_int.t >tsql_int_casts.t.c ; \
	for type1 in bit tinyint smallint int bigint real float; do \
	  case $$type1 in \
	    bit) ctype1=uint8_t ; argtype1=CHAR ; scanf1="%hhd" ; pri1=1 ;; \
	    tinyint) ctype1=uint8_t ; argtype1=CHAR ; scanf1="%hhd" ; pri1=2 ;; \
	    smallint) ctype1=int16_t ; argtype1=INT16 ; scanf1="%hd" ; pri1=3 ;; \
	    int) ctype1=int32_t ; argtype1=INT32 ; scanf1="%d" ; pri1=4 ;; \
	    bigint) ctype1=int64_t ; argtype1=INT64 ; scanf1="%ld" ; pri1=5 ;; \
	    real) ctype1=float ; argtype1=FLOAT4 ; scanf1="%f" ; pri1=6 ;; \
	    float) ctype1=double ; argtype1=POINTER ; scanf1="%lf" ; pri1=7 ;; \
	  esac ; \
	  sed "s/@C_TYPE@/$$ctype1/g;s/@TYPE_NAME@/$$type1/g;s/@GETARG_TYPE@/$$argtype1/g;s/@SCANF_FORMAT@/$$scanf1/g" <tsql_int_type.t >>tsql_int_type.t.c ; \
	  for type2 in bit tinyint smallint int bigint real float; do \
	    case $$type2 in \
	      bit) ctype2=uint8_t ; argtype2=CHAR ; pri2=1 ;; \
	      tinyint) ctype2=uint8_t ; argtype2=CHAR ; pri2=2 ;; \
	      smallint) ctype2=int16_t ; argtype2=INT16 ; pri2=3 ;; \
	      int) ctype2=int32_t ; argtype2=INT32 ; pri2=4 ;; \
	      bigint) ctype2=int64_t ; argtype2=INT64 ; pri2=8 ;; \
	      real) ctype2=float ; argtype2=FLOAT4 ; pri1=6 ;; \
	      float) ctype2=double ; argtype2=POINTER ; pri1=7 ;; \
	    esac ; \
	    if [ $$pri1 -lt $$pri2 ] ; then \
 	      type3=$$type2 ; ctype3=$$ctype2 ; argtype3=$$argtype2 ; \
	    else \
 	      type3=$$type1 ; ctype3=$$ctype1 ; argtype3=$$argtype1 ; \
            fi ; \
	    cat <tsql_int_ops.t | \
	    sed "s/@C_TYPE1@/$$ctype1/g;s/@TYPE_NAME1@/$$type1/g;s/@GETARG_TYPE1@/$$argtype1/g" | \
	    sed "s/@C_TYPE2@/$$ctype2/g;s/@TYPE_NAME2@/$$type2/g;s/@GETARG_TYPE2@/$$argtype2/g" | \
	    sed "s/@C_TYPE3@/$$ctype3/g;s/@TYPE_NAME3@/$$type3/g;s/@GETARG_TYPE3@/$$argtype3/g" >>tsql_int_ops.t.c ; \
	    if [ $$type1 != $$type2 ] ; then \
	      cat <tsql_int_casts.t | \
	      sed "s/@C_TYPE1@/$$ctype1/g;s/@TYPE_NAME1@/$$type1/g;s/@GETARG_TYPE1@/$$argtype1/g" | \
	      sed "s/@C_TYPE2@/$$ctype2/g;s/@TYPE_NAME2@/$$type2/g;s/@GETARG_TYPE2@/$$argtype2/g" | \
	      sed "s/@C_TYPE3@/$$ctype3/g;s/@TYPE_NAME3@/$$type3/g;s/@GETARG_TYPE3@/$$argtype3/g" >>tsql_int_casts.t.c ; \
	    fi ; \
	  done ; \
	done

types.o: $(OBJS)
	$(LD) -r $^ -o $@

clean:
	rm -f *.o
	rm -f *.t.c
