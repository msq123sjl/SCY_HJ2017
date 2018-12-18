TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make

OBJS = readmeter.o protocol.o

all: $(OBJS)
	$(CC)  -lm -lpthread -L$(LIBDIR) -lcommon -o $(BINDIR)/readmeter $(OBJS)

clean:
	rm -f *.o $(BINDIR)/readmeter *~ core