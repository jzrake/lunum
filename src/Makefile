

CFLAGS = -Wall -O2

SRC_C = $(wildcard *.c)
OBJ_C = $(SRC_C:.c=.o)

NUMLUA_SO = ../lib/numlua.so

default : $(NUMLUA_SO)

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $<

$(NUMLUA_SO) : $(OBJ_C)
	$(CC) -bundle -undefined dynamic_lookup -o $(NUMLUA_SO) $^

clean :
	rm -f $(OBJ_C) $(NUMLUA_SO)
