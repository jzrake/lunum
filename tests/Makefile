

LUA_HOME = /usr/local

default : capi

capi : capi.c ../src/liblunar.a
	$(CC) $(CFLAGS) -std=c99 -o $@ $< -I../src -L../src -llunar -L$(LUA_HOME)/lib -llua 

clean :
	rm -f capi
