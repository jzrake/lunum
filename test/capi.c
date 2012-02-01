


#include "lunum.h"
#include "lauxlib.h"


static int upcast(lua_State *L);


int main()
{
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  luaopen_lunum(L);

  lua_register(L, "upcast", upcast);

  if (luaL_dofile(L, "test/capi.lua")) {
    printf("%s\n", lua_tostring(L, -1));
  }

  lua_close(L);
  return 0;
}


int upcast(lua_State *L)
{
  enum ArrayType T = luaL_checkinteger(L, 2);

  if (lunum_upcast(L, 1, T, 0)) {
    lua_replace(L, 1);
  }

  lua_settop(L, 1);
  return 1;
}
