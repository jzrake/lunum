

LUA_HOME = /usr/local
LUA = -I$(LUA_HOME)/include -L$(LUA_HOME)/lib -llua

default : capi

capi : capi.c ../src/liblunar.a
	$(CC) $(CFLAGS) -std=c99 -o $@ $< -I../src -L../src -llunar $(LUA) -lm

clean :
	rm -f capi
