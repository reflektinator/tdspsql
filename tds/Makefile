include ../Makefile.common

HEADERS := \
  util.h \
  tdspacket.h \
  tdspacket_prelogin.h \
  tdspacket_login.h \
  tdspacket_tokenised.h \
  tdspacket_sql.h \
  tdspacket_rpc.h \
  tdstoken.h \
  tdstoken_loginack.h \
  tdstoken_results.h \
  tdstoken_error.h \
  tdstoken_info.h \
  tdstoken_env_database.h \
  tdstoken_env_collation.h \
  tdstoken_done.h \
  dynbuffer.h \
  tdsmain.h

OBJS := \
  tdspacket.o \
  tdspacket_prelogin.o \
  tdspacket_login.o \
  tdspacket_tokenised.o \
  tdspacket_sql.o \
  tdspacket_rpc.o \
  tdstoken.o \
  tdstoken_loginack.o \
  tdstoken_results.o \
  tdstoken_error.o \
  tdstoken_info.o \
  tdstoken_env_database.o \
  tdstoken_env_collation.o \
  tdstoken_done.o \
  tdsmain.o \
  dynbuffer.o

#CFLAGS := $(CFLAGS)

all: tds.o

tds.o: $(OBJS)
	$(LD) -r $^ -o $@

%.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o
	rm -f *.so
	rm -f core
