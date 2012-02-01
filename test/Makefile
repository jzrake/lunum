

LUA      = -I$(LUA_HOME)/include -L$(LUA_HOME)/lib -llua
LUNUM    = -I../src -L../src -llunum 

EXE = capi cplus

default : $(EXE)

capi : capi.c ../src/liblunum.a
	$(CC) $(CFLAGS) -std=c99 -o $@ $< $(LUNUM) $(LUA) $(CLIBS)

cplus : cplus.cpp ../src/liblunum.a
	$(CXX) $(CFLAGS) -o $@ $< $(LUNUM) $(LUA) $(CLIBS)

clean :
	rm -f $(EXE)
