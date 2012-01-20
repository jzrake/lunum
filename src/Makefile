




# Configuration for the host platform
# ------------------------------------------------------------------------------
OSNAME       := $(shell uname)
LUA_HOME      = /usr/local
CFLAGS        = -Wall -O2 -fPIC

ifeq ($(OSNAME), Linux)
SO = $(CC) -O -shared
AR = ar rcu
endif

ifeq ($(OSNAME), Darwin)
SO = $(CC) -O -bundle -undefined dynamic_lookup 
AR = ar rcu
endif
# ------------------------------------------------------------------------------


NUMLUA_SO = numlua.so
NUMLUA_A  = libnumlua.a


default : $(NUMLUA_A) $(NUMLUA_SO)

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $<

numlua.o : numlua.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

numlua_capi.o : numlua_capi.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

$(NUMLUA_A) : numarray.o numlua.o numlua_capi.o
	$(AR) $@ $?

$(NUMLUA_SO) : numarray.o numlua.o
	$(SO) -o $(NUMLUA_SO) $^

clean :
	rm -f *.o $(NUMLUA_A) $(NUMLUA_SO)
