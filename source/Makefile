.PHONY: all clean

PROJECT_DIR = $(PWD)

CONFIG_UCLIBC_BUILD=y

CROSS_COMPILE ?= /media/hnh99/Ubuntu20/sdk_t31/mips/mips-gcc472-glibc216-64bit/bin/mips-linux-gnu-

CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

ifeq ($(CONFIG_UCLIBC_BUILD), y)
CFLAGS += -muclibc
LDFLAG += -muclibc
endif

ifeq ($(CONFIG_UCLIBC_BUILD), y)
SDK_LIB_DIR = ./../lib/uclibc
else
SDK_LIB_DIR = ./../lib/glibc
endif

SDK_T31_DIR = ./../include/t31
SDK_IMP_DIR = ./../include/t31/imp
INC_ZBAR_DIR = ./../include/zbar
INC_JPEG_DIR = ./../include/jpeg
INC_SDL_DIR = ./../include/sdl
INC_LIVE555_1 = ./../include/usageEnvironment
INC_LIVE555_2 = ./../include/groupsock
INC_LIVE555_3 = ./../include/liveMedia
INC_LIVE555_4 = ./../include/basicUsageEnvironment


LIB_DIR = ./../lib

OBJ_DIR = $(PROJECT_DIR)/_build

-include $(PROJECT_DIR)/qrcode/Makefile.mk
-include $(PROJECT_DIR)/video/Makefile.mk
-include $(PROJECT_DIR)/osd/Makefile.mk
-include $(PROJECT_DIR)/audio/Makefile.mk
-include $(PROJECT_DIR)/led_state/Makefile.mk
-include $(PROJECT_DIR)/rtspd/Makefile.mk

OBJ += $(OBJ_DIR)/app.o

INCLUDES += -I$(SDK_LIB_DIR)
INCLUDES += -I$(SDK_IMP_DIR)
INCLUDES += -I$(SDK_T31_DIR)

INCLUDES += -I$(INC_ZBAR_DIR)
INCLUDES += -I$(INC_JPEG_DIR)
INCLUDES += -I$(INC_SDL_DIR)
INCLUDES += -I$(INC_LIVE555_1)
INCLUDES += -I$(INC_LIVE555_2)
INCLUDES += -I$(INC_LIVE555_3)
INCLUDES += -I$(INC_LIVE555_4)

INCLUDES += -L$(LIB_DIR)

LIBS = $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so \
		$(SDK_LIB_DIR)/libsysutils.so $(LIB_DIR)/libzbar.a \
		$(LIB_DIR)/libjpeg.a $(LIB_DIR)/libSDL.a $(LIB_DIR)/libSDL_ttf.a \
		$(LIB_DIR)/libfreetype.a $(LIB_DIR)/libz.a \
		$(LIB_DIR)/livelib/libliveMedia.a $(LIB_DIR)/livelib/libgroupsock.a $(LIB_DIR)/livelib/libBasicUsageEnvironment.a \
		$(LIB_DIR)/livelib/libUsageEnvironment.a


LINK 		 =  $(CROSS_COMPILE)g++ -o
COMPILE_OPTS =      $(INCLUDES) -I. -O2 -Wall -march=mips32r2 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DLOCALE_NOT_USED -g
CPLUSPLUS_FLAGS = $(COMPILE_OPTS) -Wall -DBSD=1 $(CPPFLAGS) $(CXXFLAGS)
C_FLAGS = $(COMPILE_OPTS) $(CPLUSPLUS_FLAGS) $(CFLAGS) -O2 -Wall -march=mips32r2 -limp -lalog -lsysutils
LDFLAG += -Wl,-gc-sections


APP = $(OBJ_DIR)/app-t31

RED="\033[1;31m"
GREEN="\033[3;32m"
BLUE="\033[3;34m"
YELLO="\033[1;33m"
NONE="\033[0m"

commit_tag=$(shell git rev-parse --short HEAD)

all: create version $(APP) 

version :
	@if  ! grep "$(commit_tag)" version.h >/dev/null ; then                   \
        echo "update version.h"       ;    \
        sed 's/COMMIT_TAG/"$(commit_tag)"/g' version.tpl.h > version.h     ;  \
    fi

create:
	@mkdir -p $(OBJ_DIR)

copy: 
# 	@cp -r $(APP) /mnt/nfs_share/t31_ingenic
# 	@echo $(BLUE) "link->/mnt/nfs_share/t31_ingenic"

clean:
	rm -rf $(OBJ_DIR)/*

distclean: clean
	rm -rf $(OBJ_DIR)/$(APP)

$(APP): $(LIBS) $(OBJ)
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

$(OBJ_DIR)/%.o:%.c 
	@echo $(GREEN) CC  $< $(NONE)
	@$(CC) -o $@ -c $< $(C_FLAGS)

$(OBJ_DIR)/%.o:%.cpp  
	@echo $(GREEN) CPLUSPLUS  $< $(NONE)
	@$(CPLUSPLUS) -o $@ -c $< $(C_FLAGS)



