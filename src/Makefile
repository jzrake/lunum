

LUA = $(LUA_HOME)/bin/lua

default : $(LUNUM_A) $(LUNUM_SO)

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $<

lunum.o : lunum.c array_class.lc
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

lunum_capi.o : lunum_capi.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

array_class.o : array_class.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

$(LUNUM_A)  : numarray.o lunum.o lunum_capi.o array_class.o
	$(AR) $@ $?

$(LUNUM_SO) : numarray.o lunum.o lunum_capi.o array_class.o
	$(SO) -o $(LUNUM_SO) $^

array_class.lc : array_class.lua
	$(LUA) bin2c.lua +$< > $@

clean :
	rm -f *.o $(LUNUM_A) $(LUNUM_SO) *.lc
