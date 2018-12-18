TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make


OBJS = dataproc.o


all: $(OBJS)
	$(CC) -o $(BINDIR)/dataproc $(OBJS) -L$(LIBDIR) -lcommon -lsqlite3 -lm -lpthread

clean:
	rm -f *.o $(BINDIR)/dataproc *~ core