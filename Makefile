MODULE_DIR = ../cpp-mods
include $(MODULE_DIR)/config.mk

OBJDIR := obj/
CFLAGS += -O2 -g -Wall -I$(MODULE_DIR)
#CXXFLAGS := -std=c++0x
SRCDIR := src/

#SDL_AUDIO := 1

include $(MODULE_DIR)/coreutils/module.mk
include $(MODULE_DIR)/bbsutils/module.mk
include $(MODULE_DIR)/webutils/module.mk
include $(MODULE_DIR)/sqlite3/module.mk
include $(MODULE_DIR)/audioplayer/module.mk
#include $(MODULE_DIR)/json/module.mk
#include $(MODULE_DIR)/backward-cpp/module.mk
include $(MODULE_DIR)/grappix/module.mk

include src/plugins/ModPlugin/module.mk
include src/plugins/VicePlugin/module.mk
include src/plugins/GMEPlugin/module.mk
include src/plugins/SexyPSFPlugin/module.mk
include src/plugins/SC68Plugin/module.mk
include src/plugins/UADEPlugin/module.mk

TARGET := chipmachine
LOCAL_FILES += main.cpp MusicPlayer.cpp
LIBS += -lz

USE_CCACHE=1

#CFLAGS += -I$(PI)/usr/include -I$(PI)/vc/include
#LDFLAGS += -L$(PI)/lib -L$(PI)/lib/arm-linux-gnueabihf
#LDFLAGS += -L$(PI)/usr/lib -L$(PI)/usr/lib/arm-linux-gnueabihf

# TelnetInterface.cpp SongDb.cpp SearchIndex.cpp URLPlayer.cpp

#CC=ccache clang -Qunused-arguments
#CXX=ccache clang++ -Qunused-arguments
#AR=llvm-ar
include $(MODULE_DIR)/build.mk
