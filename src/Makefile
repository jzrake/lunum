




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


LUNAR_SO = lunar.so
LUNAR_A  = liblunar.a


default : $(LUNAR_A) $(LUNAR_SO)

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $<

lunar.o : lunar.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

lunar_capi.o : lunar_capi.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

$(LUNAR_A)  : numarray.o lunar.o lunar_capi.o
	$(AR) $@ $?

$(LUNAR_SO) : numarray.o lunar.o lunar_capi.o
	$(SO) -o $(LUNAR_SO) $^

clean :
	rm -f *.o $(LUNAR_A) $(LUNAR_SO)
