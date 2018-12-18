TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make

OBJS = tinz_pub_serial.o tinz_common_helper.o tinz_pub_shm.o tinz_pub_message.o tinz_common_db_helper.o config_file.o nginx_helper.o
 
all: $(OBJS)
	$(CC) -o $(LIBDIR)/libcommon.so $(OBJS) -L$(LIBDIR) -fPIC -shared -lsqlite3 -lm
	cp -rf  $(LIBDIR)/libcommon.so $(BINDIR)

clean:
	rm -f *.o $(LIBDIR)/libcommon.so  *~ core
	rm -f $(BINDIR)/libcommon.so