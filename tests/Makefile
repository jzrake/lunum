

LUA      = -I$(LUA_HOME)/include -L$(LUA_HOME)/lib -llua
LUNAR    = -I../src -L../src -llunar 

EXE = capi cplus

default : $(EXE)

capi : capi.c ../src/liblunar.a
	$(CC) $(CFLAGS) -std=c99 -o $@ $< $(LUNAR) $(LUA) $(CLIBS)

cplus : cplus.cpp ../src/liblunar.a
	$(CXX) $(CFLAGS) -o $@ $< $(LUNAR) $(LUA) $(CLIBS)

clean :
	rm -f $(EXE)
