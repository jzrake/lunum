

LUA_HOME = /usr/local
LUA = -I$(LUA_HOME)/include -L$(LUA_HOME)/lib -llua

EXE = capi cplus

default : $(EXE)

capi : capi.c ../src/liblunar.a
	$(CC) $(CFLAGS) -std=c99 -o $@ $< -I../src -L../src -llunar $(LUA) -lm

cplus : cplus.cpp ../src/liblunar.a
	$(CXX) $(CFLAGS) -o $@ $< -I../src -L../src -llunar $(LUA) -lm

clean :
	rm -f $(EXE)
