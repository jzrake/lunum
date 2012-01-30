
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


# Configuration for the host platform
# ------------------------------------------------------------------------------
export OSNAME       = $(shell uname)
export LUA_HOME    ?= /usr/local
export CFLAGS       = -Wall -O2 -fPIC

ifeq ($(OSNAME), Linux)
export SO    = $(CC) -O -shared
export AR    = ar rcu
export CLIBS = -lm -ldl
endif

ifeq ($(OSNAME), Darwin)
export SO    = $(CC) -O -bundle -undefined dynamic_lookup 
export AR    = ar rcu
export CLIBS = 
endif
# ------------------------------------------------------------------------------


INSTALL_TOP = $(shell pwd)
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
