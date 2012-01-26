

LUA = lua

default : $(LUNAR_A) $(LUNAR_SO)

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $<

lunar.o : lunar.c array_class.lc
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

lunar_capi.o : lunar_capi.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

array_class.o : array_class.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

$(LUNAR_A)  : numarray.o lunar.o lunar_capi.o array_class.o
	$(AR) $@ $?

$(LUNAR_SO) : numarray.o lunar.o lunar_capi.o array_class.o
	$(SO) -o $(LUNAR_SO) $^

array_class.lc : array_class.lua
	$(LUA) bin2c.lua +$< > $@

clean :
	rm -f *.o $(LUNAR_A) $(LUNAR_SO) array_class.lc
