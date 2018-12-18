TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make


OBJS = up_main.o tcp_client.o protocl_gb212.o


all: $(OBJS)
	$(CC)  -lm -lpthread -L$(LIBDIR) -lcommon -o $(BINDIR)/up_main $(OBJS)

clean:
	rm -f *.o $(BINDIR)/up_main *~ core