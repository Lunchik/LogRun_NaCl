
#@file Makefile
#@author garipova.nailya@tgtoil.com
#@author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.


THIS_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
NACL_SDK_ROOT?=$(abspath $(dir $(THIS_MAKEFILE))../..)
USR_BASE=$(NACL_SDK_ROOT)/toolchain/win_x86_glibc/x86_64-nacl/usr
CXXLIBS=$(USR_BASE)/lib
CXXINCLUDE=$(USR_BASE)/include
USR_BASE32=$(NACL_SDK_ROOT)/toolchain/win_x86_glibc/i686-nacl/usr
CXXLIBS32=$(USR_BASE32)/lib
CXXINCLUDE32=$(USR_BASE32)/include

CHROME_PATH?=Undefined

#
# Defaults
#
NACL_WARNINGS:=-Wno-long-long -Wall -Wswitch-enum -Werror -pedantic 
#-Wuninitialized

#
# Project Settings
#
VALID_TOOLCHAINS:=newlib glibc
TOOLCHAIN?=glibc



#
# Default target
#
all:

#
# Alias for standard commands
#
CP:=python $(NACL_SDK_ROOT)/tools/oshelpers.py cp
MKDIR:=python $(NACL_SDK_ROOT)/tools/oshelpers.py mkdir
MV:=python $(NACL_SDK_ROOT)/tools/oshelpers.py mv
RM:=python $(NACL_SDK_ROOT)/tools/oshelpers.py rm


#
# Verify we selected a valid toolchain for this example
#
ifeq (,$(findstring $(TOOLCHAIN),$(VALID_TOOLCHAINS)))
$(warning Availbile choices are: $(VALID_TOOLCHAINS))
$(error Can not use TOOLCHAIN=$(TOOLCHAIN) on this example.)
endif


#
# Compute path to requested NaCl Toolchain
#
OSNAME:=$(shell python $(NACL_SDK_ROOT)/tools/getos.py)
TC_PATH:=$(abspath $(NACL_SDK_ROOT)/toolchain)


#
# Verify we have a valid NACL_SDK_ROOT by looking for the toolchain directory
#
ifeq (,$(wildcard $(TC_PATH)))
$(warning No valid NACL_SDK_ROOT at $(NACL_SDK_ROOT))
ifeq ($(origin NACL_SDK_ROOT), 'file')
$(error Override the default value via enviornment variable, or command-line.)
else
$(error Fix the NACL_SDK_ROOT specified in the environment or command-line.)
endif
endif


#
# Disable DOS PATH warning when using Cygwin based NaCl tools on Windows
#
CYGWIN ?= nodosfilewarning
export CYGWIN


#
# Defaults for TOOLS
#

GLIBC_CC?=$(TC_PATH)/$(OSNAME)_x86_glibc/bin/i686-nacl-gcc -c
GLIBC_CXX?=$(TC_PATH)/$(OSNAME)_x86_glibc/bin/i686-nacl-g++ -c -std=gnu++98
#-IC:/Python27/pepper_22/toolchain/win_x86_glibc/x86_64-nacl/include
GLIBC_LINK?=$(TC_PATH)/$(OSNAME)_x86_glibc/bin/i686-nacl-g++ -Wl,-as-needed
GLIBC_LIB?=$(TC_PATH)/$(OSNAME)_x86_glibc/bin/i686-nacl-ar r
GLIBC_DUMP?=$(TC_PATH)/$(OSNAME)_x86_glibc/x86_64-nacl/bin/objdump
GLIBC_PATHS:=-L $(TC_PATH)/$(OSNAME)_x86_glibc/x86_64-nacl/lib32
GLIBC_PATHS+=-L $(TC_PATH)/$(OSNAME)_x86_glibc/x86_64-nacl/lib
GLIBC_CCFLAGS?=-MMD -pthread $(NACL_WARNINGS) -idirafter $(NACL_SDK_ROOT)/include
GLIBC_LDFLAGS?=-pthread
#-lppapi_cpp -lppapi -lcairo -lpixman-1 -lfontconfig -lfreetype -lexpat -lz

BASE64_CXXFLAGS=-I$(CXXINCLUDE) -L$(CXXLIBS) -lppapi_cpp -lppapi -lpthread -lnacl-mounts -lagg -laggfontfreetype -lpixman-1 -lfontconfig -lfreetype -lexpat -lz
BASE64_32_CXXFLAGS=-I$(CXXINCLUDE32) -L$(CXXLIBS32) -lppapi_cpp -lppapi -lpthread -lnacl-mounts -lagg -laggfontfreetype -lpixman-1 -lfontconfig -lfreetype -lexpat -lz

#BASE64_CXXFLAGS=-static -Wl,-T,ldscripts/elf64_nacl.x.static -I$(CXXINCLUDE) -L$(CXXLIBS) -lppapi_cpp -lppapi -lpthread -lcairo -lpixman-1 -lfontconfig -lfreetype -lexpat -lz
#BASE64_32_CXXFLAGS=-static -Wl,-T,ldscripts/elf_nacl.x.static -I$(CXXINCLUDE32) -L$(CXXLIBS32) -lppapi_cpp -lppapi -lpthread -lcairo -lpixman-1 -lfontconfig -lfreetype -lexpat -lz


#-static -Wl,-T,ldscripts/elf64_nacl.x.static

#
# NMF Manifiest generation
#
# Use the python script create_nmf to scan the binaries for dependencies using
# objdump.  Pass in the (-L) paths to the default library toolchains so that we
# can find those libraries and have it automatically copy the files (-s) to
# the target directory for us.
NMF:=python $(NACL_SDK_ROOT)/tools/create_nmf.py


#
# Verify we can find the Chrome executable if we need to launch it.
#
.PHONY: CHECK_FOR_CHROME
CHECK_FOR_CHROME:
ifeq (,$(wildcard $(CHROME_PATH)))
	$(warning No valid Chrome found at CHROME_PATH=$(CHROME_PATH))
	$(error Set CHROME_PATH via an environment variable, or command-line.)
else
	$(warning Using chrome at: $(CHROME_PATH))
endif

#
# Per target object lists
#
BASE64_OBJS:=base64 view1 XUnzip LasParser #lr_interval lr_math lr_scale_div lr_scale_engine lr_scale_map

#
# Rules for glibc toolchain
#
glibc:
	$(MKDIR) glibc

# Include header dependency files.
-include glibc/*.d



glibc/base64_x86_32.o : logrun.cc $(THIS_MAKE) | glibc
	$(GLIBC_CC) -o $@ $< -O2 -m32 $(BASE64_32_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/uu_x86_32.o : uu.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m32 $(BASE64_32_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

glibc/view1_x86_32.o : view1.cc $(THIS_MAKE) | glibc
	$(GLIBC_CC) -o $@ $< -O2 -m32 $(BASE64_32_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

glibc/XUnzip_x86_32.o : XUnzip.cc $(THIS_MAKE) | glibc
	$(GLIBC_CC) -o $@ $< -O2 -m32 $(BASE64_32_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

glibc/LasParser_x86_32.o : LasParser.cc $(THIS_MAKE) | glibc
	$(GLIBC_CC) -o $@ $< -O2 -m32 $(BASE64_32_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/lr_interval_x86_32.o : lr_interval.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m32 $(BASE64_32_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/lr_math_x86_32.o : lr_math.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m32 $(BASE64_32_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/lr_scale_div_x86_32.o : lr_scale_div.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m32 $(BASE64_32_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/lr_scale_engine_x86_32.o : lr_scale_engine.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m32 $(BASE64_32_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/lr_scale_map_x86_32.o : lr_scale_map.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m32 $(BASE64_32_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)



BASE64_GLIBC_RELEASE_x86_32_O:=$(patsubst %,glibc/%_x86_32.o,$(BASE64_OBJS))
glibc/base64_x86_32.nexe : $(BASE64_GLIBC_RELEASE_x86_32_O)
	$(GLIBC_LINK) -o $@ $^ -m32 $(BASE64_LDFLAGS) $(BASE64_32_CXXFLAGS) $(GLIBC_LDFLAGS)

glibc/base64_x86_64.o : logrun.cc $(THIS_MAKE) | glibc
	$(GLIBC_CC) -o $@ $< -O2 -m64 $(BASE64_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/uu_x86_64.o : uu.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m64 $(BASE64_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

glibc/view1_x86_64.o : view1.cc $(THIS_MAKE) | glibc
	$(GLIBC_CC) -o $@ $< -O2 -m64 $(BASE64_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

glibc/XUnzip_x86_64.o : XUnzip.cc $(THIS_MAKE) | glibc
	$(GLIBC_CC) -o $@ $< -O2 -m64 $(BASE64_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

glibc/LasParser_x86_64.o : LasParser.cc $(THIS_MAKE) | glibc
	$(GLIBC_CC) -o $@ $< -O2 -m64 $(BASE64_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/lr_interval_x86_64.o : lr_interval.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m64 $(BASE64_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/lr_math_x86_64.o : lr_math.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m64 $(BASE64_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/lr_scale_div_x86_64.o : lr_scale_div.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m64 $(BASE64_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/lr_scale_engine_x86_64.o : lr_scale_engine.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m64 $(BASE64_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)

#glibc/lr_scale_map_x86_64.o : lr_scale_map.cc $(THIS_MAKE) | glibc
#	$(GLIBC_CC) -o $@ $< -O2 -m64 $(BASE64_CXXFLAGS) -DTCNAME=glibc $(GLIBC_CCFLAGS)


BASE64_GLIBC_RELEASE_x86_64_O:=$(patsubst %,glibc/%_x86_64.o,$(BASE64_OBJS))
glibc/base64_x86_64.nexe : $(BASE64_GLIBC_RELEASE_x86_64_O)
	$(GLIBC_LINK) -o $@ $^ -m64 $(BASE64_LDFLAGS) $(BASE64_CXXFLAGS) $(GLIBC_LDFLAGS) 


ALL_TARGETS+=glibc/logrun.nmf
glibc/logrun.nmf : glibc/base64_x86_32.nexe glibc/base64_x86_64.nexe
	$(NMF) -D $(GLIBC_DUMP) -o $@ $(GLIBC_PATHS) $^ -t glibc -s glibc $(GLIBC_REMAP)

#
# Target to remove temporary files
#
.PHONY: clean
clean:
	$(RM) -fr glibc


all: $(ALL_TARGETS)




RUN: all
	python ../httpd.py
