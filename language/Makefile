include ../Makefile.common

HEADERS := \
  scan.l.h \
  parse.y.h

OBJS := \
  parse.y.o \
  scan.l.o \
  exec.o \
  tsqlfunc.o \
  tsqlproc.o \
  misc.o

CFLAGS := $(CFLAGS) -I../include

all: language.o

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

language.o: $(OBJS)
	$(LD) -r $^ -o $@

parse.y.cc parse.y.h: parse.y
	$(YACC) $<

scan.l.cc scan.l.h: scan.l
	$(LEX) $<

clean:
	rm -f *.o
	rm -f *.l.cc *.l.h
	rm -f *.y.cc *.y.h
