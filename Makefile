
# ------------------------------------------------------------------------------
#
# Command line macros:
#
# INSTALL_TOP ... place to put includes and library files
# LUA_HOME    ... where to find lua header and library files 
#
# If this is a local build, then you should set the environment variable
# LUA_CPATH to the local directory. On bash, this will work:
#
# export LUA_CPATH="$LUA_CPATH;`pwd`/lib/?.so"
#
# Absolute paths may be used instead, which will be useful if you are doing a
# remote install.
#
# ------------------------------------------------------------------------------



# System-specific things.
# ------------------------------------------------------------------------------

# C compiler icc, gcc, etc if other than system default
# CC = cc

# C++ compiler icpc, g++, etc if other than system default
# CXX = c++

# sometimes only -fpic works
FPIC = -fPIC

# Warning level flags
WARN = -Wall

# robust optimazation level
OPTIM = -O2

# debug flags, use -g for debug symbols
DEBUG =

# location of Lua install on this system
LUA_HOME ?= $(PWD)/lua

# where to install lunar library and include
INSTALL_TOP = $(PWD)

# C Flags
CFLAGS = $(WARN) $(OPTIM) $(DEBUG) $(FPIC)


# Configuration for common platforms. If you need to use a different linker,
# archiver, or C libraries then uncomment the UNAME = Custom line below, and
# edit the custom first block following.
# ------------------------------------------------------------------------------
UNAME = $(shell uname)
#UNAME = Custom
# ------------------------------------------------------------------------------
#
#
ifeq ($(UNAME), Custom)
# common for library links on Linux
CLIBS = -lm -ldl
# command for building shared libraries (this works for most Linux systems)
SO = $(CC) -O -shared
# command for generating static archives
AR = ar rcu
endif

ifeq ($(UNAME), Linux)
SO     = $(CC) -O -shared
AR     = ar rcu
CLIBS  = -lm -ldl
endif

ifeq ($(UNAME), Darwin)
SO     = $(CC) -O -bundle -undefined dynamic_lookup
AR     = ar rcu
CLIBS  =
endif



# -------------------------------------------------
# Ensure these values are passed to child Makefiles
# -------------------------------------------------
export CC
export CXX
export CFLAGS
export LUA_HOME
export SO
export AR
export CLIBS
# -------------------------------------------------


BUILD_TOP   = $(shell pwd)
LIB_SO      = lunar.so
LIB_A       = liblunar.a

export LUNAR_SO = $(BUILD_TOP)/src/$(LIB_SO)
export LUNAR_A  = $(BUILD_TOP)/src/$(LIB_A)

INSTALL_SO  = $(INSTALL_TOP)/lib/$(LIB_SO)
INSTALL_A   = $(INSTALL_TOP)/lib/$(LIB_A)

H1 = lunar.h
H2 = numarray.h

HEADERS = \
	$(INSTALL_TOP)/include/$(H1) \
	$(INSTALL_TOP)/include/$(H2)


default : $(LUNAR_SO) $(LUNAR_A)

config : 
	@echo "AR = $(AR)"
	@echo "CC = $(CC)"
	@echo "CXX = $(CXX)"
	@echo "FPIC = $(FPIC)"
	@echo "WARN = $(WARN)"
	@echo "OPTIM = $(OPTIM)"
	@echo "DEBUG = $(DEBUG)"
	@echo "LUA_HOME = $(LUA_HOME)"
	@echo "INSTALL_TOP = $(INSTALL_TOP)"

test : $(LUNAR_SO) $(LUNAR_A)

all : default test

install : $(INSTALL_SO) $(INSTALL_A) $(HEADERS)

$(INSTALL_TOP)/include/$(H1) : 
	mkdir -p $(INSTALL_TOP)/include
	cp src/$(H1) $(INSTALL_TOP)/include

$(INSTALL_TOP)/include/$(H2) : 
	mkdir -p $(INSTALL_TOP)/include
	cp src/$(H2) $(INSTALL_TOP)/include

$(LUNAR_SO) : FORCE
	@make -C src $(LUNAR_SO)

$(LUNAR_A) : FORCE
	@make -C src $(LUNAR_A)

test : FORCE
	@make -C test

$(INSTALL_SO) : $(LUNAR_SO)
	mkdir -p $(INSTALL_TOP)/lib
	cp $(LUNAR_SO) $(INSTALL_TOP)/lib

$(INSTALL_A) : $(LUNAR_A)
	mkdir -p $(INSTALL_TOP)/lib
	cp $(LUNAR_A) $(INSTALL_TOP)/lib

clean :
	make -C test clean
	make -C src clean
	rm -rf lib include

FORCE :
