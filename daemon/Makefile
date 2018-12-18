TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make

OBJS = Con_Warrn.o gdcontrol.o gwcontrol.o


all: $(OBJS)
	$(CC)   -lm -lpthread -L$(LIBDIR) -lcommon -o $(BINDIR)/Con_Warrn $(OBJS)

Con_Warrn.o: Con_Warrn.c	
clean:
	rm -f *.o *.jpg Con_Warrn *~ core
