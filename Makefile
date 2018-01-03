include Makefile.common

.SUFFIXES:

OBJS := \
  bgworker/bgworker.o \
  language/language.o \
  tds/tds.o \
  types/types.o

.PHONY: bgworker/bgworker.o
.PHONY: language/language.o
.PHONY: tds/tds.o
.PHONY: types/types.o

all: tdspsql.so

tdspsql.so: $(OBJS)
	$(CC) -shared $^ -o $@ -lgmp -licuuc -licui18n

%.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) -c $<

bgworker/bgworker.o:
	make -C bgworker bgworker.o

language/language.o:
	make -C language language.o

tds/tds.o:
	make -C tds tds.o

types/types.o:
	make -C types types.o

clean:
	make -C bgworker clean
	make -C language clean
	make -C tds clean
	make -C types clean
	rm -f tdspsql.so
