
# ------------------------------------------------------------------------------
#
# Command line macros:
#
# INSTALL_HOME ... place to put includes and library files
# LUA_HOME     ... where to find lua header and library files 
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


INSTALL_HOME := $(shell pwd)

LIB_SO      = lunar.so
LIB_A       = liblunar.a

LUNAR_SO    = src/$(LIB_SO)
LUNAR_A     = src/$(LIB_A)

INSTALL_SO  = $(INSTALL_HOME)/lib/$(LIB_SO)
INSTALL_A   = $(INSTALL_HOME)/lib/$(LIB_A)

H1 = lunar.h
H2 = numarray.h

HEADERS = \
	$(INSTALL_HOME)/include/$(H1) \
	$(INSTALL_HOME)/include/$(H2)



default : $(LUNAR_SO) $(LUNAR_A)

tests : $(LUNAR_SO) $(LUNAR_A)

all : default tests

install : $(INSTALL_SO) $(INSTALL_A) $(HEADERS)

$(INSTALL_HOME)/include/$(H1) : 
	mkdir -p $(INSTALL_HOME)/include
	cp src/$(H1) $(INSTALL_HOME)/include

$(INSTALL_HOME)/include/$(H2) : 
	mkdir -p $(INSTALL_HOME)/include
	cp src/$(H2) $(INSTALL_HOME)/include

$(LUNAR_SO) : FORCE
	@make -C src $(LIB_SO)

$(LUNAR_A) : FORCE
	@make -C src $(LIB_A)

tests : FORCE
	@make -C tests

$(INSTALL_SO) : $(LUNAR_SO)
	mkdir -p $(INSTALL_HOME)/lib
	cp $(LUNAR_SO) $(INSTALL_HOME)/lib

$(INSTALL_A) : $(LUNAR_A)
	mkdir -p $(INSTALL_HOME)/lib
	cp $(LUNAR_A) $(INSTALL_HOME)/lib

clean :
	make -C tests clean
	make -C src clean
	rm -rf lib include

FORCE :
