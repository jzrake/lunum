

#include <stdlib.h>
#include "lunum.h"
#include "lauxlib.h"


static int test_upcast(lua_State *L);
static int test_astable(lua_State *L);
static int test_checkarray(lua_State *L);


int main()
{
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  luaopen_lunum(L);

  lua_register(L, "test_upcast", test_upcast);
  lua_register(L, "test_astable", test_astable);
  lua_register(L, "test_checkarray", test_checkarray);

  if (luaL_dofile(L, "test/capi.lua")) {
    printf("%s\n", lua_tostring(L, -1));
  }

  lua_close(L);
  return 0;
}


int test_upcast(lua_State *L)
{
  enum ArrayType T = luaL_checkinteger(L, 2);

  if (lunum_upcast(L, 1, T, 0)) {
    lua_replace(L, 1);
  }

  lua_settop(L, 1);
  return 1;
}

int test_astable(lua_State *L)
{
  int N = 100;
  struct Array A = array_new_zeros(N, ARRAY_TYPE_DOUBLE);
  lunum_pusharray1(L, &A);

  lunum_astable(L, -1);
  lua_replace(L, -2);

  return 1;
}

int test_checkarray(lua_State *L)
{
  int narg = lua_gettop(L);
  for (int i=1; i<=narg; ++i) {
    int N;
    double *data = (double*)lunum_checkarray2(L, i, ARRAY_TYPE_DOUBLE, &N);
    printf("argument %d had length %d\n", i, N);
  }
  return 0;
}
