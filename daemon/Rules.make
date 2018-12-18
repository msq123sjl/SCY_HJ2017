# -*-makefile-*-
#
# 各个应用程序的makefile 的通用定义
#
# TOPDIR is declared by the Makefile including this file.
ifndef TOPDIR
	TOPDIR = .
endif

ifndef BINDIR
	BINDIR = $(TOPDIR)/bin
endif

ifndef LIBDIR
	LIBDIR = $(TOPDIR)/lib
endif

# KERNELDIR can be speficied on the command line or environment
ifndef KERNELDIR
	KERNELDIR = /usr/src/linux
endif
# The headers are taken from the kernel
	INCLUDEDIR = $(KERNELDIR)/include


ifndef CROSS_COMPILE
	CROSS_COMPILE=arm-none-linux-gnueabi-
	#CROSS_COMPILE=
endif

# This is useful if cross-compiling. Taken from kernel Makefile (CC changed)
AS      =$(CROSS_COMPILE)as
LD      =$(CROSS_COMPILE)ld
CC      =$(CROSS_COMPILE)gcc
CPP     =$(CC) -E
AR      =$(CROSS_COMPILE)ar
NM      =$(CROSS_COMPILE)nm
STRIP   =$(CROSS_COMPILE)strip
OBJCOPY =$(CROSS_COMPILE)objcopy
OBJDUMP =$(CROSS_COMPILE)objdump

# CFLAGS: all assignments to CFLAGS are inclremental, so you can specify
# the initial flags on the command line or environment, if needed.

	CFLAGS += -g -Wall -I$(TOPDIR)/common 
