


# ------------------------------------------------------------------------------
#
# command line macros:
#
# INSTALL_HOME ... place to put includes and library files
# LUA_HOME     ... where to find lua header and library files 
#
# ------------------------------------------------------------------------------


INSTALL_HOME := $(shell pwd)

LIB_SO      = numlua.so
LIB_A       = libnumlua.a

NUMLUA_SO   = src/$(LIB_SO)
NUMLUA_A    = src/$(LIB_A)

INSTALL_SO  = $(INSTALL_HOME)/lib/$(LIB_SO)
INSTALL_A   = $(INSTALL_HOME)/lib/$(LIB_A)

H1 = numlua_capi.h
H2 = numarray.h

HEADERS = \
	$(INSTALL_HOME)/include/$(H1) \
	$(INSTALL_HOME)/include/$(H2)

default : $(NUMLUA_SO) $(NUMLUA_A)

install : $(INSTALL_SO) $(INSTALL_A) $(HEADERS)

$(INSTALL_HOME)/include/$(H1) : 
	mkdir -p $(INSTALL_HOME)/include
	cp src/$(H1) $(INSTALL_HOME)/include

$(INSTALL_HOME)/include/$(H2) : 
	mkdir -p $(INSTALL_HOME)/include
	cp src/$(H2) $(INSTALL_HOME)/include

$(NUMLUA_SO) : FORCE
	@make -C src $(LIB_SO)

$(NUMLUA_A) : FORCE
	@make -C src $(LIB_A)

$(INSTALL_SO) : $(NUMLUA_SO)
	mkdir -p $(INSTALL_HOME)/lib
	cp $(NUMLUA_SO) $(INSTALL_HOME)/lib

$(INSTALL_A) : $(NUMLUA_A)
	mkdir -p $(INSTALL_HOME)/lib
	cp $(NUMLUA_A) $(INSTALL_HOME)/lib

clean :
	make -C src clean
	rm -rf lib include

FORCE :

