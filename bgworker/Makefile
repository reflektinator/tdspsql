include ../Makefile.common

HEADERS := \
  bgworker.h

OBJS := \
  init.o

all: bgworker.o

bgworker.o: $(OBJS)
	$(LD) -r $^ -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o
	rm -f *.so
	rm -f core
