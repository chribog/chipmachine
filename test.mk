
include settings.mk

OBJDIR := testobj/
CFLAGS := -g -Wall -I. -Iinclude -Iplugins/SexyPSFPlugin -DUNIT_TEST
CFLAGS := $(CFLAGS) -Inetlink/include -Isqlite3 -Iplugins/VicePlugin -Iplugins/ModPlugin -Iplugins/GMEPlugin

TARGET := test
MODULES := ziplib netlink/src
LIBS := -lsexypsfplugin -lviceplugin -lmodplugin -lgmeplugin -lz
LDFLAGS := -Wl,-Map -Wl,mapfile -Lplugins/SexyPSFPlugin -Lplugins/VicePlugin -Lplugins/ModPlugin -Lplugins/GMEPlugin
OBJS := catch.o TelnetServer.o TextScreen.o WebGetter.o URLPlayer.o Archive.o utils.o log.o SongDb.o sqlite3/sqlite3.o

WIN_CFLAGS := $(WIN_CFLAGS) -static -Icurl/include -DWIN32
WIN_LIBS := -lwinmm -lcurldll -lws2_32
WIN_LDFLAGS := -Lcurl/lib -static
WIN_OBJS := AudioPlayerWindows.o
WIN_CC := gcc
WIN_CXX := g++

LINUX_CFLAGS := $(LINUX_CFLAGS) `curl-config --cflags`
LINUX_LIBS := -lasound `curl-config --libs`
LINUX_OBJS := AudioPlayerLinux.o

PI_OBJS := lcd.o
PI_LIBS := -lwiringPi

#GCC_VERSION := $(subst /platform-tools/,,$(dir $(shell which adb)))

all : vice sexypsf gmeplugin modplug netlink $(TARGET)$(EXT)

run :
	./player

cleanall : clean
	make -f netlink.mk clean
	make -C plugins/VicePlugin clean
	make -C plugins/SexyPSFPlugin clean
	make -C plugins/ModPlugin clean


netlink :
	make -f netlink.mk

modplug :
	make -C plugins/ModPlugin

vice :
	make -C plugins/VicePlugin

sexypsf :
	make -C plugins/SexyPSFPlugin

gmeplugin :
	make -C plugins/GMEPlugin

include Makefile.inc