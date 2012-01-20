


OSNAME       := $(shell uname)
INSTALL_HOME := $(shell pwd)/..

LUA_HOME      = /usr/local
CFLAGS        = -Wall -O2 -fPIC



# Configuration for the host platform
# ------------------------------------------------------------------------------
ifeq ($(OSNAME), Linux)
SO = $(CC) -O -shared
endif

ifeq ($(OSNAME), Darwin)
SO = $(CC) -O -bundle -undefined dynamic_lookup 
endif


OBJ_C = numarray.o numlua.o
NUMLUA_SO = $(INSTALL_HOME)/numlua.so

default : $(NUMLUA_SO) numlua_api

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $<

numlua.o : numlua.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

numlua_api : numlua_api.c numlua.c numarray.c
	$(CC) $(CFLAGS) -std=c99 $< -o $@ -L$(LUA_HOME)/lib -I$(LUA_HOME)/include -llua

$(NUMLUA_SO) : $(OBJ_C)
	$(SO) -o $(NUMLUA_SO) $^

clean :
	rm -f $(OBJ_C) $(NUMLUA_SO) numlua_api
