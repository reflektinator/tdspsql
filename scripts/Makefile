SED := sed
INT_FILES := \
  140_tsql_int.t.sql \
  240_tsql_int.t.sql \
  340_tsql_int.t.sql \
  341_tsql_int.t.sql

all: $(INT_FILES)

$(INT_FILES):
	rm -f 140_tsql_int.t.sql ; \
	rm -f 240_tsql_int.t.sql ; \
	rm -f 340_tsql_int.t.sql ; \
	rm -f 341_tsql_int.t.sql ; \
	for type1 in bit tinyint smallint int bigint real float; do \
	  case $$type1 in \
	    bit) align1=char ; len1=1 ; pri1=1 ;; \
	    tinyint) align1=char ; len1=1 ; pri1=2 ;; \
	    smallint) align1=int2 ; len1=2 ; pri1=3 ;; \
	    int) align1=int4 ; len1=4 ; pri1=4 ;; \
	    bigint) align1=int4 ; len1=8 ; pri1=5 ;; \
	    real) align1=int4 ; len1=4 ; pri1=6 ;; \
	    float) align1=int4 ; len1=8 ; pri1=7 ;; \
	  esac ; \
	  if [ $$len1 -gt 4 ] ; then \
	      byval= ; \
	  else \
	      byval=PASSEDBYVALUE, ; \
	  fi ; \
	  sed "s/@TYPE_NAME@/$$type1/g;s/@ALIGNMENT@/$$align1/g;s/@LENGTH@/$$len1/g;s/@BYVALUE@/$$byval/g" <tsql_int_type.t >>140_tsql_int.t.sql ; \
	  sed "s/@TYPE_NAME@/$$type1/g" <tsql_int_casts_other.t >>341_tsql_int.t.sql ; \
	  for type2 in bit tinyint smallint int bigint real float; do \
	    case $$type2 in \
	      bit) align2=char ; len1=1 ; pri2=1 ;; \
	      tinyint) align2=char ; len1=1 ; pri2=2 ;; \
	      smallint) align2=int2 ; len1=2 ; pri2=3 ;; \
	      int) align2=int4 ; len2=4 ; pri2=4 ;; \
	      bigint) align2=int4 ; len2=8 ; pri2=5 ;; \
	      real) align2=int4 ; len2=4 ; pri2=6 ;; \
	      float) align2=int4 ; len2=8 ; pri2=7 ;; \
	    esac ; \
	    if [ $$pri1 -lt $$pri2 ] ; then \
	      type3=$$type2 ; \
	      type=IMPLICIT ; \
	    else \
	      type3=$$type1 ; \
	      type=ASSIGNMENT ; \
	    fi ; \
	    sed "s/@TYPE_NAME1@/$$type1/g;s/@TYPE_NAME2@/$$type2/g;s/@TYPE_NAME3@/$$type3/g" <tsql_int_ops.t >>240_tsql_int.t.sql ; \
	    if [ $$type1 != $$type2 ] ; then \
	      sed "s/@TYPE_NAME1@/$$type1/g;s/@TYPE_NAME2@/$$type2/g;s/@CAST_TYPE@/$$type/g" <tsql_int_casts.t >>340_tsql_int.t.sql ; \
	    fi ;\
	  done ;\
	  sed "s/@TYPE_NAME1@/$$type1/g;s/@TYPE_NAME2@/$$type1/g;s/@TYPE_NAME3@/$$type1/g" <tsql_int_ops_class.t >>240_tsql_int.t.sql ; \
	done

clean:
	rm -f *.t.sql
