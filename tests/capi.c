


#include "lunar.h"
#include "lauxlib.h"


static int upcast(lua_State *L);
void _stack_dump(lua_State *L);

int main()
{
  lua_State *L = lua_open();
  luaL_openlibs(L);

  lua_register(L, "upcast", upcast);

  if (luaL_dofile(L, "tests/capi.lua")) {
    printf("%s\n", lua_tostring(L, -1));
  }

  lua_close(L);
  return 0;
}


int upcast(lua_State *L)
{
  enum ArrayType T = luaL_checkinteger(L, 2);

  if (lunar_upcast(L, 1, T, 0)) {
    lua_replace(L, 1);
  }

  lua_settop(L, 1);
  return 1;
}
