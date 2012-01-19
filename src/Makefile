

LUA_HOME = /usr/local
CFLAGS = -Wall -O2

SRC_C = $(wildcard *.c)
OBJ_C = $(SRC_C:.c=.o)

NUMLUA_SO = ../lib/numlua.so

default : $(NUMLUA_SO)


numarray.o : numarray.c
	$(CC) $(CFLAGS) -std=c99 -c $<

numlua.o : numlua.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include


$(NUMLUA_SO) : $(OBJ_C)
	$(CC) -bundle -undefined dynamic_lookup -o $(NUMLUA_SO) $^

clean :
	rm -f $(OBJ_C) $(NUMLUA_SO)
